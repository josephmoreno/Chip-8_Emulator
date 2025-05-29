#pragma once

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <ctime>

struct Chip8Arch {
	bool initialized = false;

	// *** Chip-8 Specs
	
	// Memory: 4,096 bytes
	std::vector<unsigned char> mem = std::vector<unsigned char>(4096, 0x00);

	// 16 8-bit general-purpose registers
	std::vector<unsigned char> gen_reg = std::vector<unsigned char>(16, 0x00);

	// 16-bit memory address register
	unsigned short addr_reg = 0x0000;

	// 1-bit "VF" register (used as a flag by some instructions)
	unsigned char flag0 = 0x00;	// Can either be 0x00 or 0x01

	// 8-bit registers for delay and sound timers
	unsigned char d_reg = 0x00, s_reg = 0x00;

	// Program counter
	unsigned short pc = 0x0200;	// Chip-8 ROMs start at address 0x0200.

	// Stack pointer
	unsigned char sp = 0x00;

	// Stack
	std::vector<unsigned short> stack = std::vector<unsigned short>(16, 0x0000);

	// Graphics
	std::vector<unsigned char> display = std::vector<unsigned char>(64 * 32, 0x00);

	// Keyboard
	std::vector<unsigned char> keys = std::vector<unsigned char>(16, 0x00);	// key not pressed == 0x00
										// key pressed == 0x01
	
	// Fontset
	std::vector<unsigned char> fontset = std::vector<unsigned char>
	{ 
  		0xf0, 0x90, 0x90, 0x90, 0xf0,	// 0
  		0x20, 0x60, 0x20, 0x20, 0x70,	// 1
  		0xf0, 0x10, 0xf0, 0x80, 0xf0,	// 2
  		0xf0, 0x10, 0xf0, 0x10, 0xf0,	// 3
  		0x90, 0x90, 0xf0, 0x10, 0x10,	// 4
  		0xf0, 0x80, 0xf0, 0x10, 0xf0,	// 5
  		0xf0, 0x80, 0xf0, 0x90, 0xf0,	// 6
  		0xf0, 0x10, 0x20, 0x40, 0x40,	// 7
  		0xf0, 0x90, 0xf0, 0x90, 0xf0,	// 8
  		0xf0, 0x90, 0xf0, 0x10, 0xf0,	// 9
  		0xf0, 0x90, 0xf0, 0x90, 0x90,	// A
  		0xe0, 0x90, 0xe0, 0x90, 0xe0,	// B
  		0xf0, 0x80, 0x80, 0x80, 0xf0,	// C
  		0xe0, 0x90, 0x90, 0x90, 0xe0,	// D
  		0xf0, 0x80, 0xf0, 0x80, 0xf0,	// E
  		0xf0, 0x80, 0xf0, 0x80, 0x80 	// F
	};

	
	// *** Functions and Variables
	
	unsigned char dispFlag = 0x00;		// Used to communicate to SDL when the screen should be updated.
	unsigned char keyDown = 0x00;		// If key pressed, keyDown == 0x01. Otherwise, 0x00.
	unsigned char keyPressed = 0x00;	// Stores the key that's pressed when keyDown == 0x01.
	
	void initialize();

	unsigned short temp_op = 0x0000;	// Used for debugging.
	unsigned short temp_pc = 0x0000;	// Used for debugging.
	void emulateCycle();
	
	void insertRom(std::vector<unsigned char> buffer);

	void printContents();
};
