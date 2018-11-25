#include "translating.h"
#define print(a) printf(#a "\n");f/*for debugging*/

#define error() fprintf(stderr,"In file '%s' line %d: \n\t ",cur_file, line_num);

/*translate_instruction(line_ptr instruction)*/
/*return: a pointer to machine code translation of the principal instruction*/
/*param: line_ptr, pointer to line of type instruction (i.e. with opcode) in the line list*/
Principal_Instruction_Machine_Code * translate_instruction(line_ptr instruction)
{
	/*allocate space*/
	Principal_Instruction_Machine_Code * translation = (Principal_Instruction_Machine_Code *) malloc(sizeof(Principal_Instruction_Machine_Code));
	
	if(!translation)
	{
		fprintf(stderr, "Error: Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	/*fill in the fields of the translation according to the parameters*/
	translation->are = ABSOLUTE;
	translation->opcode = instruction->opcode;
	if((opcodes_arr[instruction->opcode]).req_parametes==0)
	{/*no parameters so set all to immediate*/
		translation->addressing_mode_dest=IMMEDIATE;
		translation->addressing_mode_src=IMMEDIATE;
		translation->parm1=IMMEDIATE;
		translation->parm2=IMMEDIATE;
	}
	if((opcodes_arr[instruction->opcode]).req_parametes==1)
	{
		if(instruction->jump_syntax==FALSE)
		{			
			translation->addressing_mode_src=IMMEDIATE;
			
			translation->parm1=IMMEDIATE;
			translation->parm2=IMMEDIATE;		
			if(is_register(instruction->operand2)==TRUE)
			{
				translation->addressing_mode_dest=REG_DIRECT;
			}		
			else if((instruction->operand2)[0]=='#')
			{
				translation->addressing_mode_dest=IMMEDIATE;
			}
			
			else translation->addressing_mode_dest=DIRECT;	

		}
		else/*jump_syntax==TRUE*/
		{
			char temp[MAX_SYMBOL_NAME];			
			translation->addressing_mode_src=IMMEDIATE;
			if(is_register(instruction->operand2)==TRUE)
			{
				translation->addressing_mode_dest=REG_DIRECT;
			}		
			else 
			{	
				if( (strcmp((opcodes_arr[instruction->opcode]).name,"jmp")==0) ||(strcmp((opcodes_arr[instruction->opcode]).name,"bne")==0) || (strcmp((opcodes_arr[instruction->opcode]).name,"jsr")==0))
				{
					translation->addressing_mode_dest=JUMP;
				}
				else
				{
					translation->addressing_mode_dest=DIRECT;	
				}
			}
			strcpy(temp,instruction->jump_op1);
			if(is_register(temp)==TRUE)
			{
				translation->parm1=REG_DIRECT;	
			}
			else if(temp[0]=='#')
			{
				translation->parm1=IMMEDIATE;
			}
			else
			{
					translation->parm1=DIRECT;
			}
			strcpy(temp,instruction->jump_op2);
			if(is_register(temp)==TRUE)
			{
				translation->parm2=REG_DIRECT;	
			}
			else if(temp[0]=='#')
			{
				translation->parm2=IMMEDIATE;
			}
			else
			{
				translation->parm2=DIRECT;
			}
			
		}	
	}
	if((opcodes_arr[instruction->opcode]).req_parametes==2)
	{
		char temp[MAX_SYMBOL_NAME];
		translation->parm1=IMMEDIATE;
		translation->parm2=IMMEDIATE;
		strcpy(temp,instruction->operand1);
		if(is_register(temp)==TRUE)
		{
			translation->addressing_mode_src=REG_DIRECT;	
		}
		else if(temp[0]=='#')
		{
			translation->addressing_mode_src=IMMEDIATE;
		}
		else
		{
			translation->addressing_mode_src=DIRECT;		
		}
		strcpy(temp,instruction->operand2);
		if(is_register(temp)==TRUE)
		{
			translation->addressing_mode_dest=REG_DIRECT;	
		}
		else
		{
			translation->addressing_mode_dest=DIRECT;		
		}				
	}	
	return translation;
}


/*translate_register_param(int n_first_reg, int n_second_reg)*/
/*return a pointer to machine code translation of the regular instruction*/
/*param: int n_first_reg the number of the first register - 0 if none*/
/*param: int n_second_reg the number of the second register - 0 if none*/
Register_Param_Machine_Code * translate_register_param(int n_first_reg, int n_second_reg)
{
	Register_Param_Machine_Code * translation =(Register_Param_Machine_Code *) malloc(sizeof(Register_Param_Machine_Code));
	if(!translation)
	{
		fprintf(stderr, "Error: Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	
	translation->are=ABSOLUTE;
	translation->src = n_first_reg;
	translation->dest = n_second_reg;

	return translation;
	
}


/*translate_regular_param(char * parm)*/
/*return a pointer to machine code translation of the regular instruction*/
/*param: char parm[] a regular parameter , either a symbol name or an immediate parameter*/
Regular_Param_Machine_Code * translate_regular_param(char * parm)
{
	Boolean is_immediate = (parm[0]=='#')? TRUE : FALSE;
	Boolean sign = ((is_immediate == TRUE) && (!isdigit(parm[1])))? TRUE : FALSE;
	Boolean is_negative = ((sign == TRUE) && (parm[1]=='-'))? TRUE : FALSE;
	Regular_Param_Machine_Code * translation = (Regular_Param_Machine_Code *) malloc (sizeof(Regular_Param_Machine_Code ));
	if(!translation)
	{
		fprintf(stderr, "Error: Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	
	if(is_immediate == FALSE)/*the it should be a symbol in the symbol table*/
	{
		int loc;
		symbol_ptr symbol = get_symbol(symbol_list_h, parm);
		if(symbol==NULL)
		{
			error()
			fprintf(stderr, "Error: Symbol %s was not declared\n",parm);
			can_be_assembled=FALSE;
			free(translation);
			translation=NULL;
			return NULL;
		}
		translation->are = ((symbol->external)==TRUE)? EXTERNAL : RELOCATABLE;
		if((symbol->external)==TRUE)
		{
			write_external(parm);
		}
		loc = symbol->location;
		translation->param =loc;/*assuming it fits*/
		return translation;
	}
	/*if it is immediate*/
	translation->are=ABSOLUTE;
	if(sign==FALSE)
	{
		translation->param = atoi(parm+1);/*atoi the string after the '#'*/
		return translation;
	}
	if(sign==TRUE)
	{
		if(is_negative==FALSE)
		{
			translation->param = atoi(parm+2);/*atoi the string after'#+'*/
			return translation;
		}
	}
	/*is negative == true*/
	translation->param = - atoi(parm+2);/*atoi the string after'#-'*/
	return translation;	
}

