#ifndef AUX_H
#define AUX_H
#include "main.h"

/*check addresing mode*/
/*check legality of an addresing mode the user should spcify which legal addresing mode array he wishes to use, the opcode (its index in the opcodes array) and the addresing mode*/
/* return TRUE if legal and FALSE if not.*/
/*param * legal_addresing_modes_array[] array of legal addresing modes*/
/*param int opcode*/
/*param ADDRESING_MODE mode*/
Boolean is_legal_addressing_mode(char * legal_addresing_modes_array[], int opcode,ADDRESING_MODE mode);


/*is_register(char *str)*/
/*check if *str is aregister*/
/* return TRUE if it is and FALSE if not.*/
/*param *str parameter to be checked*/
Boolean is_register(char *str);

/*is_instruction_line(char *line)*/
/*check if *line is an instruction line, i.e. includes an opcode. */
/*return : opcode index (=decimal representation of its binary rep.)in opcodes_arr, or -1 if it is not an opcode*/
/*param *line in src file*/
int is_instruction_line(char *line);


/*is_string/data/entry.extern_line(char *line)*/
/*checks if string/data/entry/extern line*/
/* return TRUE if it is and FALSE if not.*/
/*param *line in src file*/
Boolean is_string_line(char *line);
Boolean is_data_line(char *line);
Boolean is_entry_line(char *line);
Boolean is_extern_line(char *line);

/*n_machine_words(line_ptr instruction)*/
/*calculates how many machine words needed for coding the instruction line, returns how many lines neede. it is used for updating the IC while parsing*/
/*param: line_ptr instruction - pointer to Line of type instruction in the list*/
/*return: int how many machine words needed*/
int n_machine_words(line_ptr instruction);

#endif
