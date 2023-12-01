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
	u16 fields[FIELD_COUNT];
	u32 field_mask = 0;
	// assert(array_size(fields) > FIELD_COUNT);

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
			case FIELD_NULL:
				// we reached the end
				// return instruction
				// core_print("format %i", format_index);
				assert(bit_index % 8 == 0);
				// instruction.size = round_up(byte_offset, )
				instruction.size = bit_index/8;
				goto done;
				break;

			case FIELD_LITERAL:
				assert(field.size <= (8-bit_offset));
				// for (int i = field.size; i >= 0; --i) {
				//     u8 bit = (0b1 << (8-bit_offset-1))
				// }
				
				if (value != field.value) {
					next = TRUE;
				}
				break;

			case FIELD_DATA_IF_W:
				if (!fields[FIELD_W]) {
					continue;
				}

			default:
				if (!field.size) {
					fields[field.type] = field.value;
				} else {
					fields[field.type] = value;
				}
				field_mask |= (1 << field.type);
			}

			if (next) {
				break;
			}

			bit_index += field.size;
		}
	}

done:
	u32 mod_mask = (1 << FIELD_MOD);

	instruction.wide = fields[FIELD_W];

	operand_t* reg = &instruction.operands[1 - fields[FIELD_D]];
	operand_t* rm = &instruction.operands[fields[FIELD_D]];
	operand_t* im = &instruction.operands[1];
	reg->reg = fields[FIELD_REG];
	rm->reg = fields[FIELD_RM];

	if (field_mask & mod_mask) {
		rm->type = OPERAND_MEMORY;

		if (fields[FIELD_MOD] == 0 && fields[FIELD_RM] == 0b110) {
			// Direct address
			rm->disp = *(u16*)(ip+instruction.size);
			instruction.size += 2;
			rm->type = OPERAND_DIRECT_ADDRESS;
		}

		if (fields[FIELD_MOD] == 0b01) {
			rm->disp = *(ip+instruction.size);
			instruction.size += 1;
			
		}

		if (fields[FIELD_MOD] == 0b10) {
			rm->disp = *(u16*)(ip+instruction.size);
			instruction.size += 2;
		}

		if (fields[FIELD_MOD] == 0b11) {
			rm->type = OPERAND_REGISTER;
		}
	}

	if (fields[FIELD_DATA]) {
		// Immediate
		im->data = ip[instruction.size];
		++instruction.size;
		if(fields[FIELD_DATA_IF_W]) {
			im->data |= ((u16)ip[instruction.size] << 8);
			++instruction.size;
		}
		im->type = OPERAND_IMMEDIATE;
	}

	return instruction;
}

void decode_program(program_t program) {
	// instruction_t a;

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
