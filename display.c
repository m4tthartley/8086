//
//  display.c
//  8086
//
//  Created by Matt Hartley on 27/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <core/core.h>

#include "8086def.h"

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

char* effective_address_strings[] = {
    "bx + si",
    "bx + di",
    "bp + si",
    "bp + di",
    "si",
    "di",
    "bp",
    "bx",
};

#define get_register_name(reg, wide) (register_names[(wide ? 0 : 8) + reg])

// void display_memory_access(instruction_t inst) {
// 	core_print_inline("[%s", register_names[rm_table[inst.fields[BITS_RM]]]);
// 	if (inst.fields[BITS_RM] < 4) {
// 		core_print_inline(" + %s", register_names[rm_plus_table[inst.fields[BITS_RM]]]);
// 	}
// 	if (inst.fields[BITS_MOD] == 0b01) {
// 		core_print_inline(" + %hhi", (u8)inst.fields[BITS_DISP8]);
// 	}
// 	if (inst.fields[BITS_MOD] == 0b10) {
// 		core_print_inline(" + %hi", inst.fields[BITS_DISP16]);
// 	}
// 	core_print_inline("]");
// }

// void print_bin8(u8 bin) {
// 	char result[12] = {0};
// 	// u64 result;
// 	result[0] = '0';
// 	result[1] = 'b';
// 	result[10] = ' ';
// 	FOR (i, 8) {
// 		// ((char*)(&result))[i] = (1<<i) & bin ? '1' : '0';
// 		result[2+i] = (1<<(7-i)) & bin ? '1' : '0';
// 	}

// 	core_print_inline(result);
// }

// void display_reg(instruction_t inst) {
//     core_print_inline(get_register_name(inst.fields[BITS_REG], inst.fields[BITS_W]));
// }

// void display_rm(instruction_t inst) {
//     // core_print_inline(get_register_name(inst.fields[BITS_REG], inst.fields[BITS_W]));

//     if (inst.fields[BITS_MOD] == 0b11) {
//         core_print_inline(get_register_name(inst.fields[BITS_RM], inst.fields[BITS_W]));
//     } else  {
//         if (inst.fields[BITS_MOD] == 0 && inst.fields[BITS_RM] == 0b110) {
//             // direct address

//             if (inst.fields[BITS_D]) {
//                 core_print_inline("[%hu]", inst.fields[BITS_DISP16]);
//             } else {
//                 core_print_inline("[%hu], ", inst.fields[BITS_DISP16]);
//             }
//         } else {
//             display_memory_access(inst);
//         }
//     }
// }

void display_operand(instruction_t inst, operand_t operand) {
    switch (operand.type) {
    case OPERAND_REGISTER:
        core_print_inline(get_register_name(operand.reg, inst.wide));
        break;
    case OPERAND_MEMORY:
        core_print_inline("[");
        core_print_inline(effective_address_strings[operand.reg]);
        if (operand.disp) {
            if (operand.disp < 0) {
                core_print_inline(" - %hi", operand.disp * -1);
            } else {
                core_print_inline(" + %hi", operand.disp);
            }
        }
        core_print_inline("]");
        break;
    case OPERAND_DIRECT_ADDRESS:
        core_print_inline("[%hu]", operand.disp);
        break;
    case OPERAND_IMMEDIATE:
        if (inst.wide) {
            core_print_inline("%hi", operand.data);
        } else {
            core_print_inline("%hhi", operand.data8.low);
        }
        break;
    }
}

void display_instruction(instruction_t inst) {
    // u8 dest = inst.fields[BITS_D];
    // u8 wide = inst.fields[BITS_W];
    // u8 mode = inst.fields[BITS_MOD];
    // u8 reg = inst.fields[BITS_REG];
    // u8 rm = inst.fields[BITS_RM];

    core_print_inline("mov ");
    display_operand(inst, inst.operands[0]);
    core_print_inline(", ");
    display_operand(inst, inst.operands[1]);
    core_print_inline("\n");

    // if (dest) {
    //     display_reg(inst);
    //     core_print_inline(", ");
    //     display_rm(inst);
    //     core_print_inline("\n");
    // } else {
    //     display_rm(inst);
    //     core_print_inline(", ");
    //     display_reg(inst);
    //     core_print_inline("\n");
    // }
}
