//
//  decode.c
//  8086
//
//  Created by Matt Hartley on 24/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "8086def.h"

extern instruction_format_t instruction_formats[];
extern int instruction_format_count;

instruction_t decode_instruction(u8* ip) {
	instruction_t instruction = {0};
	FOR (format_index, instruction_format_count) {
		instruction_format_t* format = instruction_formats + format_index;
		u8 bit_index = 0;
		FOR (field_index, array_size(format->fields)) {
			instruction_field_t field = format->fields[field_index];
			u8 byte_offset = bit_index / 8;
			u8 bit_offset = bit_index % 8;

			u8 mask = 0;
			FOR (i, field.size) mask |= (0b1<<(7-(bit_offset+i)));
			u8 value = (ip[byte_offset] & mask) >> (8-(bit_offset+field.size));

			b32 next = FALSE;
			switch (field.type) {
			case BITS_NULL:
				// we reached the end
				// return instruction
				// core_print("format %i", format_index);
				assert(bit_index % 8 == 0);
				// instruction.size = round_up(byte_offset, )
				instruction.size = bit_index/8;
				goto done;
				break;

			case BITS_LITERAL:
				assert(field.size <= (8-bit_offset));
				// for (int i = field.size; i >= 0; --i) {
				//     u8 bit = (0b1 << (8-bit_offset-1))
				// }
				
				if (value != field.value) {
					next = TRUE;
				}
				break;

			case BITS_WDATA:
				if (!instruction.fields[BITS_W]) {
					continue;
				}

			default:
				instruction.fields[field.type] = value;
			}

			if (next) {
				break;
			}

			bit_index += field.size;
		}
	}

done:
	return instruction;
}

void decode_program(program_t program) {
	instruction_t a;
	assert(array_size(a.fields) > BITS_END);

	program.ip = program.instructions;

	// core_print("%i", sizeof("hello world"));
	// char* str = "hello world";
	// core_print("%i", sizeof(str));
	// core_print("%i", sizeof(instruction_field_type_t));
	while (program.ip - program.instructions < program.size) {
		instruction_t result = decode_instruction(program.ip);
		display_instruction(result);
		program.ip += result.size;
	}
}
