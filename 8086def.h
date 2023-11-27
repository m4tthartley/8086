//
//  8086def.h
//  8086
//
//  Created by Matt Hartley on 27/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <core/core.h>

typedef enum {
	BITS_NULL,
	BITS_LITERAL,
	BITS_D,
	BITS_W,
	BITS_MOD,
	BITS_REG,
	BITS_RM,
	BITS_SR,
	BITS_DISP8,
	BITS_DISP16,
	BITS_DATA,
	BITS_WDATA,

	BITS_END,
} __attribute__ ((__packed__)) instruction_field_type_t;

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
	u16 size;
	u16 fields[16];
} instruction_t;

typedef struct {
	u8* instructions;
	u8* ip;
	int size;
} program_t;


void decode_program(program_t program);
