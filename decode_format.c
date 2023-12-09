//
//  decode_format.c
//  8086
//
//  Created by Matt Hartley on 27/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "8086def.h"

#define INSTRUCTION(name, fields, ...) {#name, fields, __VA_ARGS__},
#define BITS(bits) {FIELD_LITERAL, sizeof(#bits)-1, 0b##bits}
#define D {FIELD_D, 1}
#define W {FIELD_W, 1}
#define MOD {FIELD_MOD, 2}
#define REG {FIELD_REG, 3}
#define RM {FIELD_RM, 3}
#define SR {FIELD_SR, 2}

#define DATA {FIELD_DATA, 0, 1}
#define DATA_IF_W {FIELD_DATA_IF_W, 0, 1}
// #define DISP8 {FIELD_DISP8, 8}
// #define DISP16 {FIELD_DISP16, 16}

#define DEFAULT_D(d) {FIELD_D, 0, d}
#define DEFAULT_W(w) {FIELD_W, 0, w}
#define DEFAULT_MOD(mod) {FIELD_MOD, 0, mod}
#define DEFAULT_REG(reg) {FIELD_REG, 0, reg}
#define DEFAULT_RM(rm) {FIELD_RM, 0, rm}

#define IM {FIELD_IM, 0, 1}

instruction_format_t instruction_formats[] = {
	// Register/memory to/from register
	INSTRUCTION(mov, {BITS(100010), D, W, MOD, REG, RM})
	// Immediate to register/memory
	INSTRUCTION(mov, {BITS(1100011), W, MOD, BITS(000), RM, DATA, DATA_IF_W, DEFAULT_D(0)})
	// Immediate to register
	INSTRUCTION(mov, {BITS(1011), W, REG, DATA, DATA_IF_W, DEFAULT_D(1)})
	// Memory to accumulator
	INSTRUCTION(mov, {BITS(1010000), W, DEFAULT_MOD(0), DEFAULT_REG(0), DEFAULT_RM(0b110), DEFAULT_D(1)})
	// Accumulator to memory 
	INSTRUCTION(mov, {BITS(1010001), W, DEFAULT_MOD(0), DEFAULT_REG(0), DEFAULT_RM(0b110), DEFAULT_D(0)})
	// Register/memory to segment register
	INSTRUCTION(mov, {BITS(10001110), MOD, BITS(0), SR, RM, DEFAULT_D(1), DEFAULT_W(1)})
	// Segment register to register/memory
	INSTRUCTION(mov, {BITS(10001100), MOD, BITS(0), SR, RM, DEFAULT_D(0), DEFAULT_W(1)})
};
int instruction_format_count = array_size(instruction_formats);
