//
//  display.c
//  8086
//
//  Created by Matt Hartley on 27/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

// #define print_rm() \
// 	core_print_inline("[%s", register_names[rm_table[rm]]); \
// 	if (rm < 4) { \
// 		core_print_inline(" + %s", register_names[rm_plus_table[rm]]); \
// 	} \
// 	if (mode == 0b01) { \
// 		core_print_inline(" + %hhi", disp8); \
// 	} \
// 	if (mode == 0b10) { \
// 		core_print_inline(" + %hi", disp16); \
// 	} \
// 	core_print_inline("]");

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

// void display_instruction(instruction_t inst) {
//     u8 dest = inst.fields[BITS_D];
//     u8 wide = inst.fields[BITS_W];
//     u8 reg = inst.fields[BITS_REG];
//     u8 rm = inst.fields[BITS_RM];

//     core_print_inline("mov ");

//     if (mode == 0b11) {
//         // core_print_inline("mov ");
//         core_print_inline(get_register_name(regdst, wide));
//         core_print_inline(", %s \n", get_register_name(regsrc, wide));
//     } else  {
//         if (mode == 0 && rm == 0b110) {
//             // direct address
//             // i16 disp16 = *(i16*)ip;
//             // ip += 2;

//             if (inst.fields[BITS_D]) {
//                 // core_print_inline("mov ");
//                 core_print_inline(get_register_name(reg, wide));
//                 core_print_inline(", [%hu]", disp16);
//                 core_print_inline("\n");
//             } else {
//                 // core_print_inline("mov ");
//                 core_print_inline("[%hu], ", disp16);
//                 core_print_inline(get_register_name(reg, wide));
//                 core_print_inline("\n");
//             }
//         } else {
//             i8 disp8 = *ip;
//             i16 disp16 = *(i16*)ip;
//             ip += mode;

//             // core_print_inline("mov ");
//             if (dest) {
//                 core_print_inline(get_register_name(reg, wide));
//                 core_print_inline(", ");
//                 print_rm();
//                 core_print_inline("\n");
//             } else {
//                 print_rm();
//                 core_print_inline(", ");
//                 core_print_inline(get_register_name(reg, wide));
//                 core_print_inline("\n");
//             }
//         }
//     }
// }
