#include <iostream>

#include <bcm2835.h>
#include <stdio.h>
#include <errno.h>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <bcm2835.h>
#include <stdio.h>
#include <dirent.h>

// define for LCD fnct
#define DISP_CMD 0x0;  // Command for the display
#define RAM_WRITE_CMD  0x40; // Write to display RAM
#define CLEAR_DISP_CMD 0x01; // Clear display command
#define HOME_CMD       0x02; // Set cursos at home (0,0)
#define DISP_ON_CMD    0x0C; // Display on command
#define DISP_OFF_CMD   0x08; // Display off Command
#define SET_DDRAM_CMD  0x80; // Set DDRAM address command
#define CONTRAST_CMD   0x70; // Set contrast LCD command
#define FUNC_SET_TBL0  0x38; // Function set - 8 bit, 2 line display 5x8, inst table 0
#define FUNC_SET_TBL1  0x39; // Function set - 8 bit, 2 line display 5x8, inst table 1

int fd;								// File descriptor

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

	return 0;
}

int sendStr(const char *ptString)   
{   
	char buf[128];
	int index=0, status=-1;
	
	buf[index++]=0x00;  // Send command
	buf[index++]=0x01;  // Clear display
	buf[index++]=0x01;  // Clear display
	status = write(fd, buf, index);
    
   	if ( status != index) {	
		printf("Error writing %d bytes to i2c slave, %d (clear display)\n", index, status);
		return(1);
	}
	usleep(200000);
	
	index=0;	
	//~ printf("Displaying \"%s\"...\n", ptString);
	buf[index++]=RAM_WRITE_CMD;
    while((*ptString)!='\0')   
    {   
		if (index > 127){
			printf("buffer overflow\n");
			break;
		}
		if (*ptString =='\n'){
			while(index<41){
				buf[index++]=' ';
			}
		} else {
			buf[index++]=*ptString;
		}
		*ptString++;
    }
    status = write(fd, buf, index);
    
   	if ( status != index) {	
		printf("Error writing %d bytes to i2c slave, %d\n", index, status);
		return(1);
	}
}

int main(int argc, char **argv)
{
    const char *filename = std::string("/dev/i2c-0").c_str();								// Name of the port we will be using
	int  i2c_address = 0x3E;										// Address of LCD display										
	
	if(init_LCD(filename, i2c_address)) {
		printf("initilization error, exiting...\n");
		return 1;
	}
    if (argc>1){
        sendStr(argv[1]);
    }
}
