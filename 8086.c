//
//  8086.c
//  8086
//
//  Created by Matt Hartley on 16/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

/*
	Instruction encodings

	MOV register/memory
	   6    1 1    2   3  3
	[100010 D W] [MOD REG RM] [LOW DISP] [HIGH DISP] [LOW DATA] [HIGH DATA]

	MOV immidiate to register
	  4   1  3
	[1011 W REG] [LOW DISP] [HIGH DISP] [LOW DATA] [HIGH DATA]
*/

#include "8086def.h"

#if 0
#define OPCODE_MASK 0b11111100
#define D_MASK 0b00000010
#define S_MASK 0b00000010
#define W_MASK 0b00000001
// #define REG_MODE_MASK 0b11000000
// #define REG_DST_MASK 0b00111000
// #define REG_SRC_MASK 0b00000111

#define OPCODE_MOV				0b10001000
// #define OPCODE_MOV_IM_RM		0b11000110
#define OPCODE_MOV_IM_RM		0b11000100 // Hoping the 2nd bit isn't important, the destination is always 1
#define OPCODE_MOV_IM			0b10110000
#define OPCODE_MOV_MEM_TO_ACC	0b10100000
#define OPCODE_MOV_ACC_TO_MEM	0b10100010
#define OPCODE_MOV_RM_SEG		0b10001110
#define OPCODE_MOV_SEG_RM		0b10001100

#define OPCODE_ADD_REG_RM		0b00000000
#define OPCODE_ADD_IM_TO_RM		0b10000000
#define OPCODE_ADD_IM_TO_ACC	0b00000100

#define OPCODE_SUB_REG_RM		0b00101000
#define OPCODE_SUB_IM_TO_RM		0b10000000
#define OPCODE_SUB_IM_TO_ACC	0b00101100

#define OPCODE_CMP_REG_RM		0b00111000
#define OPCODE_CMP_IM_TO_RM		0b10000000
#define OPCODE_CMP_IM_TO_ACC	0b00111100

#define OPCODE_JNZ				0b01110101
#define OPCODE_JE				0b01110100
#define OPCODE_JL				0b01111100
#define OPCODE_JLE				0b01111110
#define OPCODE_JB				0b01110010
#define OPCODE_JBE				0b01110110
#define OPCODE_JP				0b01111010
#define OPCODE_JO				0b01110000
#define OPCODE_JS				0b01111000
// #define OPCODE_JNE				0b01110101
#define OPCODE_JNL				0b01111101
#define OPCODE_JG				0b01111111
#define OPCODE_JNB				0b01110011
#define OPCODE_JA				0b01110111
#define OPCODE_JNP				0b01111011
#define OPCODE_JNO				0b01110001
#define OPCODE_JNS				0b01111001
#define OPCODE_LOOP				0b11100010
#define OPCODE_LOOPZ			0b11100001
#define OPCODE_LOOPNZ			0b11100000
#define OPCODE_JCXZ				0b11100011

u8 jumps8[] = {
	OPCODE_JNZ,
	OPCODE_JE,
	OPCODE_JL,
	OPCODE_JLE,
	OPCODE_JB,
	OPCODE_JBE,
	OPCODE_JP,
	OPCODE_JO,
	OPCODE_JS,
	// OPCODE_JNE,
	OPCODE_JNL,
	OPCODE_JG,
	OPCODE_JNB,
	OPCODE_JA,
	OPCODE_JNP,
	OPCODE_JNO,
	OPCODE_JNS,
	OPCODE_LOOP,
	OPCODE_LOOPZ,
	OPCODE_LOOPNZ,
	OPCODE_JCXZ,
};

char* jumps8_str[] = {
	"jnz",
	"je",
	"jl",
	"jle",
	"jb",
	"jbe",
	"jp",
	"jo",
	"js",
	// "jne",
	"jnl",
	"jg",
	"jnb",
	"ja",
	"jnp",
	"jno",
	"jns",
	"loop",
	"loopz",
	"loopnz",
	"jcxz",
};

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

// char* get_rm_string(u8 opcode, u8 reg_code, u8 rm) {
// #define get_rm_string(rm) \
// 	if (mode == 0b11) {\
// 		return get_register_name(rm, wide);\
// 	} else {\
// 		if (mode == 0 && rm == 0b110) {\
// 			i16 disp16 = *(i16*)ip;
// 			ip += 2;

// 		} else {\

// 		}\
// 	}\
// }

#define print_rm() \
	core_print_inline("[%s", register_names[rm_table[rm]]); \
	if (rm < 4) { \
		core_print_inline(" + %s", register_names[rm_plus_table[rm]]); \
	} \
	if (mode == 0b01) { \
		core_print_inline(" + %hhi", disp8); \
	} \
	if (mode == 0b10) { \
		core_print_inline(" + %hi", disp16); \
	} \
	core_print_inline("]");

void print_bin8(u8 bin) {
	char result[12] = {0};
	// u64 result;
	result[0] = '0';
	result[1] = 'b';
	result[10] = ' ';
	FOR (i, 8) {
		// ((char*)(&result))[i] = (1<<i) & bin ? '1' : '0';
		result[2+i] = (1<<(7-i)) & bin ? '1' : '0';
	}

	core_print_inline(result);
}

void run_program(program_t program) {
	u8* ip = program.instructions;
	while (ip < program.instructions+program.size) {
		u8 opcode = *ip++;

		// print 8 bit binary opcode
		print_bin8(opcode);

		// Immediate to register
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

		u8 opcode6 = (opcode & 0b11111100);

		// Register/memory to/from register
		if (opcode6 == OPCODE_MOV
			|| opcode6 == OPCODE_ADD_REG_RM
			|| opcode6 == OPCODE_SUB_REG_RM
			|| opcode6 == OPCODE_CMP_REG_RM) {
			u8 reg_code = *ip++;
			b32 dest = (opcode&D_MASK) >> 1;
			b32 wide = 	opcode&W_MASK;
			u8 mode = 	(reg_code&0b11000000) >> 6;
			u8 reg = 	(reg_code&0b00111000) >> 3;
			u8 rm = 	(reg_code&0b00000111);
			u8 fields[] = {reg, rm};
			u8 regdst = fields[1-dest];
			u8 regsrc = fields[dest];
			
			switch (opcode6) {
			case OPCODE_MOV:
				core_print_inline("mov ");
				break;
			case OPCODE_ADD_REG_RM:
				core_print_inline("add ");
				break;
			case OPCODE_SUB_REG_RM:
				core_print_inline("sub ");
				break;
			case OPCODE_CMP_REG_RM:
				core_print_inline("cmp ");
				break;
			}
			
			if (mode == 0b11) {
				// core_print_inline("mov ");
				core_print_inline(get_register_name(regdst, wide));
				core_print_inline(", %s \n", get_register_name(regsrc, wide));
			} else  {
				if (mode == 0 && rm == 0b110) {
					// direct address
					i16 disp16 = *(i16*)ip;
					ip += 2;

					if (dest) {
						// core_print_inline("mov ");
						core_print_inline(get_register_name(reg, wide));
						core_print_inline(", [%hu]", disp16);
						core_print_inline("\n");
					} else {
						// core_print_inline("mov ");
						core_print_inline("[%hu], ", disp16);
						core_print_inline(get_register_name(reg, wide));
						core_print_inline("\n");
					}
				} else {
					i8 disp8 = *ip;
					i16 disp16 = *(i16*)ip;
					ip += mode;

					// core_print_inline("mov ");
					if (dest) {
						core_print_inline(get_register_name(reg, wide));
						core_print_inline(", ");
						print_rm();
						core_print_inline("\n");
					} else {
						print_rm();
						core_print_inline(", ");
						core_print_inline(get_register_name(reg, wide));
						core_print_inline("\n");
					}
				}
			}
			continue;
		}

		if ((opcode & 0b11111100) == OPCODE_ADD_REG_RM) {

		}

		// Immediate to register/memory
		if (opcode6 == OPCODE_MOV_IM_RM
			|| opcode6 == OPCODE_ADD_IM_TO_RM) {
			u8 reg_code = *ip++;
			b32 sign = (opcode&S_MASK) >> 1;
			// In the immediate add instruction, for some reason sign has to be 0
			// for it to use the second byte, I don't understand why
			b32 wide = (opcode&W_MASK);// && (opcode6==OPCODE_ADD_IM_TO_RM ? !sign : TRUE);
			u8 mode = 	(reg_code&0b11000000) >> 6;
			u8 reg = 	(reg_code&0b00111000) >> 3;
			u8 rm = 	(reg_code&0b00000111);
			// u8 fields[] = {reg, rm};
			// u8 regdst = fields[1-dest];
			// u8 regsrc = fields[dest];

			switch (opcode6) {
			case OPCODE_MOV_IM_RM:
				core_print_inline("mov ");
				break;
			case OPCODE_ADD_IM_TO_RM:
				switch (reg) {
				case 0b000:
					core_print_inline("add ");
					break;
				case 0b101:
					core_print_inline("sub ");
					break;
				case 0b111:
					core_print_inline("cmp ");
					break;
				}
				break;
			}

			if (mode == 0b11) {
				i8 data8 = *ip;
				i16 data16 = *(i16*)ip;
				ip += 1 + ((opcode6==OPCODE_MOV_IM_RM) ? wide : (!sign && wide));
				// core_print_inline("mov ");
				core_print_inline("%s", get_register_name(rm, wide));
				if ((opcode6==OPCODE_MOV_IM_RM) ? wide : (!sign && wide)) {
					core_print_inline(", word %hi", data16);
				} else {
					core_print_inline(", byte %hhi", data8);
				}
				core_print_inline("\n");
			} else {
				i8 disp8 = *ip;
				i16 disp16 = *(i16*)ip;
				ip += (mode == 0 && rm == 0b110) ? 2 : mode;

				i8 data8 = *ip;
				i16 data16 = *(i16*)ip;
				ip += 1 + ((opcode6==OPCODE_MOV_IM_RM) ? wide : (!sign && wide));

				if (mode == 0 && rm == 0b110) {
					// direct address
					// i16 disp16 = *(i16*)ip;
					// ip += 2;

					// core_print_inline(get_register_name(reg, wide));
					core_print_inline("[%hu]", disp16);
					if ((opcode6==OPCODE_MOV_IM_RM) ? wide : (!sign && wide)) {
						core_print_inline(", word %hi", data16);
					} else {
						core_print_inline(", byte %hhi", data8);
					}
					core_print_inline("\n");
				} else {
					// core_print_inline("mov ");

					// core_print_inline("[%s", register_names[rm_table[rm]]);
					// if (rm < 4) {
					// 	core_print_inline(" + %s", register_names[rm_plus_table[rm]]);
					// }
					// if (mode == 0b01) {
					// 	core_print_inline(" + %hhi", disp8);
					// }
					// if (mode == 0b10) {
					// 	core_print_inline(" + %hi", disp16);
					// }
					// core_print_inline("]");
					print_rm();
					if ((opcode6==OPCODE_MOV_IM_RM) ? wide : (!sign && wide)) {
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

		if (opcode6 == OPCODE_ADD_IM_TO_ACC
			|| opcode6 == OPCODE_SUB_IM_TO_ACC
			|| opcode6 == OPCODE_CMP_IM_TO_ACC) {
			b32 wide = 	opcode&W_MASK;
			i16 disp16 = *(i16*)ip;
			ip += 1+wide;

			switch (opcode) {
			case OPCODE_ADD_IM_TO_ACC:
				core_print_inline("add ");
				break;
			case OPCODE_SUB_IM_TO_ACC:
				core_print_inline("sub ");
				break;
			case OPCODE_CMP_IM_TO_ACC:
				core_print_inline("cmp ");
				break;
			}
			
			if (wide) {
				core_print_inline("ax");
				core_print_inline(", %hi", disp16);
			} else {
				core_print_inline("al");
				core_print_inline(", %hhi", disp16);
			}
			core_print_inline("\n");

			continue;
		}

		// if (opcode == OPCODE_JNZ) {
		// 	i8 data8 = *ip++;
		// 	core_print("jnz %hhi", data8);
		// 	continue;
		// }
		// if (opcode == OPCODE_JE) {
		// 	i8 data8 = *ip++;
		// 	core_print("je %hhi", data8);
		// 	continue;
		// }
		// if (opcode == OPCODE_JL) {
		// 	i8 data8 = *ip++;
		// 	core_print("jl %hhi", data8);
		// 	continue;
		// }
		// if (opcode == OPCODE_JLE) {
		// 	i8 data8 = *ip++;
		// 	core_print("jle %hhi", data8);
		// 	continue;
		// }

		FOR (i, array_size(jumps8)) {
			if (opcode == jumps8[i]) {
				i8 data8 = *ip++;
				core_print("%s %hhi", jumps8_str[i], data8);
				goto next;
			}
		}

		core_error("unrecognised opcode");
		exit(1);
next:
		continue;
	}
}
#endif

program_t load_program(char* path) {
	program_t result = {0};
	core_handle_t file = core_open(path);
	if (file) {
		core_stat_t stat = core_stat(file);
		u8* data = malloc(stat.size);
		core_read(file, 0, data, stat.size);
		core_close(file);
		result.instructions = data;
		result.size = stat.size;
	}
	return result;
}

int main() {
	char buffer[1024];
	core_allocator_t allocator = core_allocator(buffer, sizeof(buffer));
	core_use_allocator(&allocator);

	program_t single = load_program("data/listing_0037_single_register_mov");
	program_t many = load_program("data/listing_0038_many_register_mov");

	program_t lesson2part1 = load_program("data/listing_0039_more_movs");
	program_t lesson2part2 = load_program("data/listing_0040_challenge_movs");

	program_t lesson3 = load_program("data/listing_0041_add_sub_cmp_jnz");

	// core_print("single program");
	// run_program(single);

#if 0
	run_program(lesson2part1);
	core_print("---- ---- ---- ---- ---- ---- ---- ----");
	run_program(lesson2part2);
	core_print("---- ---- ---- ---- ---- ---- ---- ----");
	run_program(lesson3);
#endif

	// core_print("\nsingle mov");
	// decode_program(single);

	// core_print("\nmany movs");
	// decode_program(many);

	// core_print("\nmore movs");
	// decode_program(lesson2part1);

	core_print("\nchallenge movs");
	decode_program(lesson2part2);

	int x = 0;
}
