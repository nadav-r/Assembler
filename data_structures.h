#ifndef DS_H
#define DS_H
#include "main.h"
extern int line_num;
extern char cur_file[];

/*is_opcode (char *str)*/
/*checks if a string is an op code, if it is, it returns is binary code in decimal, which is also its index int the array. If it is not an opcode it returns -1*/
/*return : opcode index (=decimal representation of its binary rep.), or -1 if it is not an opcode*/
/*param *str string to be checked*/
int is_opcode(char *str);


/*is_protected_name(char *str)*/
/*check if str is protected (cannot be used as symbol)*/
/*return: TRUE if it is, FALSE if not*/
/*param *str name to be checked*/
Boolean is_protected_name(char *str);

/*symbol_is_legal(symbol_ptr head, char *symbol_name)*/
/*check if str is a legal symbol*/
/*return: TRUE if it is, FALSE if not*/
/*param symbol_ptr head of symbols' list*/
/*param char *symbol_name to be checked*/
Boolean symbol_is_legal(symbol_ptr head, char *symbol_name);

/*symbol list operations*/
void add_to_symbol_tab(symbol_ptr * head, symbol_ptr *tail,char *myname, Boolean is_in_instruction,Boolean is_external);
void free_symbol_tab(symbol_ptr *head);
symbol_ptr get_symbol(symbol_ptr head,char *symbol_name);

/*after parsing update the symbol locations*/
void update_symbol_locaction(symbol_ptr *head);

/*line list operations*/
void add_line(line_ptr *head, line_ptr * tail, Line_Type mytype, Boolean is_symbol_first,Boolean is_jump_syntax, int myopcode, char *operand1, char *operand2,char *jmp_op1,char *jmp_op2);
void free_line_list(line_ptr *head);

/*data list operations*/
void add_data(data_ptr *head, data_ptr *tail,Line_Type mytype, int mydata);
void free_data_list(data_ptr *head);



#endif
