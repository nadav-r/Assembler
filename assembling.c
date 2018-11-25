#include "assembling.h"
#define error() fprintf(stderr,"In file '%s' line %d: \n\t ",cur_file, line_num);

extern char cur_file[];/*defined in parsing.c, current file name being processed*/
FILE *ext_fp;/*file pointer to external file it is global because the write_extern is called from a nested function in translating.c*/

/*assemble_src_file(char *src_file_name)*/
/*opens an object file, an entry file and extranl file for writing  and process each Line in that line list*/
/*param char src_file_name[] file name without .as ending*/
void assemble_src_file(char *src_file_name)
{
	FILE *ob_fp, *ent_fp;
	char ob_file_name[MAX_FILE_NAME+5];/*I'm adding to the file names another 5 chars for the suffix*/
	char ent_file_name[MAX_FILE_NAME+5];
 	char ext_file_name[MAX_FILE_NAME+5];
	line_ptr line=line_list_h;
	data_ptr data=data_list_h;
	Boolean ent_fp_closed=FALSE;
	Boolean ext_fp_closed=FALSE;

	strcpy(ob_file_name,src_file_name);
	strcat(ob_file_name,".ob");

	strcpy(ent_file_name,src_file_name);
	strcat(ent_file_name,".ent");

	strcpy(ext_file_name,src_file_name);
	strcat(ext_file_name,".ext");
	
	ob_fp = fopen(ob_file_name, "w");
	ent_fp = fopen(ent_file_name, "w");
	ext_fp = fopen(ext_file_name, "w");

	if(!ob_fp)
	{
		fprintf(stderr, "Error: Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	if(!ent_fp)
	{
		fprintf(stderr, "Error: Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	if(!ext_fp)
	{
		fprintf(stderr, "Error: Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	fprintf(ob_fp, "\t%d %d\n",IC,DC);/*first line in the .ob file specifies how many instruction lines and how many data line are in the file...the ic and dc are currently updated from the routine in parsing.*/
	update_symbol_locaction(&symbol_list_h);/*updating locations for symbol according to IC*/
	line_num=1;/*is external variable, keeping track of line numbers in src file*/
	IC=0;/*update IC.*/
	while(line!=NULL)/*first process all lines*/
	{
		process_line(line, ob_fp,ent_fp,ext_fp);
		line=line->next;
		if(line)
			line_num=line->line_num_in_src;/*keep track of original line numbers for error messages */
	}	
	while(data)/*after finished with lines, write all the data in ob_fp*/
	{
		print_data(data,ob_fp);
		data=data->next;
	}

	/*check if .ext or .ent are empty. if they are delete them. (i do not delete .ob if it is empty)*/
	fseek(ent_fp,0,SEEK_END);
	if(ftell(ent_fp)==0)
	{
		fclose(ent_fp);
		remove(ent_file_name);
		ent_fp_closed=TRUE;
	}
	fseek(ext_fp,0,SEEK_END);
	if(ftell(ext_fp)==0)
	{
		fclose(ext_fp);
		remove(ext_file_name);
		ext_fp_closed=TRUE;
	}

	fclose(ob_fp);
	if(ent_fp_closed==FALSE)
		fclose(ent_fp);
	if(ext_fp_closed==FALSE)
		fclose(ext_fp);
	if(can_be_assembled==FALSE)
		remove(ob_file_name);
}


/*write_entry(line_ptr line, FILE *ent_fp)*/
/*write entry symbols and locations in .ent. */
/*param line_ptr line the line being processed*/
/*param FILE *ent_fp pointer to the entry file*/
void write_entry(line_ptr line, FILE *ent_fp)
{
	symbol_ptr symbol = get_symbol( symbol_list_h,line->operand1);/*in entry line operand1 is their symbol*/
	if(!symbol)
	{
		error() 
		fprintf(stderr,"Error: Symbol '%s' was not declared\n",line->operand1);
		can_be_assembled=FALSE;
		return;
	}
	fprintf(ent_fp, "%s\t%d\n",symbol->name,symbol->location);
	
}

/*write_external(char *symbol_name)*/
/*write external in .ent.  symbol_name[] is a symbol with an external flag so we write it's name and the currernt ic+starting address (this are where an external symbol is used and should be taken care of by the linker)*/
/*write_external(char *symbol_name)*/
/*write external in .ext */ 
/*param char symbol_name[] is a symbol with an external flag*/
void write_external(char *symbol_name)
{
	symbol_ptr symbol = get_symbol( symbol_list_h,symbol_name);
	if(!symbol)
	{
		error() 
		fprintf(stderr,"Error: Symbol '%s' was not declared\n",symbol_name);
		can_be_assembled=FALSE;
		return;
	}
	fprintf(ext_fp, "%s\t%d\n",symbol->name,IC+STARTING_ADDRESS);
}


/*process_line(line_ptr line, FILE *ob_fp, FILE *ent_fp, FILE *ext_fp)*/
/*process_line: if line is instruction or entry call the corresponding function*/
/*param line_ptr line the line being processed*/
/*param FILE *ob_fp pointer to object file*/
/*param FILE *ent_fp pointer to entry file*/
/*param FILE *ext_fp pointer to external file*/
void process_line(line_ptr line, FILE *ob_fp, FILE *ent_fp, FILE *ext_fp)
{	
	switch (line->type)
	{	
		case INSTRUCTION:
			write_instruction(line, ob_fp);
			break;
		case DATA:
		case STRING:
			break;
		case ENTRY:
			write_entry(line, ent_fp);
			break;
		case EXTERN:
			break;
	}
}
/*write_instruction(line_ptr line, FILE * ob_fp)
 translates each line and each relevant parameter using functions from traslating.c and then prints the translations by using functions from printings.c.
it keeps track of IC and it takes care to free memory that is allocated by the translating functions.  */
/*param line_ptr line the line being processed*/
/*param FILE *ob_fp pointer to object file*/
void write_instruction(line_ptr line, FILE *ob_fp)
{

	/*check num of parameters required*/
	if((opcodes_arr[line->opcode]).req_parametes==0)
	{
		Principal_Instruction_Machine_Code * translation = translate_instruction(line);/*translate returns allocates space and return a pointer*/
		if(translation)
		{
			print_principal_instruction(translation,ob_fp);/*print the translation in the ob.file*/
			IC++;/*update ic*/
			free(translation);/*free memory allocated by translate_instruction...*/
		}
		return;/*nothing else to translate, no parameters*/
	}

	if((opcodes_arr[line->opcode]).req_parametes==1)
	{	
		Boolean jmp_syntax=line->jump_syntax;/*flag for jump syntax*/
		Principal_Instruction_Machine_Code * translation = translate_instruction(line);/*translate principal instruction*/
		if(translation)
		{
			print_principal_instruction(translation,ob_fp);/*print it in file*/
			IC++;
			free(translation);/*free space allocated by the translator*/
			if(jmp_syntax==FALSE)
			{
				if(is_register(line->operand2)==TRUE)
				{
					int reg_num = (line->operand2[1])-'0';/*if it is a register like r2 we need the to take '2'and subtract '0'*/
					Register_Param_Machine_Code * register_translation = translate_register_param(0,reg_num);
					if(register_translation)
					{
						print_register_param(register_translation,ob_fp);
						IC++;
						free(register_translation);
						return;
					}
				}
				else /*it is a regular parameter*/
				{
					Regular_Param_Machine_Code * regular_translation = translate_regular_param(line->operand2);
					if(regular_translation)
					{
						print_regular_translation(regular_translation,ob_fp);
						IC++;
						free(regular_translation);
						return;						
					}
				}
			}
			else/*jump_syntax==TRUE*/
			{
				int reg_num_jmp1=-1;/*just to have an initial value*/
				int reg_num_jmp2=-1;	
				if(is_register(line->operand2)==TRUE)
				{	
					int reg_num = (line->operand2[1])-'0';/*if it is a register like r2 we need the to take '2'and subtract '0'*/
					
					Register_Param_Machine_Code * register_translation = translate_register_param(0,reg_num);
					if(register_translation)
					{
						print_register_param(register_translation,ob_fp);
						IC++;
						free(register_translation);
					}
				}
				else
				{
					Regular_Param_Machine_Code * regular_translation;
					regular_translation=translate_regular_param(line->operand2);
					if(regular_translation)
					{
						print_regular_translation(regular_translation,ob_fp);
						IC++;
						free(regular_translation);
					}
				}
				if((is_register(line->jump_op1)==TRUE) && (is_register(line->jump_op2) ==TRUE))
				{
					Register_Param_Machine_Code * register_translation;
					reg_num_jmp1=(line->jump_op1[1])-'0';
					reg_num_jmp2=(line->jump_op2[1])-'0';
					register_translation = translate_register_param(reg_num_jmp1,reg_num_jmp2);
					if(register_translation)
					{
						print_register_param(register_translation,ob_fp);
						IC++;
						free(register_translation);
						return;
					}
				}/*end of routine for 2 jmp register parameters*/
				if((is_register(line->jump_op1)==TRUE) && (is_register(line->jump_op2) ==FALSE))
				{
					Regular_Param_Machine_Code * regular_translation;
					Register_Param_Machine_Code * register_translation;
					reg_num_jmp1=(line->jump_op1[1])-'0';
					register_translation = translate_register_param(reg_num_jmp1,0);
					if(register_translation)
					{		
						print_register_param(register_translation,ob_fp);
						IC++;
						free(register_translation);
					}					
					regular_translation=translate_regular_param(line->jump_op2);
					if(regular_translation)
					{
						print_regular_translation(regular_translation,ob_fp);
						IC++;
						free(regular_translation);
						return;
					}
				
				}/*end of routine for first jmp parm is reg and second not*/
				if((is_register(line->jump_op1)==FALSE) && (is_register(line->jump_op2) ==TRUE))
				{
					Regular_Param_Machine_Code * regular_translation;
					Register_Param_Machine_Code * register_translation;					
					regular_translation=translate_regular_param(line->jump_op1);
					if(regular_translation)
					{
						print_regular_translation(regular_translation,ob_fp);
						IC++;
						free(regular_translation);
					}
					reg_num_jmp2=(line->jump_op2[1])-'0';			
					register_translation = translate_register_param(0,reg_num_jmp2);
					if(register_translation)
					{		
						print_register_param(register_translation,ob_fp);
						IC++;
						free(register_translation);
					}					
				}/*end of routine for first jmp parm is NOT register and the second is*/
				if((is_register(line->jump_op1)==FALSE) && (is_register(line->jump_op2) ==FALSE))
				{
					Regular_Param_Machine_Code * regular_translation1;
					Regular_Param_Machine_Code * regular_translation2;					
					regular_translation1=translate_regular_param(line->jump_op1);
					if(regular_translation1)
					{
						print_regular_translation(regular_translation1,ob_fp);
						IC++;
						free(regular_translation1);
					}
					regular_translation2=translate_regular_param(line->jump_op2);
					if(regular_translation2)
					{		
						print_regular_translation(regular_translation2,ob_fp);
						IC++;
						free(regular_translation2);
					}					
				}/*end of routine for first jmp parm is NOT register and the second is NOT register*/
			}/*end of jump_syntax==TRUE*/	
		}/*end of if(translation)*/
	}/*end of req_parameters==1*/
	
	if((opcodes_arr[line->opcode]).req_parametes==2)
	{ 
		int reg_num1;
		int reg_num2;
		Principal_Instruction_Machine_Code * translation = translate_instruction(line);
		if(translation)
		{
			print_principal_instruction(translation,ob_fp);
			IC++;
			free(translation);
		}		
		
		if((is_register(line->operand1)==TRUE) && (is_register(line->operand2)==TRUE))
		{

			Register_Param_Machine_Code * register_translation;
			reg_num1=(line->operand1[1])-'0';
			reg_num2=(line->operand2[1])-'0';
			register_translation = translate_register_param(reg_num1,reg_num2);		
			if(register_translation)
			{
				print_register_param(register_translation,ob_fp);
				IC++;
				free(register_translation);
				return;
			}
		}
		if((is_register(line->operand1)==TRUE) && (is_register(line->operand2)==FALSE))
		{
			Regular_Param_Machine_Code * regular_translation;
			Register_Param_Machine_Code * register_translation;
			reg_num1=(line->operand1[1])-'0';
			register_translation = translate_register_param(reg_num1,0);
			if(register_translation)
			{		
				print_register_param(register_translation,ob_fp);
				IC++;
				free(register_translation);
			}					
			regular_translation=translate_regular_param(line->operand2);
			if(regular_translation)
			{
				print_regular_translation(regular_translation,ob_fp);
				IC++;
				free(regular_translation);
				return;
			}
		}
		if((is_register(line->operand1)==FALSE) && (is_register(line->operand2)==TRUE))
		{
			Regular_Param_Machine_Code * regular_translation;
			Register_Param_Machine_Code * register_translation;					
			regular_translation=translate_regular_param(line->operand1);
			if(regular_translation)
			{
				print_regular_translation(regular_translation,ob_fp);
				IC++;
				free(regular_translation);
			}
			reg_num2=(line->operand2[1])-'0';
			register_translation = translate_register_param(reg_num2,0);
			if(register_translation)
			{		
				print_register_param(register_translation,ob_fp);
				IC++;
				free(register_translation);
			}
		}
		if((is_register(line->operand1)==FALSE) && (is_register(line->operand2)==FALSE))
		{
			Regular_Param_Machine_Code * regular_translation1;
			Regular_Param_Machine_Code * regular_translation2;					
			regular_translation1=translate_regular_param(line->operand1);
			if(regular_translation1)
			{
				print_regular_translation(regular_translation1,ob_fp);
				IC++;
				free(regular_translation1);
			}
			regular_translation2=translate_regular_param(line->operand2);
			if(regular_translation2)
			{		
				print_regular_translation(regular_translation2,ob_fp);
				IC++;
				free(regular_translation2);
			}
		}
	}		
}


