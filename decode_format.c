//
//  decode_format.c
//  8086
//
//  Created by Matt Hartley on 27/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "8086def.h"

#define INSTRUCTION(name, fields, ...) {#name, fields, __VA_ARGS__},
#define BITS(bits) {BITS_LITERAL, sizeof(#bits)-1, 0b##bits}
#define D {BITS_D, 1}
#define W {BITS_W, 1}
#define MOD {BITS_MOD, 2}
#define REG {BITS_REG, 3}
#define RM {BITS_RM, 3}
#define SR {BITS_SR, 2}

#define DATA {BITS_DATA, 8}
#define WDATA {BITS_WDATA, 8}
#define DISP8 {BITS_DISP8, 8}
#define DISP16 {BITS_DISP16, 16}

#define DEFAULT_D(d) {BITS_D, 0, d}
#define DEFAULT_W(w) {BITS_W, 0, w}
#define DEFAULT_MOD(mod) {BITS_MOD, 0, mod}
#define DEFAULT_REG(reg) {BITS_REG, 0, reg}
#define DEFAULT_RM(rm) {BITS_RM, 0, rm}

instruction_format_t instruction_formats[] = {
	INSTRUCTION(mov, {BITS(100010), D, W, MOD, REG, RM})
	INSTRUCTION(mov, {BITS(1100011), W, MOD, BITS(000), RM, DATA, WDATA, DEFAULT_D(0)})
	INSTRUCTION(mov, {BITS(1011), W, REG, DATA, WDATA, DEFAULT_D(1)})

	INSTRUCTION(mov, {BITS(1010000), W, DEFAULT_MOD(0), DEFAULT_REG(0), DEFAULT_RM(110), DEFAULT_D(1)})
	INSTRUCTION(mov, {BITS(1010001), W, DEFAULT_MOD(0), DEFAULT_REG(0), DEFAULT_RM(110), DEFAULT_D(0)})
	INSTRUCTION(mov, {BITS(10001110), MOD, BITS(0), SR, RM, DISP16, DEFAULT_D(1), DEFAULT_W(1)})
	INSTRUCTION(mov, {BITS(10001100), MOD, BITS(0), SR, RM, DISP16, DEFAULT_D(0), DEFAULT_W(1)})
};
int instruction_format_count = array_size(instruction_formats);
