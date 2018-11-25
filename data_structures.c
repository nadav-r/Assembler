#include "data_structures.h"
#define error() fprintf(stderr,"In file '%s' line %d: \n\t ",cur_file, line_num);

#define NUMBER_OF_PROTECTED_NAMES 26
/*protected_names: array of illegal names for symbols*/
char *protected_names[]={"mov","cmp,","add","sub","not","clr","lea","inc","dec","jmp","bne","red",
"prn","jsr","rts","stop","#",":","r0","r1","r2","r3","r4","r5","r6","r7"};

/*is_protected_name(char *str)*/
/*check if str is protected (cannot be used as symbol)*/
/*return: TRUE if it is, FALSE if not*/
/*param *str name to be checked*/
Boolean is_protected_name(char *str)
{
	int i;
	for(i=0;i<NUMBER_OF_PROTECTED_NAMES;i++)
	{
		if(strcmp(protected_names[i],str)==0)
			return TRUE;
	}
	return FALSE;
}

/*-------------------------------------------------------------------------------------------------*/
/*DATA STRUCTURES AND RELATED FUNCTION*/
/*-------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------*/
/* OPCODES TABLE, REGISTERS, ADDRESING MODES AND RELATED TASKS*/
/*-------------------------------------------------------------------------------------------------*/

/*declare and define the opcodes' table: columns: name, required parameters, machine code in decimal*/
Opcode opcodes_arr[MAX_OPCODES] = {/*"name",req_params,binary code in decimal*/
								   {"mov",2,0},
								   {"cmp",2,1},
								   {"add",2,2},
								   {"sub",2,3},
								   {"not",1,4},
								   {"clr",1,5},
								   {"lea",2,6},
							       {"inc",1,7},
			   					   {"dec",1,8},
	                               {"jmp",1,9},
								   {"bne",1,10},
								   {"red",1,11},
								   {"prn",1,12},
								   {"jsr",1,13},
								   {"rts",0,14},
								   {"stop",0,15}
									};

/*is_opcode (char *str)*/
/*checks if a string is an op code, if it is, it returns is binary code in decimal, which is also its index int the array. If it is not an opcode it returns -1*/
/*return : opcode index (=decimal representation of its binary rep.), or -1 if it is not an opcode*/
/*param *str string to be checked*/
int is_opcode(char *str)
{
	int i;
	for(i=0;i<MAX_OPCODES;i++)
	{
		if(strcmp((opcodes_arr[i]).name,str)==0)
			return i;
	}
	return -1;
}/*end of is_opcode()*/





/*-------------------------------------------------------------------------------------------------*/
/*SYMBOL TABLE AND RELATED TASKS*/
/*-------------------------------------------------------------------------------------------------*/
/*adding a new symbol to the symbols' table.*/
Boolean symbol_is_legal(symbol_ptr head, char *symbol_name);
void add_to_symbol_tab(symbol_ptr * head, symbol_ptr *tail,char *myname, Boolean is_in_instruction,Boolean is_external)
{	
	symbol_ptr new_symbol;
	if(!symbol_is_legal(*head,myname))
		return;
	new_symbol = (symbol_ptr) malloc(sizeof(Symbol));	
	if(!new_symbol)
	{
		fprintf(stderr, "Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	
	strcpy(new_symbol->name, myname);
	new_symbol->in_instruction_line=is_in_instruction;
	new_symbol->external=is_external;
	if (new_symbol->in_instruction_line)
	{
		new_symbol->location=IC;
	}
	else if(!(new_symbol->external))
	{
		new_symbol->location=DC;
	}
	if (new_symbol->external)
	{
		new_symbol->location=0;
	}
	/*if the list is empty:*/	
	if(!(*head))
	{
		*head=new_symbol;
		*tail=new_symbol;
		(*tail)->next=NULL;
		return;
	}
	/*if the list is not empty, add the new symbol at the end, updating the tail*/
	(*tail)->next=new_symbol;
	*tail=new_symbol;
	(*tail)->next=NULL;
	return;
}/*end of add_to_symbol_tab()*/

void free_symbol_tab(symbol_ptr *head)
{
	symbol_ptr temp;
	if(!(*head))
		return;
	while(*head)
	{
		temp=*head;
		(*head)=(*head)->next;
		free(temp);
	}
}/*end of free_symbol_tab()*/ 

/*symbol_is_legal(symbol_ptr head, char *symbol_name)*/
/*check if str is a legal symbol*/
/*return: TRUE if it is, FALSE if not*/
/*param symbol_ptr head of symbols' list*/
/*param char *symbol_name to be checked*/
Boolean symbol_is_legal(symbol_ptr head, char *symbol_name)
{	
	int i;
	symbol_ptr cur = head;
	if(is_protected_name(symbol_name))
	{	
		error()
		fprintf(stderr,"Error: Illegal symbol name (protected name)\n");
		can_be_assembled=FALSE;			
		return FALSE;
	}
	for(i=0;i<strlen(symbol_name);i++)
	{
		if((i==0) && !isalpha(symbol_name[i]))/*a symbol should start with a letter*/
		{
			error()
			fprintf(stderr,"Error: Symbol name  must start with a letter\n");
			can_be_assembled=FALSE;			
			return FALSE;			
		}
		if(!(isalnum(symbol_name[i])))
		{	
			error()
			fprintf(stderr,"Error:illegal character in symbol '%c'\n",symbol_name[i]);
			can_be_assembled=FALSE;			
			return FALSE;			
		}
	}
	while(cur)
	{
		if(strcmp(cur->name,symbol_name)==0)
		{	/*another symbol with the same name was found, so this name is illegal*/
			error()
			fprintf(stderr,"Error:Symbol %s is already taken\n",symbol_name);
			can_be_assembled=FALSE;			
			return FALSE;
		}
		cur=cur->next;
	}
	/*the symbol name is unique so it is legal*/
	return TRUE;
}/*end of symbol_is_legal()*/


/*get symbol by ptr*/
symbol_ptr get_symbol(symbol_ptr head,char *symbol_name)
{
	symbol_ptr cur = head;

	while((cur) && (strcmp(cur->name,symbol_name)!=0))/*find the symbol in the table*/
		cur=cur->next;
	
	return cur;
}/*end of get_symbol()*/

/*after parsing the source file symbol's locations should be updated, this function takes care of it*/
void update_symbol_locaction(symbol_ptr *head)
{
	symbol_ptr cur = *head;
	while(cur)
	{
		if((cur->in_instruction_line)==FALSE &&((cur->external)==FALSE))
		{
			cur->location = (cur->location)+IC +STARTING_ADDRESS;
		}
		if((cur->in_instruction_line)==TRUE &&((cur->external)==FALSE))
		{
			cur->location = (cur->location) +STARTING_ADDRESS;
		}
		cur=cur->next;
	}

}

/*-------------------------------------------------------------------------------------------------*/
/*LINES' LIST*/
/*-------------------------------------------------------------------------------------------------*/
void add_line(line_ptr *head, line_ptr * tail, Line_Type mytype, Boolean is_symbol_first,Boolean is_jump_syntax, int myopcode, char *operand1, char *operand2,char *jmp_op1,char *jmp_op2)
{
	line_ptr new_line = (line_ptr)malloc(sizeof(Line));
	if(!new_line)
	{
		fprintf(stderr,"Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	new_line->type  = mytype;
	new_line->symbol_first= is_symbol_first;
	new_line->jump_syntax= is_jump_syntax;
	new_line->opcode=myopcode;
	strcpy(new_line->operand1,operand1);
	strcpy(new_line->operand2,operand2);
	strcpy(new_line->jump_op1,jmp_op1);
	strcpy(new_line->jump_op2,jmp_op2);
	new_line->line_num_in_src=line_num;
	
	if(!(*head))
	{
		*head=new_line;
		*tail=new_line;
		(*tail)->next=NULL;
		return;
	}
	
	(*tail)->next=new_line;
	*tail=new_line;
	(*tail)->next=NULL;
	return;

}
void free_line_list(line_ptr *head)
{
	line_ptr temp;
	if(!(*head))
		return;
	while(*head)
	{
		temp=*head;
		(*head)=(*head)->next;
		free(temp);
	}
}


/*-------------------------------------------------------------------------------------------------*/
/*DATA CODE LIST AND RELATED TASKS*/
/*-------------------------------------------------------------------------------------------------*/

void add_data(data_ptr *head, data_ptr *tail,Line_Type mytype, int mydata)
{
	data_ptr new_data = (data_ptr) malloc (sizeof(Data_Code));
	if(!new_data)
	{
		fprintf(stderr,"Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	new_data->type=mytype;
	
	new_data->data=mydata;
	
	DC++;

	if(!(*head))
	{
		*head=new_data;
		*tail=new_data;
		(*tail)->next=NULL;
		return;
	}
	
	(*tail)->next=new_data;
	*tail=new_data;
	(*tail)->next=NULL;
	return;
}

void free_data_list(data_ptr *head)
{
	data_ptr temp;
	if(!(*head))
		return;
	while(*head)
	{
		temp=*head;
		(*head)=(*head)->next;
		free(temp);
	}
}
