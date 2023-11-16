//
//  8086.c
//  8086
//
//  Created by Matt Hartley on 16/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <core/core.h>

#define OPCODE_MASK 0b11111100
#define D_MASK 0b00000010
#define W_MASK 0b00000001
#define REG_MODE_MASK 0b11000000
#define REG_DST_MASK 0b00111000
#define REG_SRC_MASK 0b00000111

#define MOV_OPCODE 0b10001000

// typedef union {
// 	u16
// } register_t;
u16 registers[8] = {0};

char* register_names[] = {
	"al",
	"cl",
	"dl",
	"bl",
	"ah",
	"ch",
	"dh",
	"bh",

	"ax",
	"cx",
	"dx",
	"bx",
	"sp",
	"bp",
	"si",
	"di",
};

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
		u8 regs = *ip++;
		switch (opcode & OPCODE_MASK) {
		case MOV_OPCODE:
			b32 swap = opcode&D_MASK;
			b32 wide = opcode&W_MASK;
			u8 reg1 = (regs & REG_DST_MASK) >> 3;
			u8 reg2 = regs & REG_SRC_MASK;
			if (swap) {
				reg2 += reg1;
				reg1 = reg2 - reg1;
				reg2 -= reg1;
			}
			char* dest_reg = register_names[(wide ? 8 : 0) + reg2];
			char* src_reg = register_names[(wide ? 8 : 0) + reg1];
			// core_print("d=%i w=%i", swap, wide);
			core_print("mov %s, %s", dest_reg, src_reg);
			break;
		}
	}
}

int main() {
	program_t single = load_program("listing_0037_single_register_mov");
	program_t many = load_program("listing_0038_many_register_mov");

	// core_print("single program");
	// run_program(single);
	core_print("many program");
	run_program(many);

	int x = 0;
}
