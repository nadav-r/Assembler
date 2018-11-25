#include "auxilary.h"

/*legal addresing modes arrays that correspond to the opcodes_arr. for example, a string like "013" in index 1 indicates that addressing modes 0,1,3, are legal for the opcode that is in index 1 in the opcodes_arr */

char *legal_addresing_modes_src[] = 
{"013", "013", "013", "013","-","-","1","-","-","-","-","-","-","-","-","-"};

char *legal_addresing_modes_dest[] = 
{"13", "013", "13", "13","13","13","13","13","13","123","123","13","013","123","-","-"};

/*check addresing mode*/
/*check legality of an addresing mode the user should spcify which legal addresing mode array he wishes to use, the opcode (its index in the opcodes array) and the addresing mode*/
/* return TRUE if legal and FALSE if not.*/
/*param * legal_addresing_modes_array[] array of legal addresing modes*/
/*param int opcode*/
/*param ADDRESING_MODE mode*/
Boolean is_legal_addressing_mode(char * legal_addresing_modes_array[], int opcode,ADDRESING_MODE mode)
{
	char addresing_mode = mode +'0';/*convert addresing mode to its corresponding char*/
	if(strchr(legal_addresing_modes_array[opcode],addresing_mode))
		return TRUE;
	return FALSE;
}

/*array of registers*/
char *registers_general[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
                 
/*is_register(char *str)*/
/*check if *str is aregister*/
/* return TRUE if it is and FALSE if not.*/
/*param *str parameter to be checked*/                  
Boolean is_register(char *str)
{
	int i;
	for(i=0;i<NUM_OF_REGULAR_REGISTERS;i++)
	{
		if(strcmp(registers_general[i],str)==0)
			return TRUE;
	}
	return FALSE;
}


/*is_instruction_line(char *line)*/
/*check if *line is an instruction line, i.e. includes an opcode. */
/*return : opcode index (=decimal representation of its binary rep.)in opcodes_arr, or -1 if it is not an opcode*/
/*param *line in src file*/
int is_instruction_line(char *line)
{
	int i;
	char *p;
	for(i=0;i<MAX_OPCODES;i++)
	{
		if((p=strstr(line,(opcodes_arr[i]).name)))
		{	
			int substring_i=p-line;
			int next_c_i = substring_i + strlen(opcodes_arr[i].name);
			if(substring_i>0)
			{
				char prev_c = line[substring_i-1];
				
				if(!isspace(prev_c) && prev_c!=':')
					continue;
			}
						
			if(next_c_i<=strlen(line))
			{
				char next_c=line[next_c_i];
				if(!isspace(next_c) && next_c!='\0')
					continue;
			}
			return i;
		}
	}
	return -1;
}


/*is_string/data/entry.extern_line(char *line)*/
/*checks if string/data/entry/extern line*/
/* return TRUE if it is and FALSE if not.*/
/*param *line in src file*/
Boolean is_string_line(char *line)
{
	char *p;
	int len=7;/*len of".string"*/
	if((p=strstr(line,".string")))
	{
		int substring_i=p-line;
		int next_c_i = substring_i + len;
		if(substring_i>0)
		{
			char prev_c = line[substring_i-1];
			
			if(!isspace(prev_c)&&prev_c!=':')
				return FALSE;
		}
		if(next_c_i<=strlen(line))
		{	
			char next_c=line[next_c_i];
			if(!isspace(next_c) && next_c!='\0')
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

Boolean is_data_line(char *line)
{
	char *p;
	int len=5;/*len of".data"*/
	if((p=strstr(line,".data")))
	{
		int substring_i=p-line;
		int next_c_i = substring_i + len;
		if(substring_i>0)
		{
			char prev_c = line[substring_i-1];
			
			if(!isspace(prev_c)&&prev_c!=':')
				return FALSE;
		}
		if(next_c_i<=strlen(line))
		{	
			char next_c=line[next_c_i];
			if(!isspace(next_c) && next_c!='\0')
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

Boolean is_entry_line(char *line)
{
	char *p;
	int len=6;/*len of".entry"*/
	if((p=strstr(line,".entry")))
	{
		int substring_i=p-line;
		int next_c_i = substring_i + len;
		if(substring_i>0)
		{
			char prev_c = line[substring_i-1];
			
			if(!isspace(prev_c)&&prev_c!=':')
				return FALSE;
		}
		if(next_c_i<=strlen(line))
		{	
			char next_c=line[next_c_i];
			if(!isspace(next_c) && next_c!='\0')
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

Boolean is_extern_line(char *line)
{
	char *p;
	int len=7;/*len of".extern"*/
	if((p=strstr(line,".extern")))
	{
		int substring_i=p-line;
		int next_c_i = substring_i + len;
		if(substring_i>0)
		{
			char prev_c = line[substring_i-1];
			
			if(!isspace(prev_c)&&prev_c!=':')
				return FALSE;
		}
		if(next_c_i<=strlen(line))
		{	
			char next_c=line[next_c_i];
			if(!isspace(next_c) && next_c!='\0')
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}


/*n_machine_words(line_ptr instruction)*/
/*calculates how many machine words needed for coding the instruction line, returns how many lines neede. it is used for updating the IC while parsing*/
/*param: line_ptr instruction - pointer to Line of type instruction in the list*/
/*return: int how many machine words needed*/
int n_machine_words(line_ptr instruction)
{
	int L = 1; /*at least one machine word needed*/
	int n_parameters = (opcodes_arr[(instruction -> opcode)]).req_parametes;  
	Boolean jump_syntax = instruction->jump_syntax;
	switch (n_parameters)
	{
		case 0:
			break;
		case 1: 
			L++;
			if(jump_syntax==TRUE)
			{
				if((is_register(instruction->jump_op1)==TRUE) && (is_register(instruction->jump_op2)==TRUE))
				{
					L++;
				}
				else
				{
					L+=2;
				}
			}
			break;
		case 2:
			if((is_register(instruction->operand1)==TRUE) && (is_register(instruction->operand2)==TRUE))
			{
				L++;
			}
			else 
			{
				L+=2;
			}
			break;
	}
	return L;
}



