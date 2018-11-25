#ifndef TRANSLATING_H
#define TRANSLATING_H
#include "main.h"
#include "auxilary.h"
#include "data_structures.h"
#include "assembling.h"

extern int line_num;/*line number in source file*/
extern char cur_file[];/*current file name being processed*/
extern FILE *ext_fp;/*file pointer to extern file*/

/*the translating functions allocate space for a pointer to  machine code and fill in the bit fields of the machine code according to parameters and return that pointer. the user of these function is responsible for freeing memory*/

/*translate_instruction(line_ptr instruction)*/
/*return: a pointer to machine code translation of the principal instruction*/
/*param: line_ptr, pointer to line of type instruction (i.e. with opcode) in the line list*/
Principal_Instruction_Machine_Code * translate_instruction(line_ptr instruction);

/*translate_regular_param(char * parm)*/
/*return a pointer to machine code translation of the regular instruction*/
/*param: char parm[] a regular parameter , either a symbol name or an immediate parameter*/
Regular_Param_Machine_Code * translate_regular_param(char * parm);

/*translate_register_param(int n_first_reg, int n_second_reg)*/
/*return a pointer to machine code translation of the regular instruction*/
/*param: int n_first_reg the number of the first register - 0 if none*/
/*param: int n_second_reg the number of the second register - 0 if none*/
Register_Param_Machine_Code * translate_register_param(int n_first_reg, int n_second_reg);
#endif
