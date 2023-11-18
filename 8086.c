//
//  8086.c
//  8086
//
//  Created by Matt Hartley on 16/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <core/core.h>

/*
	Instruction encodings

	MOV register/memory
	   6    1 1    2   3  3
	[100010 D W] [MOD REG RM] [LOW DISP] [HIGH DISP] [LOW DATA] [HIGH DATA]

	MOV immidiate to register
	  4   1  3
	[1011 W REG] [LOW DISP] [HIGH DISP] [LOW DATA] [HIGH DATA]
*/

#define OPCODE_MASK 0b11111100
#define D_MASK 0b00000010
#define W_MASK 0b00000001
// #define REG_MODE_MASK 0b11000000
// #define REG_DST_MASK 0b00111000
// #define REG_SRC_MASK 0b00000111

#define OPCODE_MOV			0b10001000
#define OPCODE_MOV_IM_RM	0b11000110
#define OPCODE_MOV_IM		0b10110000
#define OPCODE_MOV_MEM_TO_ACC	0b10100000
#define OPCODE_MOV_ACC_TO_MEM	0b10100010
#define OPCODE_MOV_RM_SEG	0b10001110
#define OPCODE_MOV_SEG_RM	0b10001100

// typedef union {
// 	u16
// } register_t;
// This is so you can use index -1 to get zero
// And it's 16 instead of just 9 to keep it aligned nicely
u16 registers_raw[16] = {
	// padding
	0, 0, 0, 0, 0, 0, 0, 0,
	// registers
	0, 0, 0, 0, 0, 0, 0, 0,
};
u16* registers = registers_raw + 8;

#define ax 0
#define cx 1
#define dx 2
#define bx 3
#define sp 4
#define bp 5
#define si 6
#define di 7

int rm_table[] = {
	bx,
	bx,
	bp,
	bp,
	si,
	di,
	bp,
	bx
};
int rm_plus_table[] = {
	si,
	di,
	si,
	di,
	-1,
	-1,
	-1,
	-1,
};

char* register_names[] = {
	"ax",
	"cx",
	"dx",
	"bx",
	"sp",
	"bp",
	"si",
	"di",

	"al",
	"cl",
	"dl",
	"bl",
	"ah",
	"ch",
	"dh",
	"bh",
};

#define get_register_name(reg, wide) (register_names[(wide ? 0 : 8) + reg])

typedef struct {
	u8* instructions;
	int size;
} program_t;

program_t load_program(char* path) {
	core_handle_t file = core_open(path);
	core_stat_t stat = core_stat(file);
	u8* data = malloc(stat.size);
	core_read(file, 0, data, stat.size);
	core_close(file);
	program_t result = {
		data,
		stat.size
	};
	return result;
}

void run_program(program_t program) {
	u8* ip = program.instructions;
	while (ip < program.instructions+program.size) {
		u8 opcode = *ip++;

		// Immediate the register
		if ((opcode & 0b11110000) == OPCODE_MOV_IM) {
			u8 wide = (opcode & 0b1000) >> 3;
			u8 reg = opcode & 0b111;
			if (wide) {
				u16 data = *(u16*)ip;
				ip += 2;
				char* dest_reg = register_names[(wide ? 0 : 8) + reg];
				core_print("mov %s, %hu", dest_reg, data);
			} else {
				u8 data = *ip++;
				char* dest_reg = register_names[(wide ? 0 : 8) + reg];
				core_print("mov %s, %hhu", dest_reg, data);
			}
			continue;
		}

		// Register/memory to/from register
		if ((opcode & 0b11111100) == OPCODE_MOV) {
			u8 reg_code = *ip++;
			b32 dest = (opcode&D_MASK) >> 1;
			b32 wide = 	opcode&W_MASK;
			u8 mode = 	(reg_code&0b11000000) >> 6;
			u8 reg = 	(reg_code&0b00111000) >> 3;
			u8 rm = 	(reg_code&0b00000111);
			u8 fields[] = {reg, rm};
			u8 regdst = fields[1-dest];
			u8 regsrc = fields[dest];
			
			if (mode == 0b11) {
				core_print_inline("mov ");
				core_print_inline(get_register_name(regdst, wide));
				core_print_inline(", %s \n", get_register_name(regsrc, wide));
			} else  {
				if (mode == 0 && rm == 0b110) {
					// direct address
					// core_error("OPCODE_MOV direct address");
					// exit(1);
					i16 disp16 = *(i16*)ip;
					ip += 2;

					if (dest) {
						core_print_inline("mov ");
						core_print_inline(get_register_name(reg, wide));
						core_print_inline(", [%hu]", disp16);
						core_print_inline("\n");
					} else {
						core_print_inline("mov ");
						core_print_inline("[%hu], ", disp16);
						core_print_inline(get_register_name(reg, wide));
						core_print_inline("\n");
					}
				} else {
					i8 disp8 = *ip;
					i16 disp16 = *(i16*)ip;
					ip += mode;

					core_print_inline("mov ");
					if (dest) {
						core_print_inline(get_register_name(reg, wide));
						core_print_inline(", [%s", register_names[rm_table[rm]]);
						if (rm < 4) {
							core_print_inline(" + %s", register_names[rm_plus_table[rm]]);
						}
						if (mode == 0b01) {
							core_print_inline(" + %hhi", disp8);
						}
						if (mode == 0b10) {
							core_print_inline(" + %hi", disp16);
						}
						core_print_inline("]");
						core_print_inline("\n");
					} else {
						core_print_inline("[%s", register_names[rm_table[rm]]);
						if (rm < 4) {
							core_print_inline(" + %s", register_names[rm_plus_table[rm]]);
						}
						if (mode == 0b01) {
							core_print_inline(" + %hhi", disp8);
						}
						if (mode == 0b10) {
							core_print_inline(" + %hi", disp16);
						}
						core_print_inline("], ");
						core_print_inline(get_register_name(reg, wide));
						core_print_inline("\n");
					}
				}
			}
			continue;
		}

		// Immediate to register/memory
		if ((opcode & 0b11111110) == OPCODE_MOV_IM_RM) {
			u8 reg_code = *ip++;
			b32 wide = 	opcode&W_MASK;
			u8 mode = 	(reg_code&0b11000000) >> 6;
			// u8 reg = 	(reg_code&0b00111000) >> 3;
			u8 rm = 	(reg_code&0b00000111);
			// u8 fields[] = {reg, rm};
			// u8 regdst = fields[1-dest];
			// u8 regsrc = fields[dest];

			if (mode == 0b11) {
				i8 data8 = *ip;
				i16 data16 = *(i16*)ip;
				ip += 1+wide;
				core_print_inline("mov ");
				core_print_inline("%s,  \n", get_register_name(rm, wide));
				if (wide) {
					core_print_inline("word %hi", data16);
				} else {
					core_print_inline("byte %hhi", data8);
				}
			} else {
				i8 disp8 = *ip;
				i16 disp16 = *(i16*)ip;
				ip += mode;

				i8 data8 = *ip;
				i16 data16 = *(i16*)ip;
				ip += 1+wide;

				if (mode == 0 && rm == 0b110) {
					// direct address
					core_error("OPCODE_MOV_IM_RM direct address");
					exit(1);
				} else {
					core_print_inline("mov ");
					core_print_inline("[%s", register_names[rm_table[rm]]);
					if (rm < 4) {
						core_print_inline(" + %s", register_names[rm_plus_table[rm]]);
					}
					if (mode == 0b01) {
						core_print_inline(" + %hhi", disp8);
					}
					if (mode == 0b10) {
						core_print_inline(" + %hi", disp16);
					}
					core_print_inline("]");
					if (wide) {
						core_print_inline(", word %hi", data16);
					} else {
						core_print_inline(", byte %hhi", data8);
					}
					core_print_inline("\n");
				}
			}
			continue;
		}

		if ((opcode & 0b11111110) == OPCODE_MOV_MEM_TO_ACC) {
			b32 wide = 	opcode&W_MASK;
			i16 disp16 = *(i16*)ip;
			ip += 2;

			core_print_inline("mov ");
			if (wide) {
				core_print_inline("ax");
			} else {
				core_print_inline("al");
			}
			core_print_inline(", [%hi]", disp16);
			core_print_inline("\n");

			continue;
		}

		if ((opcode & 0b11111110) == OPCODE_MOV_ACC_TO_MEM) {
			b32 wide = 	opcode&W_MASK;
			i16 disp16 = *(i16*)ip;
			ip += 2;

			core_print_inline("mov ");
			core_print_inline("[%hi], ", disp16);
			if (wide) {
				core_print_inline("ax");
			} else {
				core_print_inline("al");
			}
			core_print_inline("\n");

			continue;
		}

		core_error("unrecognised opcode");
		exit(1);
	}
}

int main() {
	program_t single = load_program("data/listing_0037_single_register_mov");
	program_t many = load_program("data/listing_0038_many_register_mov");

	program_t lesson2part1 = load_program("data/listing_0039_more_movs");
	program_t lesson2part2 = load_program("data/listing_0040_challenge_movs");

	// core_print("single program");
	// run_program(single);

	run_program(lesson2part2);

	int x = 0;
}
