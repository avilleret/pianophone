// pianophone.cpp
//
// interface a telephone keypad with pd
//
// Author: Antoine Villeret (antoine[DOT]villeret[AT]gmail[DOT]com)
// Copyright (C) 2012-2013 Antoine Villeret
// 2012/11/18 : initial version
// 2013/02/01 : added LCD support

#include "pianophone.hpp"

//~  prototypes :
int sendStr(const char *ptString, const char *ptString2) ;
int sendStr(const char *ptString) ;
										
int main(int argc, char **argv)
{
    // If you call this, it will not actually access the GPIO
    // Use for testing
	//bcm2835_set_debug(1);

	uint8_t curr_state[20],last_state[20], change_state[20];
	int i,j, err=0, file_index=0;
	lo_address t;
	curr_state[18]=0;
	curr_state[19]=0;
    start_time=0;
	
	const char *filename = std::string("/dev/i2c-0").c_str();								// Name of the port we will be using
	int  i2c_address = 0x3E;										// Address of LCD display										
	
	if(init_GPIO() || init_LCD(filename, i2c_address)) {
		printf("initilization error, exiting...\n");
		return 1;
	}
	
	sendStr("(R)pianophone","V.1.0");
	sleep(2);
	const char *display;
	if ( files.size() > file_index+1 ){
		sendStr(files[file_index].c_str(),files[file_index+1].c_str());
	} else if (files.size()==(file_index+1)){
		sendStr(files[0].c_str(),"");
	} else {
        sendStr("","");
	}
	
	t=lo_address_new("239.0.0.1","7770"); // multicast the OSC stream, so we can receive note on any computer on the same local network
	lo_send(t, "/open", "s", files[file_index].c_str());
    
	while (1)
	{	
		//~ scan keypad array
		for(i=0;i<4;i++){
			bcm2835_gpio_set_pud(rowpin[i], BCM2835_GPIO_PUD_OFF);
			bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_write(rowpin[i], HIGH);
			for(j=0;j<5;j++){
				curr_state[i*5+j] = bcm2835_gpio_lev(colpin[j]);
			}
			bcm2835_gpio_write(rowpin[i], LOW);
			bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_INPT);
			bcm2835_gpio_set_pud(rowpin[i], BCM2835_GPIO_PUD_DOWN);
		}
		
		//~ detect change and send note over OSC
		for(i=0;i<20;i++){ 
            if ( i != 17 ) // there is nothing on 17  
            {
                change_state[i] = curr_state[i] != last_state[i];
                last_state[i] = curr_state[i];
                if (change_state[i]) {
                    err=lo_send(t, "/note", "ii", keymap[i], curr_state[i]);
                }
            }
		}
        
         if (curr_state[BTN_0_INDEX] && curr_state[BTN_1_INDEX]) {
             //~ When you press the two buttons together during at least 3 seconds, the RPi shuts down
            if ( start_time == 0 ){
                start_time = std::clock();
            }
            end_time=std::clock();
            if ( (end_time - start_time) > 3*CLOCKS_PER_SEC ) {
                //~ shutdown the RPi
                sendStr("Shutdown...", "bye bye !");
                system("shutdown -h -P now");
                return 0;
            }
        } else if (curr_state[BTN_0_INDEX] || curr_state[BTN_1_INDEX]) { //~ check buttons state
			if ( change_state[BTN_0_INDEX] && curr_state[BTN_0_INDEX]) file_index--;
			if ( change_state[BTN_1_INDEX] && curr_state[BTN_1_INDEX]) file_index++;
			file_index=(file_index+10000*files.size())%files.size(); //~ add 1000 times files.size() to allow rolling (go from the last to first and vice versa)
            if ( files.size() > file_index+1 ){
                sendStr(files[file_index].c_str(),files[file_index+1].c_str());
            } else if (files.size()==(file_index+1)){
                sendStr(files[file_index].c_str(),"");
            } else {
                sendStr("<no patch found>","");
            }
			err=lo_send(t, "/open", "s", files[file_index].c_str());
		} else {
            start_time = 0;
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
		// Set RPI rowpin to be an input
		bcm2835_gpio_fsel(rowpin[i], BCM2835_GPIO_FSEL_INPT);
		//  with a pullup
		bcm2835_gpio_set_pud(rowpin[i], BCM2835_GPIO_PUD_DOWN);
	}
	for(i=0;i<5;i++){
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
	const std::string path=std::string("./patches");
	files = read_directory(path);
		
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
			std::cout << tmp << std::endl;
		}
	}
    closedir( dp );
    std::sort( result.begin(), result.end() );
    }
    printf("nombre de patches trouvés : %d\n", result.size());
    
  return result;
}

// Write a string to LCD  

int sendStr(const char *ptString)   
{   
    sendStr(ptString, "");
}

int sendStr(const char *ptString, const char *ptString2)   
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
    while(index<21)   
    {   
		if ((*ptString)!='\0'){
			buf[index++]=*ptString++;
        } else {
            buf[index++]=0x20;
        }
    }
    
    index = 41;
    while(index<61)   
    {   
		if ((*ptString2)!='\0'){
			buf[index++]=*ptString2++;
        } else {
            buf[index++]=0x20;
        }
    }
    
    status = write(fd, buf, index);
    
   	if ( status != index) {	
		printf("Error writing %d bytes to i2c slave, %d\n", index, status);
		return(1);
	}
}
