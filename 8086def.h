//
//  8086def.h
//  8086
//
//  Created by Matt Hartley on 27/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <core/core.h>

typedef enum {
	FIELD_NULL,
	FIELD_LITERAL,
	FIELD_D,
	FIELD_W,
	FIELD_MOD,
	FIELD_REG,
	FIELD_RM,
	FIELD_SR,
	FIELD_DISP8,
	FIELD_DISP16,
	FIELD_DATA,
	FIELD_DATA_IF_W,

	FIELD_IM,

	FIELD_COUNT,
} /*__attribute__ ((__packed__))*/ instruction_field_type_t;

typedef struct {
	instruction_field_type_t type;
	u8 size;
	u16 value;
} instruction_field_t;

typedef struct {
	char* name;
	instruction_field_t fields[8];
} instruction_format_t;

typedef struct {
	enum : u8 {
		OPERAND_REGISTER,
		OPERAND_MEMORY,
		OPERAND_DIRECT_ADDRESS,
		OPERAND_IMMEDIATE,
	} type;
	u8 reg;
	u16 disp;
	union {
		u16 data;
		struct {
			u8 low;
			u8 high;
		} data8;
	};
} operand_t;

typedef struct {
	u16 size;
	// u16 fields[16];
	operand_t operands[2];
	b32 wide;
} instruction_t;

typedef struct {
	u8* instructions;
	u8* ip;
	int size;
} program_t;


void decode_program(program_t program);

void display_instruction(instruction_t inst);
