#ifndef MAIN_H
#define MAIN_H
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define STARTING_ADDRESS 100 /*the object file starts at memory location number 100*/

#define MAX_SYMBOL_NAME 32
#define MAX_OPCODES 16
#define MAX_LINE 80
#define MAX_FILE_NAME 80 
#define NUM_OF_REGULAR_REGISTERS 8

/*for the bit representations of a machine word*/

#define BITS_PER_WORD 14
#define PRINCIPAL_BIT_LOC 14
#define ARE_LOC 0
#define DEST_ADDRESING_MODE_LOC 2
#define SOURCE_ADDRESING_MODE_LOC 4
#define OPCODE_LOC 6
#define PARAM1_LOC 12
#define PARAM2_LOC 10

#define SRC_REG_LOC 8
#define DEST_REG_LOC 2
#define REGULAR_PARAM_LOC 2


/*****************************DATA STRUCTURES*****************************************************/

typedef enum {FALSE, TRUE} Boolean;


/****************************************************************************************/
/*Line: Represents lines (not including comment lines and empty lines) in the source file.
The fields are filled in by the parsing routines.*/
/*****************************************************************************************/

typedef enum{INSTRUCTION, DATA,STRING,ENTRY,EXTERN}Line_Type;

typedef struct line_item *line_ptr;
typedef struct line_item
{
	Line_Type type;
	Boolean symbol_first;
	Boolean jump_syntax;	
	int opcode;
	char operand1[MAX_SYMBOL_NAME];
	char operand2[MAX_SYMBOL_NAME];
	char jump_op1[MAX_SYMBOL_NAME];
	char jump_op2[MAX_SYMBOL_NAME];
	int line_num_in_src;
	line_ptr next;
}Line;

/****************************************************************************************/
/*Opcode: For a table of opcodes, which is declared in data_structures.c
The fields are filled by the parsing routines.*/
/*****************************************************************************************/

#define MAX_OPCODE_NAME 4
#define BITS_FOR_OPCODE 4

typedef struct
{
	char name[MAX_OPCODE_NAME+1];
	int req_parametes;
	unsigned int binary_rep: BITS_FOR_OPCODE;
}Opcode;


typedef enum{IMMEDIATE,DIRECT,JUMP,REG_DIRECT} ADDRESING_MODE;




/******************************DS's FOR MACHINE_CODES****************************************/
/*There are 3 types of machine codes (1) Principal_Instruction_Machine_Code (2) Register_Param_Machine_Code (3)Regular_Param_Machine_Code*/
/****************************************************************************************/

/*defininitions for machine codes - how many bits does representing each of the following pieces of information requires*/
#define BITS_FOR_ARE 2
#define	BITS_FOR_DEST_ADDRESING_MODE 2
#define	BITS_FOR_SOURCE_ADDRESING_MODE 2
#define	BITS_FOR_FIRST_PARAM 2
#define	BITS_FOR_SECOND_PARAM 2
#define	BITS_FOR_REGISTER 6
#define	BITS_FOR_REGULAR_PARAM 12

/****************************************************************************************/
/*(1)Principal_Instruction_Machine_Code: Represeting the machine code of a principal instruction line, i.e, a line which includes an opcode*/
/*****************************************************************************************/

typedef struct
{
	unsigned int are:BITS_FOR_ARE;
	unsigned int addressing_mode_dest:BITS_FOR_DEST_ADDRESING_MODE;
	unsigned int addressing_mode_src:BITS_FOR_SOURCE_ADDRESING_MODE;
	unsigned int opcode:BITS_FOR_OPCODE;
	unsigned int parm1:BITS_FOR_FIRST_PARAM;
	unsigned int parm2:	BITS_FOR_SECOND_PARAM;
}Principal_Instruction_Machine_Code;


/****************************************************************************************/
/*(2)Register_Param_Machine_Code: Represeting the machine code that specifies a register parameter(s)*/
/*****************************************************************************************/

typedef struct
{
	unsigned int are:BITS_FOR_ARE;
	unsigned int src:BITS_FOR_REGISTER;
	unsigned int dest: BITS_FOR_REGISTER;
}Register_Param_Machine_Code;
/****************************************************************************************/
/*(3)Register_Param_Machine_Code: Represeting the machine code that specifies either an immediate parameter or a label address*/
/*****************************************************************************************/
typedef struct
{
	unsigned int are:BITS_FOR_ARE;
	unsigned int param:BITS_FOR_REGULAR_PARAM;
}Regular_Param_Machine_Code;



/***************************DS FOR DATA LIST*******************************************************/
/*Data_Code represents the machine code for data (e.g. for the line '.string "abc"' A Data_Code is created for each char (and another for '\0'). ). it also encodes lines such as ".data 1,2,3", creating a Data_code for each integer*/
typedef struct data_code_item * data_ptr;

typedef struct data_code_item
{
	Line_Type type;
	int data; 
	data_ptr next;
	
}Data_Code;



/***************************DS FOR SYMBOL TABLE *******************************************************/


typedef enum {ABSOLUTE,EXTERNAL,RELOCATABLE} ARE_TYPE;

typedef struct symbol_item * symbol_ptr;

typedef struct symbol_item
{
	char name[MAX_SYMBOL_NAME];
	int location;
	Boolean in_instruction_line;
	Boolean external;
	symbol_ptr next; 

}Symbol;


/*external variables for global use*/
extern Opcode opcodes_arr[MAX_OPCODES];
extern int IC;
extern int DC;
extern Boolean can_be_assembled;
extern symbol_ptr symbol_list_h;
extern symbol_ptr symbol_list_t;
extern line_ptr line_list_h;
extern line_ptr line_list_t;
extern data_ptr data_list_h;
extern data_ptr data_list_t;
#endif
