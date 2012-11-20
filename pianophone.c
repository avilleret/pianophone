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
					RPI_GPIO_P1_24, \
					RPI_GPIO_P1_23, \
					RPI_GPIO_P1_22, \
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
				61 /* 1 */, \
				71 /* 2 */, \
				65 /* 3 */, \
				60 /* 4*/, \
				75 /* 5 */, \
				72 /* 6*/, \
				67 /* 7 */, \
				62 /* 8 */, \
				63 /* 9 */, \
				74 /* 10 */, \
				69 /* 11 */, \
				64 /* 12 */, \
				73 /* 13 */, \
				68 /* 14 */, \
				15 /* 15 */, \
				66 /* 16 */, \
				70 /* 17 */, \
				74 /* 18 */};
										
int main(int argc, char **argv)
{
    // If you call this, it will not actually access the GPIO
    // Use for testing
	//bcm2835_set_debug(1);

	uint8_t curr_state[18],last_state[18], change_state[18];
	int i,j;
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
			if (change_state[i]) {
				int err=0;
				err=lo_send(t, "/note", "cc", keymap[i], curr_state[i]);
				printf("/note %d %d\n",keymap[i], curr_state[i]);
			}
		}
	
		// wait a bit
		 //~ delay(1000);
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

