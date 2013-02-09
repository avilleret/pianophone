#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <bcm2835.h>
#include <stdio.h>
#include <lo/lo.h> // liblo is for OSC communication
#include <dirent.h>

// define come LCD fnct
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

// prototypes 
int init_GPIO(void);
int init_LCD (const char *fileName, int address);
int list_patch();
std::vector <std::string> read_directory( const std::string& path = std::string() );
int sendStr(const char *ptString) ;

// global variables
int fd;								// File descriptor
std::vector <std::string> files;	// *.pd files in current directory

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

static uint8_t keymap[20]={	\
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
				0 /* 14 */, \
				66 /* 15 */, \
				70 /* 16 */, \
				75 /* 17 */};
