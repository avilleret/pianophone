// pianophone.c
//
// interface a telephone keypad with pd
//
// Author: Antoine Villeret (antoine[DOT]villeret[AT]gmail[DOT]com)
// Copyright (C) 2012 Antoine Villeret
// 18.11.2012

#include <bcm2835.h>
#include <stdio.h>
#include <lo/lo.h>

char rowpin[6]={ \
					RPI_GPIO_P1_26, \
					RPI_GPIO_P1_08, \
					RPI_GPIO_P1_23, \
					RPI_GPIO_P1_10, \
					RPI_GPIO_P1_15, \
					RPI_GPIO_P1_12 };

char colpin[6]={ \
					RPI_GPIO_P1_21, \
					RPI_GPIO_P1_19, \
					RPI_GPIO_P1_18, \
					RPI_GPIO_P1_16, \
					RPI_GPIO_P1_13, \
					RPI_GPIO_P1_11 };

static uint8_t keymap[18]={	\
				61 /* 0 */, \
				71 /* 1 */, \
				65 /* 2 */, \
				60 /* 3 */, \
				76 /* 4 */, \
				72 /* 5 */, \
				67 /* 6 */, \
				62 /* 7 */, \
				63 /* 8 */, \
				74 /* 9 */, \
				69 /* 10 */, \
				64 /* 11 */, \
				73 /* 12 */, \
				68 /* 13 */, \
				15 /* 14 */, \
				66 /* 15 */, \
				70 /* 16 */, \
				75 /* 17 */};
										
int main(int argc, char **argv)
{
    // If you call this, it will not actually access the GPIO
    // Use for testing
	//bcm2835_set_debug(1);

	uint8_t curr_state[18],last_state[18], change_state[18];
	int i,j, err=0;
	lo_address t;

	if(init()) {
		return 1;
	}
	t=lo_address_new("239.0.0.1","7770");
	
	while (1)
	{	
		//~ scan keypad array
		for(i=0;i<4;i++){
			bcm2835_gpio_set_pud(rowpin[i], BCM2835_GPIO_PUD_OFF);
			bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(rowpin[i], HIGH);
			for(j=0;j<4;j++){
				curr_state[i*4+j] = bcm2835_gpio_lev(colpin[j]);
				//~ printf("\t[%d,%d]:%d",i,j,curr_state[i*4+j]);
			}
			bcm2835_gpio_write(rowpin[i], LOW);
			bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_INPT);
			bcm2835_gpio_set_pud(rowpin[i], BCM2835_GPIO_PUD_DOWN);
			//~ printf("\n");
		}
		
		//~ scan last 2 switchs
		for(i=4;i<6;i++){
			curr_state[12+i] = bcm2835_gpio_lev(colpin[i]);
			//~ printf("\t%d",curr_state[12+i]);
		}
		
		//~ detect change and send note over OSC
		for(i=0;i<18;i++){
			change_state[i] = curr_state[i] ^ last_state[i];
			last_state[i] = curr_state[i];
			//~ printf("%d ", curr_state[i]);
			if (change_state[i]) {
				err=lo_send(t, "/note", "cc", keymap[i], curr_state[i]);
				//~ printf("/note %d %d\n",keymap[i], curr_state[i]);
			}
		}
	//~ printf("\n");
		// wait a bit
		 delay(1);
    }
}

int init(){
	
	int i;
	if (!bcm2835_init()) {
		printf("can't init bcm2835\n");
		return 1;
	}
	printf("setup GPIO pins\n");
	for(i=0;i<4;i++){
		// Set RPI colpin & rowpin to be an input
		bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_fsel(colpin[i], BCM2835_GPIO_FSEL_INPT);
		//  with a pullup
		bcm2835_gpio_set_pud(rowpin[i], BCM2835_GPIO_PUD_DOWN);
		bcm2835_gpio_set_pud(colpin[i], BCM2835_GPIO_PUD_DOWN);
	}
	for(i=4;i<6;i++){
		bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(rowpin[i], HIGH);
		
		bcm2835_gpio_fsel(colpin[i], BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_set_pud(colpin[i], BCM2835_GPIO_PUD_DOWN);
	}
	
	printf("setup done\n");
	return 0;

}
