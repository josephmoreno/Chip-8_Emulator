#pragma once

#include <vector>
#include <ctime>

struct chip8_arch {
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
	
	void initialize() {
		pc = 0x0200;	// Chip-8 ROMs start at address 0x0200.

		unsigned int x;
		for(x = 0; x < 0x50; ++x) {
			mem[x + 0x50] = fontset[x];	// Load fontset into memory, starting at mem[0x50].
		}

		mem.assign(4096, 0x00);
		gen_reg.assign(16, 0x00);
		addr_reg = 0x0000;
		//flag0 = 0x00;
		d_reg = 0x00;
		s_reg = 0x00;
		sp = 0x00;
		stack.assign(16, 0x0000);
		display.assign(64 * 32, 0x00);
		keys.assign(16, 0x00);
		dispFlag = 0x00;

		initialized = true;

		return;
	}

	unsigned short temp_op = 0x0000;	// Used for debugging.
	unsigned short temp_pc = 0x0000;	// Used for debugging.
	void emulateCycle() {
		// Fetch
		unsigned short opcode = ((mem[pc] << 8) | mem[pc + 1]);
		unsigned char MSH = mem[pc] & 0xf0;
		
		unsigned short s0 = 0x0000;
		unsigned char x = 0x00, y = 0x00, disp_pix = 0x00, sprite_pix = 0x00;
		unsigned char c0 = 0x00, c1 = 0x00, c2 = 0x00;

		srand(time(0));	// Used for random numbers.

		temp_op = opcode;
		temp_pc = pc;

		// Decode and Execute (based off the most significant hex number)
		switch (MSH) {
			case 0x00:
				// Clears the screen.
				if (opcode == 0x00e0) {
					display.assign(64 * 32, 0x00);
					dispFlag = 0x01;

					pc += 2;

				// Return from the subroutine.
				}else if (opcode == 0x00ee) {
					pc = stack[sp];
					stack[sp] = 0x0000;
					if (sp > 0) --sp;
				}

				// Opcode 0x0NNN calls an RCA 1802 program at address 0x0NNN; not necessary for most ROMs
				// Implement later if necessary.

				break;

			case 0x10:
				// opcode == 0x1NNN; set PC to address 0x0NNN to jump to that instruction.
				pc = opcode & 0x0fff;

				break;

			case 0x20:
				// opcode == 0x2NNN; set PC to address 0x0NNN to call that subroutine.
				if (stack[sp] == 0x0000)
					stack[sp] = (pc + 0x0002);
				else {
					++sp;
					stack[sp] = (pc + 0x0002);
				}

				pc = opcode & 0x0fff;

				break;

			case 0x30:
				// opcode == 0x3XYY; if gen_reg[0x0X] == 0xYY, then skip the next instruction.
				if (gen_reg[(mem[pc] & 0x0f)] == mem[pc + 1])
					pc += 4;
				else
					pc += 2;

				break;

			case 0x40:
				// opcode == 0x4XYY; if gen_reg[0x0X] != 0xYY, then skip the next instruction.
				if (gen_reg[(mem[pc] & 0x0f)] != mem[pc + 1])
					pc += 4;
				else
					pc += 2;

				break;

			case 0x50:
				// opcode == 0x5XY0; if gen_reg[0x0X] == gen_reg[0x0Y], then skip the next instruction.
				if (gen_reg[(mem[pc] & 0x0f)] == gen_reg[(mem[pc + 1] >> 4)])
					pc += 4;
				else
					pc += 2;

				break;

			case 0x60:
				// opcode == 0x6XNN; set gen_reg[0x0X] = 0xNN.
				gen_reg[(mem[pc] & 0x0f)] = mem[pc + 1];
				pc += 2;

				break;

			case 0x70:
				// opcode == 0x7XNN; adds 0xNN to gen_reg[0x0X] and carry flag (flag0) is left unchanged.
				gen_reg[(mem[pc] & 0x0f)] += mem[pc + 1];
				pc += 2;

				break;

			case 0x80:
				// opcode == 0x8XY0; sets gen_reg[0x0X] = gen_reg[0x0Y].
				if ((mem[pc + 1] & 0x0f) == 0x00) {
					gen_reg[(mem[pc] & 0x0f)] = gen_reg[(mem[pc + 1] >> 4)];
					pc += 2;

				// opcode == 0x8XY1; sets gen_reg[0x0X] = gen_reg[0x0X] | gen_reg[0x0Y].
				}else if ((mem[pc + 1] & 0x0f) == 0x01) {
					gen_reg[(mem[pc] & 0x0f)] = gen_reg[(mem[pc] & 0x0f)] | gen_reg[(mem[pc + 1] >> 4)];
					pc += 2;

				// opcode == 0x8XY2; sets gen_reg[0x0X] = gen_reg[0x0X] & gen_reg[0x0Y].
				}else if ((mem[pc + 1] & 0x0f) == 0x02) {
					gen_reg[(mem[pc] & 0x0f)] = gen_reg[(mem[pc] & 0x0f)] & gen_reg[(mem[pc + 1] >> 4)];
					pc += 2;

				// opcode == 0x8XY3; sets gen_reg[0x0X] = gen_reg[0x0X] XOR gen_reg[0x0Y].
				}else if ((mem[pc + 1] & 0x0f) == 0x03) {
					gen_reg[(mem[pc] & 0x0f)] = gen_reg[(mem[pc] & 0x0f)] ^ gen_reg[(mem[pc + 1] >> 4)];
					pc += 2;

				// opcode == 0x8XY4; gen_reg[0x0X] += gen_reg[0x0Y] and the carry flag (flag0) is changed accordingly.
				}else if ((mem[pc + 1] & 0x0f) == 0x04) {
					gen_reg[(mem[pc] & 0x0f)] += gen_reg[(mem[pc + 1] >> 4)];
					s0 = gen_reg[(mem[pc] & 0x0f)] + gen_reg[(mem[pc + 1] >> 4)];

					if ((s0 & 0xff00) >= 0x0100)
						gen_reg[0x0f] = 0x01; //flag0 = 0x01;
					else
						gen_reg[0x0f] = 0x00; //flag0 = 0x00;
					
					s0 = 0x0000;
					pc += 2;

				// opcode == 0x8XY5; gen_reg[0x0X] -= gen_reg[0x0Y] and the borrow flag (flag0)
				// is changed to 1 if gen_reg[0x0X] >= gen_reg[0x0Y]. Otherwise, flag0 = 0.
				}else if ((mem[pc + 1] & 0x0f) == 0x05) {
					if (gen_reg[(mem[pc] & 0x0f)] >= gen_reg[(mem[pc + 1] >> 4)]) {
						gen_reg[(mem[pc] & 0x0f)] -= gen_reg[(mem[pc + 1] >> 4)];
						gen_reg[0x0f] = 0x01; //flag0 = 0x01;
					}else {
						gen_reg[(mem[pc] & 0x0f)] = 0x00;
						gen_reg[0x0f] = 0x00; //flag0 = 0x00;
					}

					pc += 2;

				// opcode == 0x8X06; flag0 equals the LSB of gen_reg[0x0X]
				// and gen_reg[0x0X] = gen_reg[0x0X] >> 1.
				}else if ((mem[pc + 1] & 0x0f) == 0x06) {
					gen_reg[0x0f] = gen_reg[(mem[pc] & 0x0f)] & 0x01; //flag0 = gen_reg[(mem[pc] & 0x0f)] & 0x01;
					gen_reg[(mem[pc] & 0x0f)] = gen_reg[(mem[pc] & 0x0f)] >> 1;

					pc += 2;

				// opcode == 0x8XY7; gen_reg[0x0X] = gen_reg[0x0Y] - gen_reg[0x0X].
				// If gen_reg[0x0Y] >= gen_reg[0x0X], then flag0 = 0x01. Else, flag0 = 0x00.
				}else if ((mem[pc + 1] & 0x0f) == 0x07) {
					if (gen_reg[(mem[pc + 1] >> 4)] >= gen_reg[(mem[pc] & 0x0f)]) {
						gen_reg[(mem[pc] & 0x0f)] = gen_reg[(mem[pc + 1] >> 4)] - gen_reg[(mem[pc] & 0x0f)];
						gen_reg[0x0f] = 0x01; //flag0 = 0x01;
					}else {
						gen_reg[(mem[pc] & 0x0f)] = 0x00;
						gen_reg[0x0f] = 0x00; //flag0 = 0x00;
					}

					pc += 2;

				// opcode == 0x8X0e; flag0 equals the MSB of gen_reg[0x0X]
				// and gen_reg[0x0X] = gen_reg[0x0X] << 1.
				}else if ((mem[pc + 1] & 0x0f) == 0x0e) {
					gen_reg[0x0f] = gen_reg[(mem[pc] & 0x0f)] >> 7; //flag0 = gen_reg[(mem[pc] & 0x0f)] >> 7;
					gen_reg[(mem[pc] & 0x0f)] = gen_reg[(mem[pc] & 0x0f)] << 1;

					pc += 2;
				}else 
					printf("Unknown opcode: 0x%x\n", opcode);
				
				break;

			case 0x90:
				// opcode == 0x9XY0; next instruction is skipped if gen_reg[0x0X] != gen_reg[0x0Y].
				if ((mem[pc + 1] & 0x0f) == 0x00) {
					if (gen_reg[(mem[pc] & 0x0f)] != gen_reg[(mem[pc + 1] >> 4)])
						pc += 4;
					else
						pc += 2;
				}else
					printf("Unknown opcode: 0x%x\n", opcode);

				break;

			case 0xa0:
				// opcode == 0xaNNN; sets mem_addr = 0x0NNN
				addr_reg = opcode & 0x0fff;
				pc += 2;

				break;

			case 0xb0:
				// opcode = 0xbNNN; sets pc = gen_reg[0x00] + 0x0NNN
				pc = gen_reg[0x00] + (opcode & 0x0fff);

				break;

			case 0xc0:
				// opcode = 0xcXNN; gen_reg[0x0X] = (random number) & 0xNN
				c0 = rand() % 256;
				gen_reg[(mem[pc] & 0x0f)] = c0 & mem[pc + 1];
				
				c0 = 0x00;
				pc += 2;

				break;

			case 0xd0:
				// opcode = 0xdXYN; draws sprite at coordinates (gen_reg[0x0X], gen_reg[0x0Y]).
				// Sprite width is fixed at 8 pixels and height = 0x0N pixels. Sprite is read
				// from addr_reg. The sprite's pixels are XOR'd with the corresponding positions
				// (pixels) in the display vector.
				
				x = gen_reg[(mem[pc] & 0x0f)];		// X-coordinate for display.
				y = gen_reg[(mem[pc + 1] >> 4)];	// Y-coordinate for display.
				c0 = mem[pc + 1] & 0x0f;		// Height of the sprite.

				gen_reg[0x0f] = 0x00;
				//flag0 = 0x00;	// flag0 stays as 0x00 if none of the display's pixels go from 0x01 to 0x00.
						// If at least 1 pixel goes from 0x01 to 0x00, flag0 = 0x01.

				for(c1 = 0x00; c1 < c0; ++c1) {		// Height loop.
					for(c2 = 0x00; c2 < 0x08; ++c2) {	// Width loop.
						disp_pix = display[(64 * (c1 + y)) + (x + c2)];
						sprite_pix = (mem[addr_reg + c1] >> (0x07 - c2)) & 0x01;

						if (disp_pix == 0x01 && sprite_pix == 0x01)
							gen_reg[0x0f] = 0x01; //flag0 = 0x01;

						display[(64 * (c1 + y)) + (x + c2)] ^= (mem[addr_reg + c1] >> (0x07 - c2)) & 0x01;
					}
				}

				// Reset variables to 0.
				x = 0x00; y = 0x00;
				disp_pix = 0x00; sprite_pix = 0x00;
				c0 = 0x00; c1 = 0x00; c2 = 0x00;

				dispFlag = 0x01;	// Tell SDL to render to and update the screen.
				pc += 2;

				break;

			case 0xe0:
				// opcode == 0xeX9e; if keys[gen_reg[0x0X]] == 0x01, then skip the next instruction.
				if (mem[pc + 1] == 0x9e) {
					if (keys[gen_reg[(mem[pc] & 0x0f)]] == 0x01)
						pc += 4;
					else
						pc += 2;

				// opcode == 0xeXa1; if keys[gen_reg[0x0X]] == 0x00, then skip the next instruction.
				}else if (mem[pc + 1] == 0xa1) {
					if (keys[gen_reg[(mem[pc] & 0x0f)]] == 0x00)
						pc += 4;
					else
						pc += 2;
				}else
					printf("Unknown opcode: 0x%x\n", opcode);

				break;

			case 0xf0:
				// opcode == 0xfX07; sets gen_reg[0x0X] = d_reg.
				if (mem[pc + 1] == 0x07) {
					gen_reg[(mem[pc] & 0x0f)] = d_reg;

					pc += 2;

				// opcode == 0xfX0a; waits for a key press then stores it in gen_reg[0x0X].
				}else if (mem[pc + 1] == 0x0a) {
					if (keyDown == 0x01) {
						gen_reg[(mem[pc] & 0x0f)] = keyPressed;
					
						keyDown = 0x00;
						pc += 2;
					}

				// opcode == 0xfX15; sets d_reg = gen_reg[0x0X].
				}else if (mem[pc + 1] == 0x15) {
					d_reg = gen_reg[(mem[pc] & 0x0f)];

					pc += 2;

				// opcode == 0xfX18; sets s_reg = gen_reg[0x0X].
				}else if (mem[pc + 1] == 0x18) {
					s_reg = gen_reg[(mem[pc] & 0x0f)];

					pc += 2;

				// opcode == 0xfX1e; sets addr_reg += gen_reg[0x0X] and if the sum is > 0x0fff, then flag0 = 0x01.
				}else if (mem[pc + 1] == 0x1e) {
					addr_reg += gen_reg[(mem[pc] & 0x0f)];

					if (addr_reg > 0x0fff)
						gen_reg[0x0f] = 0x01; //flag0 = 0x01;
					else
						gen_reg[0x0f] = 0x00; //flag0 = 0x00;

					pc +=2;

				// opcode == 0xfX29; sets addr_reg to the sprite address corresponding to the hexadecimal number
				// stored in gen_reg[0x0X]. For example, if gen_reg[0x0X] == 0x00, then addr_reg = 0x0050.
				}else if (mem[pc + 1] == 0x29) {
					switch(gen_reg[(mem[pc] & 0x0f)]) {
						case 0x00:
							addr_reg = 0x0050;
							break;

						case 0x01:
							addr_reg = 0x0055;
							break;

						case 0x02:
							addr_reg = 0x005a;
							break;

						case 0x03:
							addr_reg = 0x005f;
							break;

						case 0x04:
							addr_reg = 0x0064;
							break;

						case 0x05:
							addr_reg = 0x0069;
							break;

						case 0x06:
							addr_reg = 0x006e;
							break;

						case 0x07:
							addr_reg = 0x0073;
							break;

						case 0x08:
							addr_reg = 0x0078;
							break;

						case 0x09:
							addr_reg = 0x007d;
							break;

						case 0x0a:
							addr_reg = 0x0082;
							break;

						case 0x0b:
							addr_reg = 0x0087;
							break;

						case 0x0c:
							addr_reg = 0x008c;
							break;

						case 0x0d:
							addr_reg = 0x0091;
							break;

						case 0x0e:
							addr_reg = 0x0096;
							break;

						case 0x0f:
							addr_reg = 0x009b;
							break;

						default: printf("Hexadecimal sprite couldn't be loaded. Opcode: %x\n", opcode);
					}

					pc += 2;

				// opcode == 0xfX33; stores gen_reg[0x0X] as BCD with the hundreds place at mem[addr_reg], tens place at
				// mem[addr_reg + 1], and ones place mem[addr_reg + 2].
				}else if (mem[pc + 1] == 0x33) {
					// Hundreds place...
					c0 = gen_reg[(mem[pc] & 0x0f)] / 100;
					mem[addr_reg] = c0;

					// Tens place...
					c0 = gen_reg[(mem[pc] & 0x0f)] % 100;
					c0 = c0 / 10;
					mem[addr_reg + 1] = c0;

					// Ones place...
					c0 = c0 % 10;
					mem[addr_reg + 2] = c0;

					c0 = 0x00;
					pc += 2;

				// opcode == 0xfX55; stores gen_reg[0x00] through gen_reg[0x0X] in memory starting at the address
				// stored in addr_reg.
				}else if (mem[pc + 1] == 0x55) {
					for(c0 = 0x00; c0 <= (mem[pc] & 0x0f); ++c0) {
						mem[addr_reg + c0] = gen_reg[c0];
					}

					c0 = 0x00;
					pc += 2;

				// opcode == 0xfX65; fills gen_reg[0x00] through gen_reg[0x0X] from memory starting at the address
				// stored in addr_reg.
				}else if (mem[pc + 1] == 0x65) {
					for(c0 = 0x00; c0 <= (mem[pc] & 0x0f); ++c0) {
						gen_reg[c0] = mem[addr_reg + c0];
					}

					c0 = 0x00;
					pc += 2;
				}else
					printf("Unknown opcode: 0x%x\n", opcode);

				break;

			default: printf("Unknown opcode: 0x%x\n", opcode);
		}

		if (d_reg > 0x00) 
			--d_reg;

		if (s_reg > 0x00)
			--s_reg;

		return;
	}
	
	void insertRom(std::vector<unsigned char> buffer) {
		unsigned int x;
		unsigned int addr = 0x200;

		for (x = 0; x < buffer.size(); ++x) {
			mem[addr] = buffer[x];
			++addr;
		}

		return;
	}

	void printContents() {
		unsigned int x;
		
		printf("===> PC: 0x%x\n", temp_pc);
		printf("Opcode: 0x%x\n\n", temp_op);

		printf("General Registers:\n");
		for(x = 0; x < 16; ++x)
			printf("%d: 0x%x\n", x, gen_reg[x]);

		printf("\nAddress Register: 0x%x\n\n", addr_reg);

		printf("Flag Register: 0x%x\n\n", gen_reg[0x0f] /*flag0*/);

		printf("Delay Register: 0x%x\n", d_reg);
		printf("Sound Register: 0x%x\n\n", s_reg);

		printf("Stack Pointer: 0x%x\nStack:\n", sp);
		for(x = 0; x < 16; ++x) 
			printf("%d: 0x%x\n", x, stack[x]);

		printf("\n");

		return;
	}
};
