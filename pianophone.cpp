// pianophone.c
//
// interface a telephone keypad with pd
//
// Author: Antoine Villeret (antoine[DOT]villeret[AT]gmail[DOT]com)
// Copyright (C) 2012-2013 Antoine Villeret
// 2012/11/18 : initial version
// 2013/02/01 : added LCD support

#include "pianophone.hpp"

										
int main(int argc, char **argv)
{
    // If you call this, it will not actually access the GPIO
    // Use for testing
	//bcm2835_set_debug(1);

	uint8_t curr_state[18],last_state[18], change_state[18];
	int i,j, err=0;
	lo_address t;
	
	const char *filename = std::string("/dev/i2c-0").c_str();								// Name of the port we will be using
	int  i2c_address = 0x3E;										// Address of LCD display										
	
	if(init_GPIO() || init_LCD(filename, i2c_address)) {
		return 1;
	}
	
	t=lo_address_new("239.0.0.1","7770"); // multicast the OSC stream, so we can receive note on any computer on the same local network
	
	while (1)
	{	
		//~ scan keypad array
		for(i=0;i<4;i++){
			bcm2835_gpio_set_pud(rowpin[i], BCM2835_GPIO_PUD_OFF);
			bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(rowpin[i], HIGH);
			for(j=0;j<4;j++){
				curr_state[i*4+j] = bcm2835_gpio_lev(colpin[j]);
			}
			bcm2835_gpio_write(rowpin[i], LOW);
			bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_INPT);
			bcm2835_gpio_set_pud(rowpin[i], BCM2835_GPIO_PUD_DOWN);
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
    }
}

int init_GPIO(){
	
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

int init_LCD (const char *fileName, int address) 
{
	int index=0;
	char buf[12], busy=0;
	int status;
	
	printf("open device %s ...", fileName);
	if ((fd = open(fileName, O_RDWR)) < 0) {					// Open port for reading and writing
		printf("\terror\nFailed to open i2c port : %x\n", errno);
		return 1;
	} else {
		printf("\tOK\n");
	}
	//~ sleep(0.05);
	
	printf("connect to slave 0x%02.2X ...",address);
	if (ioctl(fd, I2C_SLAVE, address) < 0) {					// Set the port options and set the address of the device we wish to speak to
		printf("Unable to get bus access to talk to slave\n");
		return 1;
	} else {
		printf("\tOK\n");
	}
	
	printf("initialize display...");

	buf[index++]=0x00;   // Send command to the display
	buf[index++]=FUNC_SET_TBL0;
	buf[index++]=FUNC_SET_TBL1;
	buf[index++]=0x17;  // Set internal OSC frequency to 347 Hz & bias to 1/5
	buf[index++]=0x7F;  // Set contrast to max (low byte)
	buf[index++]=0x5F;  // ICON disp on, Booster on, Contrast high byte 
	buf[index++]=0x6D;  // Follower circuit (internal), amp ratio (6)
	
	busy = read(fd, &busy, 1);
	printf("busy flag : %d, address : %X\n", busy >> 7, busy && 0x7F);
	printf("busy %X\n", busy);

	status = write( fd, buf, index);
	if ( status != index ){
		printf("\terror : %x\nstatus : %d\n", errno, status);
		return -1;
	} else {
		printf("\tOK\n");
	}
	
	sleep(0.2);
	printf("switch on display ...");
	index=0;
	
	buf[index++]=0x00;   // Send command to the display
	buf[index++]=0x0C;  // Display on
	buf[index++]=0x01;  // Clear display
	buf[index++]=0x06;  // Entry mode set - increment
	
	status = write( fd, buf, index);
	if ( status != index ){
		printf("\terror : %x\nstatus : %d\n", errno, status);
		return -1;
	} else {
		printf("\tOK\n");
	}
	const std::string path=std::string(".");
	std::vector <std::string> files = read_directory(path);
	
	sendStr(files[0].c_str());
	
	return 0;
}

std::vector <std::string> read_directory( const std::string& path )
{
	std::vector <std::string> result;
	dirent* de;
	DIR* dp;
	errno = 0;
	dp = opendir( path.empty() ? "." : path.c_str() );
	printf("list file...\n");
	if (dp)
	{
		while (true)
		{
		errno = 0;
		de = readdir( dp );
		if (de == NULL) break;
		std::string tmp = std::string( de->d_name );
		if ( (tmp.length()>3) && tmp.substr(tmp.length()-3)==std::string(".pd") ){ // only catch .pd files
			result.push_back( tmp );
			//~ std::cout << tmp << "\t" << tmp.substr(tmp.length()-3) << std::endl;
		}
	}
    closedir( dp );
    std::sort( result.begin(), result.end() );
    }	
    
  return result;
}

// Write a string to LCD   
void sendStr(const char *ptString)   
{   
	char buf[128];
	int index=0;
		
	printf("Displaying \"%s\"...\n", ptString);
	buf[index++]=RAM_WRITE_CMD;
    while((*ptString)!='\0')   
    {   
		if (index > 127){
			printf("buffer overflow\n");
			break;
		}
        buf[index++]=*ptString++;
    }
    int status = write(fd, buf, index);
    
   	if ( status != index) {	
		printf("Error writing %d bytes to i2c slave, %d\n", index, status);
		exit(1);
	}
}
