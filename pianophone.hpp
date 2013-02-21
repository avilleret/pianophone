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

// prototypes 
int init_GPIO(void);
int init_LCD (const char *fileName, int address);
int list_patch();
std::vector <std::string> read_directory( const std::string& path = std::string() );
int sendStr(const char *ptString) ;

// global variables
int fd;								// File descriptor
std::vector <std::string> files;	// *.pd files in current directory

char rowpin[4]={ \
					RPI_GPIO_P1_26, \
					RPI_GPIO_P1_08, \
					RPI_GPIO_P1_23, \
					RPI_GPIO_P1_10  };

char colpin[5]={ \
					RPI_GPIO_P1_21, \
					RPI_GPIO_P1_19, \
					RPI_GPIO_P1_18, \
					RPI_GPIO_P1_16, \
					RPI_GPIO_P1_13 };

static uint8_t keymap[20]={	\
				61 /* 0 */, \
				71 /* 1 */, \
				65 /* 2 */, \
				60 /* 3 */, \
				70 /* 4 */, \
				76 /* 5 */, \
				72 /* 6 */, \
				67 /* 7 */, \
				62 /* 8 */, \
				75 /* 9 */, \
				63 /* 10 */, \
				74 /* 11 */, \
				69 /* 12 */, \
				64 /* 13 */, \
				1 /* 14 */, \
				73 /* 15 */, \
				68 /* 16 */, \
				17 /* 17 */, \
				66 /* 18 */, \
				1 /* 19 */  };

#define BTN_0_INDEX 14
#define BTN_1_INDEX 19
