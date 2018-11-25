#ifndef PARSING_H
#define PARSING_H
#include "main.h"
#include "data_structures.h"
#include "auxilary.h"
/*************************parsing.c*************************************************/
/*parsing.c handles reading the file and saving valuable data  for assembling in the lines, symbols and data lists and also reporting errors. The parsing module is divided into several functions with different tasks: read lines and skip comment lines, identify a line's type, parsing this specific line type (handle symbols if declared).  
*/

/*tables that are defined in auxilary.c*/
extern char *legal_addresing_modes_src[];
extern char *legal_addresing_modes_dest[];

/*parse_src_file(char *)*/
/*parse_src_file(char *) opens the source file, reads each line and if it is not a comment line or an empty line it passes it as a parameter for parse_line(char *line).*/
/*parm: char src_file_name[] the file's name without the ".as" ending */
void parse_src_file(char *src_file_name);

/*parse_line(char *) identifies the line type (using auxilary functions) and calls the relevant function to continue parsing.
It creates a copy of original line for facilating the other functions in tokenizing the line*/
/*parm char line[] read by parse_src_file*/
void parse_line(char *line);/*identifies the type of the line and calling the right parsing function*/

/*each of the following five functions are called from parse_line. they check if the line starts with a symbol or not and call the right parsing function, passing them the line and the line copy */
void parse_instruction_line(char *line,char *line_copy,int opcode);
void parse_data_line(char *line,char *line_copy);
void parse_string_line(char *line,char *line_copy);
void parse_extern_line(char *line,char *line_copy);
void parse_entry_line(char *line,char *line_copy);


/*each of the folowing three pairs of function parses a specific type of line.
the parsing function that deals with the symbol pass shortened strings that do not contain the symbol to the regular parsing function that do not deal with symbols themselves*/

void parse_instruction_line_no_symbol(char *line, char *line_copy, int opcode,Boolean is_symbol_first);
void parse_instruction_line_symbol(char *line,char *line_copy,int opcode);

void parse_data_line_no_symbol(char *line,char *line_copy);
void parse_data_line_symbol(char *line,char *line_copy);

void parse_string_line_no_symbol(char *line,char *line_copy);
void parse_string_line_symbol(char *line,char *line_copy);

#endif
