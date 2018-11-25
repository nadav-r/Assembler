#ifndef PRINTING_H
#define PRINTING_H
#include "main.h"
/*the first three printing functions print to the .ob file the machine code, using a pointer to translation object created by a translator and print them according to their inner structure using bit fields, the last is given a data ptr and prints it as machine word*/


/*print_principal_instruction(Principal_Instruction_Machine_Code * translation ,FILE *ob_fp)*/
/*printing the bit representation translation object (made by a translator) according to its inner bit fields structure.*/
/*param Principal_Instruction_Machine_Code * translation, pointer to object made by a translator. represents the principal instruction, i.e, a line that includes an opcode */
/*param FILE *ob_fp pointer to the object file */
void print_principal_instruction(Principal_Instruction_Machine_Code * translation ,FILE *ob_fp);

/*print_register_param(Register_Param_Machine_Code * translation,FILE *ob_fp)*/
/*printing the bit representation translation object (made by a translator) according to its inner bit fields structure.*/
/*param Register_Param_Machine_Code * translation, pointer to object made by a translator, represents register parameters */
/*param FILE *ob_fp pointer to the object file */
void print_register_param(Register_Param_Machine_Code * translation,FILE *ob_fp);

/*print_regular_translation(Regular_Param_Machine_Code * translation,FILE *ob_fp)*/
/*printing the bit representation of translation object (made by a translator) according to its inner bit fields structure.*/
/*param Regular_Param_Machine_Code * translation, pointer to object made by a translator, represents regular parameters, i.e., immediate parametes and locations of symbols */
/*param FILE *ob_fp pointer to the object file */
void print_regular_translation(Regular_Param_Machine_Code * translation,FILE *ob_fp);


/*print_data(data_ptr data,FILE *ob_fp)*/
/*printing the bit representation of a data from the data list.*/
/*param data_ptr data, pointer to data in the data list*/
/*param FILE *ob_fp pointer to the object file */
void print_data(data_ptr data,FILE *ob_fp);

#endif
