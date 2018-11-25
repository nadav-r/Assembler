#ifndef ASSEMBLING_H
#define ASSEMBLING_H
#include "main.h"
#include "translating.h"
#include "printings.h"
#include "data_structures.h"
#include "auxilary.h"
extern int line_num;/*line number in source file*/

/*assemble_src_file(char *src_file_name)*/
/*opens an object file, an entry file and extranl file for writing  and process each Line in that line list*/
/*param char src_file_name[] file name without .as ending*/
void assemble_src_file(char *src_file_name);


/*process_line(line_ptr line, FILE *ob_fp, FILE *ent_fp, FILE *ext_fp)*/
/*process_line: if line is instruction or entry call the corresponding function*/
/*param line_ptr line the line being processed*/
/*param FILE *ob_fp pointer to object file*/
/*param FILE *ent_fp pointer to entry file*/
/*param FILE *ext_fp pointer to external file*/
void process_line(line_ptr line, FILE *ob_fp, FILE *ent_fp, FILE *ext_fp);

/*write_instruction(line_ptr line, FILE * ob_fp)
 translates each line and each relevant parameter using functions from traslating.c and then prints the translations by using functions from printings.c.
it keeps track of IC and it takes care to free memory that is allocated by the translating functions.  */
/*param line_ptr line the line being processed*/
/*param FILE *ob_fp pointer to object file*/
void write_instruction(line_ptr line, FILE * ob_fp);


/*write_entry(line_ptr line, FILE *ent_fp)*/
/*write entry symbols and locations in .ent. */
/*param line_ptr line the line being processed*/
/*param FILE *ent_fp pointer to the entry file*/
void write_entry(line_ptr line, FILE *ent_fp);

/*write_external(char *symbol_name)*/
/*write external in .ext */ 
/*param char symbol_name[] is a symbol with an external flag*/
void write_external(char *symbol_name);
#endif
