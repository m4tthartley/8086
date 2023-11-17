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
#define OPCODE_MOV_MEM_ACC	0b10100000
#define OPCODE_MOV_ACC_MEM	0b10100010
#define OPCODE_MOV_RM_SEG	0b10001110
#define OPCODE_MOV_SEG_RM	0b10001100

// typedef union {
// 	u16
// } register_t;
u16 registers[8] = {0};

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

#define ax 0
#define cx 1
#define dx 2
#define bx 3
#define sp 4
#define bp 5
#define si 6
#define di 7

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
			// if (swap) {
			// 	rm += reg;
			// 	reg = rm - reg;
			// 	rm -= reg;
			// }
			// char* dest_reg = register_names[(wide ? 0 : 8) + reg2];
			// char* src_reg = register_names[(wide ? 0 : 8) + reg1];
			if (mode == 0b11) {
				core_print_inline("mov ");
				core_print_inline(get_register_name(regdst, wide));
				core_print_inline(", %s \n", get_register_name(regsrc, wide));
			} else  {
				int reg_table[] = {
					bx,
					bx,
					bp,
					bp,
					si,
					di,
					bp,
					bx
				};
				int reg_plus_table[] = {
					si,
					di,
					si,
					di,
					-1,
					-1,
					-1,
					-1,
				};

				if (mode == 0 && rm == 0b110) {
					// direct address
					core_error("unrecognised opcode");
					exit(1);
				} else {
					u8 data8 = *ip;
					u16 data16 = *(u16*)ip;
					ip += mode;

					core_print_inline("mov ");
					if (dest) {
						core_print_inline(get_register_name(reg, wide));
						core_print_inline(", [%s", register_names[reg_table[rm]]);
						if (rm < 4) {
							core_print_inline(" + %s", register_names[reg_plus_table[rm]]);
						}
						if (mode == 0b01) {
							core_print_inline(" + %hhu", data8);
						}
						if (mode == 0b10) {
							core_print_inline(" + %hu", data16);
						}
						core_print_inline("]");
						core_print_inline("\n");
					} else {
						core_print_inline("[%s", register_names[reg_table[rm]]);
						if (rm < 4) {
							core_print_inline(" + %s", register_names[reg_plus_table[rm]]);
						}
						if (mode == 0b01) {
							core_print_inline(" + %hhu", data8);
						}
						if (mode == 0b10) {
							core_print_inline(" + %hu", data16);
						}
						core_print_inline("], ");
						core_print_inline(get_register_name(reg, wide));
						core_print_inline("\n");
					}
					
					// switch (mode) {
					// case 0b00:
					// 	core_print_inline("mov ");
					// 	if (dest) {
					// 		core_print_inline(get_register_name(reg, wide));
					// 		core_print_inline(", [%s", register_names[reg_table[rm]]);
					// 		if (rm < 4) {
					// 			core_print_inline(" + %s", register_names[reg_plus_table[rm]]);
					// 		}
					// 		core_print_inline("]");
					// 		core_print_inline("\n");
					// 	} else {
					// 		core_print_inline("[%s", register_names[reg_table[rm]]);
					// 		if (rm < 4) {
					// 			core_print_inline(" + %s", register_names[reg_plus_table[rm]]);
					// 		}
					// 		core_print_inline("], ");
					// 		core_print_inline(get_register_name(reg, wide));
					// 		core_print_inline("\n");
					// 	}
					// 	break;
					// case 0b01:
					// 	u8 data8 = *ip++;
					// 	core_print_inline("mov %s, [%s",
					// 		get_register_name(reg, wide),
					// 		register_names[reg_table[rm]]);
					// 		if (rm < 4) {
					// 			core_print_inline(" + %s", register_names[reg_plus_table[rm]]);
					// 		}
					// 		core_print(" + %hhu]", data8);
					// 		// reg_plus_table[rm]>0 ? register_names[reg_plus_table[rm]] : "_",
					// 		// data8);
					// 	break;
					// case 0b10:
					// 	u16 data16 = *(u16*)ip;
					// 	ip += 2;
					// 	core_print_inline("mov %s, [%s",
					// 		get_register_name(reg, wide),
					// 		register_names[reg_table[rm]]);
					// 		if (rm < 4) {
					// 			core_print_inline(" + %s", register_names[reg_plus_table[rm]]);
					// 		}
					// 		core_print(" + %hu]", data16);
					// 	break;
					// }

					// switch (reg2) {
					// case 0b000:
					// 	core_print("mov %s, [bx + si]", dest_reg);
					// 	break;
					// case 0b001:
					// 	core_print("mov %s, [bx + di]", dest_reg);
					// 	break;
					// case 0b010:
					// 	core_print("mov %s, [bp + si]", dest_reg);
					// 	break;
					// case 0b011:
					// 	core_print("mov %s, [bp + di]", dest_reg);
					// 	break;
					// case 0b100:
					// 	core_print("mov %s, [si]", dest_reg);
					// 	break;
					// case 0b101:
					// 	core_print("mov %s, [di]", dest_reg);
					// 	break;
					// case 0b111:
					// 	core_print("mov %s, [bx]", dest_reg);
					// 	break;
					// }
				}
			}
			// if (mode == 0b01) {

			// }
			// if (mode == 0b10) {

			// }
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

	run_program(lesson2part1);

	int x = 0;
}
