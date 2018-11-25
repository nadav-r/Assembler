/*************************parsing.c*************************************************/
/*parsing.c handles reading the file and saving valuable data  for assembling in the lines, symbols and data lists and also reporting errors. The parsing module is divided into several functions with different tasks: read lines and skip comment lines, identify a line's type, parsing this specific line type (handle symbols if declared).  
*/
#include "parsing.h"
#define error() fprintf(stderr,"In file '%s' line %d: \n\t ",cur_file, line_num);

int line_num=1;/*keeping track of the line numbering in the src file*/
char cur_file[MAX_FILE_NAME+4];/*the name of the file that is currently being processed. global for faciliating error messages*/



/*parse_src_file(char *)*/
/*parse_src_file(char *) opens the source file, reads each line and if it is not a comment line or an empty line it passes it as a parameter for parse_line(char *line).*/
/*parm: char src_file_name[] the file's name without the ".as" ending */
void parse_src_file(char *src_file_name)
{
	FILE  *src_file_fp;
	char as_file_name[MAX_FILE_NAME+4];/*as_file_name is the file name + ".as" so we need extra space*/
	char line[MAX_LINE];
	int c;/*for going over characters in the line*/
	int i;/*index*/
	line_num=1;	
	if(strlen(src_file_name)>MAX_FILE_NAME)
	{	
		fprintf(stderr, "Error: File name '%s' is too long.\n",src_file_name);
		can_be_assembled=FALSE;
		return;
	}
	strcpy(as_file_name,src_file_name);
	strcat(as_file_name,".as");
	strcpy(cur_file, as_file_name);
	
	src_file_fp=fopen(as_file_name, "r");

	if(!src_file_fp)
	{
		fprintf(stderr, "Error: Could not open the source file '%s'\n",as_file_name);
		can_be_assembled=FALSE;
		return;
	}

	while(fgets(line, MAX_LINE,src_file_fp )!=NULL)/*read each line, if it is not empty or comment call parse_line*/
	{
		Boolean to_ignore=FALSE;
		for(i=0;i<strlen(line);i++)
		{
			c=line[i];
			if(!isspace(c))
			{
				to_ignore= (c==';')? TRUE: FALSE;/*found a comment line - ignore it*/
				break;
			}
			if(i==strlen(line)-1)/*found an empty line - ignore it*/
			{
				to_ignore=TRUE;
			}
		}
		if(to_ignore==FALSE)
		{
			parse_line(line);
		}
		line_num++;	
	}	
	fclose(src_file_fp);
}

/*parse_line(char *) identifies the line type (using auxilary functions) and calls the relevant function to continue parsing.
It creates a copy of original line for facilating the other functions in tokenizing the line*/
/*parm char line[] read by parse_src_file*/
void parse_line(char *line)	
{
	int line_length=strlen(line)+1;
	int opcode=-1;
	char *line_copy=(char *)malloc(line_length);/*auxilary copy for continuing parsing and tokenizing the lines in the relevant functions called from here*/
	Boolean line_identified=FALSE;
	if(!line_copy)
	{
		fprintf(stderr,"Memory Allocation Failed\n");
		exit(EXIT_FAILURE);
	}
	strcpy(line_copy,line);
	
	if((opcode=is_instruction_line(line))!=-1)/*is_instruction_line() returns -1 if opcode not found*/
	{
		parse_instruction_line(line,line_copy,opcode);
		line_identified=TRUE;
	}
	else if(is_data_line(line))
	{
		parse_data_line(line,line_copy);
		line_identified=TRUE;
	}
	else if(is_string_line(line))
	{	
		parse_string_line(line,line_copy);
		line_identified=TRUE;
	}
	else if(is_extern_line(line))
	{
		parse_extern_line(line,line_copy);
		line_identified=TRUE;
	}
	else if(is_entry_line(line))
	{
		parse_entry_line(line,line_copy);
		line_identified=TRUE;
	}
	
	free(line_copy);

	if(line_identified==FALSE)
	{	
		error()
		fprintf(stderr,"Error: Line does not match any legal line type\n");
		can_be_assembled=FALSE;
		return;
	}
	return;
}


/*The following functions deal with specific line types. Each line type can either begin with a symbol or not. Accordingly, each line has 3 associated functions: one to decide if it begins with a symbol or not, one to parse it with a symbol, and one without a symbol*/

void parse_data_line(char *line,char *line_copy)
{
	Boolean starts_with_symbol=FALSE;
	char temp[MAX_SYMBOL_NAME];
	sscanf(line, " %s ", temp);

	if(strcmp(temp,".data")!=0)/*checking first token, whether it is a symbol or not*/
	{
		starts_with_symbol=TRUE;
	}
	if(starts_with_symbol)
	{
		parse_data_line_symbol(line,line_copy);
	}
	if(starts_with_symbol==FALSE)
	{
		parse_data_line_no_symbol(line,line_copy);
	}
}


/*parsing data lines with no preceeding symbols , adding data to data list and lines to line list*/
void parse_data_line_no_symbol(char *line,char *line_copy)
{
	char *token;
	int num;/*for storing data*/
	char adjusted_token[8];/*token after removal of white spaces and sign*/
	int c;/*going throuh chars*/
	int i;/*general index*/
	int token_i;/*where is the token in the line_copy*/
	int delim_i;/*where is the delimitor in the line_copy (strtok changes line)*/
	char *start;/*for calculating the index of token */
	char delim;
	Boolean with_sign;
	int first_digit_i=-1;/*index of first index (*/
	int j=0;/*another index*/
	char sign;/*'+' '-'*/
	Boolean first_num_found=FALSE;
	Boolean first_loop=TRUE;/*for validating that at least one number was added after first while loop*/
	int prev_c= '\0';
	start=line;
	/*check comma rules*/
	for(i=0;i<strlen(line);i++)
	{
		c=line[i];
		if(isspace(c))
			continue;
		if(c==',')
		{
			if(prev_c==',')
			{
			error() 
			fprintf(stderr, "Error: Illegal comma\n");
			can_be_assembled=FALSE;
			return;
			}
		}
		prev_c=c;
	}
	/*get index of last comma, check that something follows it*/
	if(strrchr(line,','))
	{
		i=strrchr(line,',')-start;
		i++;
		c=(i<strlen(line))? line[i]: '\0';
		for(;(i<strlen(line))&&(isspace(c));i++)
		{
			c=line[i];
		}
		if(i==strlen(line))
		{
			error() 
			fprintf(stderr, "Error: Illegal comma\n");
			can_be_assembled=FALSE;
			return;
		}
	}
	token=strtok(line, " \t");/*this should get ".data" with or without preceding white space(parse_data_line_with symbol takes care of calling the function without a symbol)*/
	token_i=token-start;/*calculate token index*/
	delim_i=token_i+strlen(token);
	i=delim_i;
	c=line_copy[i];
	/*check syntax*/
	while((c) && (isspace(c)) &&(i<strlen(line_copy)-1))
	{	
		c=line_copy[++i];
	}
	if((c != '-') && (c != '+') &&(!isdigit(c)) &&(c!='\0') && c!='\n') 
	{	
		if(c==',')	
		{	
			error() 
			fprintf(stderr, "Error: Illegal comma\n");
		}
		else
		{
			error() 
			fprintf(stderr, "Error: Illegal character '%c'\n",c);
		}
		can_be_assembled=FALSE;
		return;
	}
	
	token=strtok(NULL, " \t ,");/*get first number parameter - the must be at least one*/
	while(token!=NULL)
	{
		token_i=token-start;
		delim_i=token_i+strlen(token);
		delim = line_copy[delim_i];
		i=delim_i;
		c=(i+1<strlen(line_copy))? line_copy[++i]: '\0';
		/*syntax checking*/
		while((c) && (isspace(c))&& (i<strlen(line_copy)-1))
		{
			c=line_copy[++i];
		}
		if((delim !=',') && (c!=',') && (c!='\0') && (first_num_found==TRUE)&& (c!='\n'))
		{
			error() 
			fprintf(stderr, "Error: Missing comma\n");
			can_be_assembled=FALSE;
			return;
		}
		
		if((c!=',')&&(c!='+')&&(c!='-')&&(!isdigit(c)) && (c!='\0')&& (c!='\n')) 
		{
			error() 
			fprintf(stderr, "Error: Illegal character '%c'\n",c);
			can_be_assembled=FALSE;
			return;
		}
		/*check data legality and if preceded by a sign*/
		for(i=0;i<strlen(token);i++)
		{
			c=token[i];
			if(c=='-' || c== '+')
			{
				with_sign=TRUE;
				sign=c;
			}
			if( isdigit(c))
			{
				if(first_digit_i==-1)
				{
					first_digit_i=i;
				}
				adjusted_token[j++]=(char)c;/*adjusted_token includes only digits and no sign*/				
			}
			if((c!='+')&&(c!='-')&&(!isdigit(c))&&(!isspace(c)))
			{
				error() 
				fprintf(stderr, "Error: Illegal character '%c'\n",c);
				can_be_assembled=FALSE;
				return;
			}		
		}
		if((with_sign==TRUE) && (first_digit_i==-1))
		{
			error()
			fprintf(stderr, "Error: Must enter a number\n");
			can_be_assembled=FALSE;
			return;
		}
		else if(first_digit_i==-1)
		{	
			if(first_loop==TRUE)
			{
				error()
				fprintf(stderr, "Error: Must enter at least one number\n");
				can_be_assembled=FALSE;
			return;
			}
			break;/*nothing to add to data, go outside of while loop*/
		}
		/*convert string representation to an int*/
		num=atoi(adjusted_token);
		if(with_sign==TRUE && sign=='-')
		{
			num=-num;
		}
		/*add data to the data list*/
		add_data(&data_list_h,&data_list_t, DATA,num);
		/*prepare for next data*/
		first_num_found=TRUE; 
		token = strtok(NULL," \t ,");
		adjusted_token[0]='\0';
		j=0;
		with_sign=FALSE;
		first_digit_i=-1;
		sign='+';	
		first_loop=FALSE;
	}
	/*add line to the line list*/
	add_line(&line_list_h, &line_list_t, DATA, FALSE/*in any case, symbol is ignored later*/,FALSE/*not in instruction line*/, -1/*no opcode*/, "dummy", "dummy","dummy","dummy");
}

/*parse data line with preceding symbol, it finds the symbol enters it to the symbol list, and calss parse_data_line_no_symbol with a shorter string that does not include the symbol for further parsing*/
void parse_data_line_symbol(char *line,char *line_copy)
{
	
	char *symbol_name;
	char symbol_name_adjusted[MAX_SYMBOL_NAME];/*after removal of spaces and alignment*/
	int symbol_i;/*index for symbol in the line*/
	int c;	/*for going over characters*/
	char *start;
	int delim_i;
	int i;
	start=line;
	symbol_name=strtok(line,":");
	symbol_i=symbol_name-start;
	delim_i=symbol_i+strlen(symbol_name);
	c=line_copy[delim_i];
	if(c!=':')
	{
			error() 
			fprintf(stderr,"Error: Symbol must be followed by ':'m otherwise, instruction line should begin with an instruction word\n");
			can_be_assembled=FALSE;
			return;
	}
	if(delim_i==0)
	{
		error() 
		fprintf(stderr,"Error: line cannot start with ':'\n");
		can_be_assembled=FALSE;
		return;			
	}

	if(delim_i>0 && isspace(line_copy[delim_i-1]))
	{
		error() 
		fprintf(stderr, "Error: Illegal space ':' should immediately come after the symbol");
		can_be_assembled=FALSE;
		return;
	}
	/*should check that symbol name is ok, and remove preceded white space*/
	i=0;
	c=symbol_name[i];
	while(c && isspace(c))
	{
		c=symbol_name[++i];
	}
	strcpy(symbol_name_adjusted,symbol_name+i);
	add_to_symbol_tab(&symbol_list_h, &symbol_list_t,symbol_name_adjusted, FALSE,FALSE);


	if (can_be_assembled==TRUE)
	{
		parse_data_line_no_symbol(line+delim_i+1,line_copy+delim_i+1);/*call parse_data_line_no_symbol with shortened strings*/
	}
}

/*parse_string_line follows the same logic of parse_data_line.*/
void parse_string_line(char *line,char *line_copy)
{
	Boolean starts_with_symbol=FALSE;
	char temp[MAX_SYMBOL_NAME];
	sscanf(line, " %s ", temp);
	if(strcmp(temp,".string")!=0)
	{
		starts_with_symbol=TRUE;
	}
	if(starts_with_symbol)
	{
		parse_string_line_symbol(line,line_copy);
	}
	if(starts_with_symbol==FALSE)
	{
		parse_string_line_no_symbol(line,line_copy);
	}
}

/*parse_string_line_no_symbol follows the same logic of parse_data_line_no_symbol */
void parse_string_line_no_symbol(char *line,char *line_copy)
{
	char *token;
	int new_char;
	int c;
	int i;
	int token_i;
	int delim_i;
	char *start;
	char delim;
	start=line;
	token=strtok(line, "\"");
	token_i=token-start;
	delim_i=token_i+strlen(token);
	c=line_copy[delim_i];
	if (c!='"')
	{
		error() 
		fprintf(stderr, "Error: A string should start and end with '\"'\n");
		can_be_assembled=FALSE;
		return;
	}
	token = strtok(NULL, "\"");/*get what is between the "...." */
	token_i=token-start;
	delim_i=token_i+strlen(token);
	delim=line_copy[delim_i];
	if (delim!='"')
	{
		error() 
		fprintf(stderr, "Error: A string should start and end with '\"'\n");
		can_be_assembled=FALSE;
		return;
	}
	i=delim_i;
	c=line_copy[i];
	while(c)
	{
		c=line_copy[++i];
		if(!isspace(c)&&(c!='\0'))
		{
			error() 
			fprintf(stderr, "Error: Extraneous text \n");
			can_be_assembled=FALSE;
			return;
		}
	}
	for(i=0;i<strlen(token);i++)
	{	/*add chars one after another to data list */
		new_char=token[i];
		add_data(&data_list_h,&data_list_t, STRING,new_char);
	}
	add_data(&data_list_h,&data_list_t, STRING,'\0');/*to signal end of string*/
	add_line(&line_list_h, &line_list_t, STRING, FALSE/*in any case, symbol is ignored later on*/,FALSE/*not instruction line*/, -1/*no opcode*/, "dummy", "dummy","dummy","dummy");
}

/*parse_string_line_symbol follows the same logic of parse_data_line_symbol */
void parse_string_line_symbol(char *line,char *line_copy)
{
	char *symbol_name;
	char symbol_name_adjusted[MAX_SYMBOL_NAME];
	int symbol_i;
	int c;	
	char *start;
	int delim_i;
	int i;
	start=line;
	symbol_name=strtok(line,":");
	symbol_i=symbol_name-start;
	delim_i=symbol_i+strlen(symbol_name);
	c=line_copy[delim_i];
	if(c!=':')
	{
			error() 
			fprintf(stderr,"Error: Symbol must be followed by ':'m otherwise, instruction line should begin with an instruction word\n");
			can_be_assembled=FALSE;
			return;
	}
	if(delim_i==0)
	{
		error() 
		fprintf(stderr,"Error: A line cannot start with ':'\n");
		can_be_assembled=FALSE;
		return;			
	}

	if(delim_i>0 && isspace(line_copy[delim_i-1]))
	{
		error() 
		fprintf(stderr, "Error: Illegal space ':' should come immediately after the symbol\n");
		can_be_assembled=FALSE;
		return;
	}
	/*should check that symbol name is ok, and remove preceded white space*/
	i=0;
	c=symbol_name[i];
	while(c && isspace(c))
	{
		c=symbol_name[++i];
	}
	strcpy(symbol_name_adjusted,symbol_name+i);
	add_to_symbol_tab(&symbol_list_h, &symbol_list_t,symbol_name_adjusted, FALSE,FALSE);

	if (can_be_assembled==TRUE)
	{
		parse_string_line_no_symbol(line+delim_i+1,line_copy+delim_i+1);/*call parse_string_line_no_symbol with shortened strings*/
	}
}



/*checking legality  and saveing the line with parameter in the line list (however, not saving symbol parameter in symbol list)*/
void parse_entry_line_no_symbol(char *line,char *line_copy)
{
	char *token;
	char operand[MAX_SYMBOL_NAME];
	char adjusted_token[MAX_SYMBOL_NAME];
	int token_i;		
	int delim_i;
	int c;	
	int temp_c;
	int i;
	char *start;
	start=line;	
	strcpy(operand,"dummy");
	token = strtok(line, " \t");
	token_i=(token-start);
	delim_i=token_i+strlen(token);
	c=line_copy[delim_i];
	while(c && isspace(c))
	{
		c=line_copy[++delim_i];
	}	
	if(c=='\0')
	{
		error() 
		fprintf(stderr,"Error: Missing operand\n");
		can_be_assembled=FALSE;
		return;
	}
	if(c==',')
	{
		error() 
		fprintf(stderr,"Error: Illegal comma\n");
		can_be_assembled=FALSE;
		return;
	}
	token = strtok(NULL, " \t");
	i=0;
	temp_c=token[i];
	while(temp_c && isspace(temp_c))
	{
		temp_c=token[++i];
	}
	strcpy(adjusted_token,token+i);
	strcpy(operand,adjusted_token);
	token_i=(token-start);
	delim_i=token_i+strlen(token);
	c=line_copy[delim_i];
	while(c && isspace(c))
	{
		c=line_copy[++delim_i];
	}
	c=line_copy[delim_i++];			
	while(c && isspace(c))
	{
		c=line_copy[++delim_i];
	}
	if(c!='\0')
	{
		error() 
		fprintf(stderr,"Error: Extraneous text \n");
		can_be_assembled=FALSE;
	}

	for (i=0;i<strlen(operand);i++)
	{	
		c=operand[i];		
		if(!(isalnum(c))&&(c!='\0')&&(c!='\n'))
		{

			error() 
			fprintf(stderr,"Error: Illegal character '%c' \n",c);
			can_be_assembled=FALSE;
		}
	}
	
	if(operand[strlen(operand)-1]=='\n')
	{
		operand[strlen(operand)-1]='\0';
	}
	add_line(&line_list_h, &line_list_t, ENTRY, FALSE/*in any case, a preceding symbol is ignored*/,FALSE, -1/*no opcode*/, operand, "dummy","dummy","dummy");
	
}


/*checks symbol legality (but do not add it to symbol list), calls parse_entry_line_no symbol for further parsing*/
void parse_entry_line_symbol(char *line,char *line_copy)
{
	char *symbol_name;
	char symbol_name_adjusted[MAX_SYMBOL_NAME];
	int symbol_i;
	int c;	
	char *start;
	int delim_i;
	int i;
	/*Boolean space_found=FALSE;
	int first_space_i=-1;*/
	start=line;
	symbol_name=strtok(line,":");
	symbol_i=symbol_name-start;
	delim_i=symbol_i+strlen(symbol_name);
	c=line_copy[delim_i];
	if(c!=':')
	{
			error() 
			fprintf(stderr,"Error: Symbol must be followed by ':' otherwise, .entry line should begin with an '.entry' word\n");
			can_be_assembled=FALSE;
			return;
	}
	if(delim_i==0)
	{
		error() 
		fprintf(stderr,"Error: A line cannot start with ':'\n");
		can_be_assembled=FALSE;	
		return;		

	}
	if(delim_i>0 && isspace(line_copy[delim_i-1]))
	{
		error() 
		fprintf(stderr, "Error: Illegal space ':' should come immediately after the symbol");
		can_be_assembled=FALSE;
		return;
	}
	i=0;
	c=symbol_name[i];
	while(c && isspace(c))
	{
		c=symbol_name[++i];
	}
	strcpy(symbol_name_adjusted,symbol_name+i);
	symbol_is_legal(symbol_list_h, symbol_name_adjusted);/*this checks the name, updating flag can_be_assembled, anyway i do not add the symbol to symbol tab*/
	if (can_be_assembled==TRUE)
	{
		error() 
		fprintf(stderr, "Warrning: Symbol before .entry line is ignored\n");
		parse_entry_line_no_symbol(line+delim_i+1,line_copy+delim_i+1);
	}
}

/*similar to parse_entry_line_no_symbol except that it adds the symbol parameter to symbol list with external flag*/
void parse_extern_line_no_symbol(char *line,char *line_copy)
{
	char *token;
	char operand[MAX_SYMBOL_NAME];
	char adjusted_token[MAX_SYMBOL_NAME];
	int token_i;		
	int delim_i;
	int c;	
	int temp_c;
	int i;
	char *start;
	start=line;	
	strcpy(operand,"dummy");
	token = strtok(line, " \t");
	token_i=(token-start);
	delim_i=token_i+strlen(token);
	c=line_copy[delim_i];
	while(c && isspace(c))
	{
		c=line_copy[++delim_i];
	}	
	if(c=='\0')
	{
		error() 
		fprintf(stderr,"Error: Missing operand\n");
		can_be_assembled=FALSE;
		return;
	}
	if(c==',')
	{
		error() 
		fprintf(stderr,"Error: Illegal comma\n");
		can_be_assembled=FALSE;
		return;
	}
	token = strtok(NULL, " \t");
	i=0;
	temp_c=token[i];
	while(temp_c && isspace(temp_c))
	{
		temp_c=token[++i];
	}
	strcpy(adjusted_token,token+i);
	strcpy(operand,adjusted_token);
	token_i=(token-start);
	delim_i=token_i+strlen(token);
	c=line_copy[delim_i];
	while(c && isspace(c))
	{
		c=line_copy[++delim_i];
	}
	c=line_copy[delim_i++];			
	while(c && isspace(c))
	{
		c=line_copy[++delim_i];
	}
	if(c!='\0')
	{
		error() 
		fprintf(stderr,"Error: Extraneous text \n");
		can_be_assembled=FALSE;
		return;
	}
	for (i=0;i<strlen(operand);i++)
	{	
		c=operand[i];		
		if(!(isalnum(c))&&(c!='\0')&&(c!='\n'))
		{
			error() 
			fprintf(stderr,"Error: illegal character '%c'\n",c);
			can_be_assembled=FALSE;
			return;
		}
	}

	if(strlen(operand)>0 && operand[strlen(operand)-1]=='\n')/*adjust operand*/
	{
		operand[strlen(operand)-1]='\0';
	}		
	add_line(&line_list_h, &line_list_t, EXTERN, FALSE/*in any case, symbol is ignored later on*/,FALSE, -1/*no opcode*/, operand, "dummy","dummy","dummy");
	add_to_symbol_tab(&symbol_list_h, &symbol_list_t,operand, FALSE,TRUE);
}


/*checks symbol legality (but do not add it to symbol list), calls parse_extern_line_no symbol for further parsing*/
void parse_extern_line_symbol(char *line,char *line_copy)
{
	char *symbol_name;
	char symbol_name_adjusted[MAX_SYMBOL_NAME];
	int symbol_i;
	int c;	
	char *start;
	int delim_i;
	int i;
	/*Boolean space_found=FALSE;
	int first_space_i=-1;*/
	start=line;
	symbol_name=strtok(line,":");
	symbol_i=symbol_name-start;
	delim_i=symbol_i+strlen(symbol_name);
	c=line_copy[delim_i];
	if(c!=':')
	{
			error() 
			fprintf(stderr,"Error: Symbol must be followed by ':' otherwise, .extern line should begin with '.extern' word\n");
			can_be_assembled=FALSE;
			return;
	}
	if(delim_i==0)
	{
		error() 
		fprintf(stderr,"Error: A line cannot start with ':'\n");
		can_be_assembled=FALSE;			
		return;
	}
	if(delim_i>0 && isspace(line_copy[delim_i-1]))
	{
		error() 
		fprintf(stderr, "Error: Illegal space ':' should come immediately after the symbol\n");
		can_be_assembled=FALSE;
		return;
	}
	i=0;
	c=symbol_name[i];
	while(c && isspace(c))
	{
		c=symbol_name[++i];
	}
	strcpy(symbol_name_adjusted,symbol_name+i);
	symbol_is_legal(symbol_list_h, symbol_name_adjusted);
	if (can_be_assembled==TRUE)
	{
		error() /*it's actually a warnning but error() macro only specifies line and file name so it is ok*/
		fprintf(stderr, "Warrning: Symbol before .extern line is ignored\n");
		parse_extern_line_no_symbol(line+delim_i+1,line_copy+delim_i+1);
	}
}


/*check if it starts with symbol or not call the right function*/
void parse_entry_line(char *line,char *line_copy)
{
	Boolean starts_with_symbol=FALSE;
	char temp[MAX_SYMBOL_NAME];
	sscanf(line, " %s ", temp);
	if(strcmp(temp,".entry")!=0)
	{
		starts_with_symbol=TRUE;
	}
	if(starts_with_symbol)
	{
		parse_entry_line_symbol(line,line_copy);
	}
	if(starts_with_symbol==FALSE)
	{
		parse_entry_line_no_symbol(line,line_copy);
	}
}
/*check if it starts with symbol or not call the right function*/
void parse_extern_line(char *line,char *line_copy)
{
	Boolean starts_with_symbol=FALSE;
	char temp[MAX_SYMBOL_NAME];
	sscanf(line, " %s ", temp);
	if(strcmp(temp,".extern")!=0)
	{
		starts_with_symbol=TRUE;
	}
	if(starts_with_symbol)
	{
		parse_extern_line_symbol(line,line_copy);
	}
	if(starts_with_symbol==FALSE)
	{
		parse_extern_line_no_symbol(line,line_copy);
	}
}



/*parsing lines with opcodes, checking for syntax and general legality and adding to line list.
first, if present, take care of preceding symbol, then parse with no symbol*/
void parse_instruction_line(char *line, char *line_copy, int opcode)
{
	Boolean starts_with_symbol=FALSE;
	char temp[MAX_SYMBOL_NAME];
	sscanf(line, " %s ", temp);
	if(strcmp(temp,(opcodes_arr[opcode]).name)!=0)
	{
		starts_with_symbol=TRUE;
	}
	if(starts_with_symbol)
	{
		parse_instruction_line_symbol(line,line_copy,opcode);
	}
	else
	{ 
		parse_instruction_line_no_symbol(line,line_copy,opcode,FALSE);
	}
}
/*parsing instruction line with preceding symbol, processing the symbol and call parse_instruction_line_no_symbol()*/
void parse_instruction_line_symbol(char *line,char *line_copy,int opcode)
{
	char *symbol_name;
	char symbol_name_adjusted[MAX_SYMBOL_NAME];
	int symbol_i;
	int c;	
	char *start;
	int delim_i;
	int i;
	/*Boolean space_found=FALSE;
	int first_space_i=-1;*/
	start=line;
	symbol_name=strtok(line,":");
	symbol_i=symbol_name-start;
	delim_i=symbol_i+strlen(symbol_name);
	c=line_copy[delim_i];
	if(c!=':')
	{
			error() 
			fprintf(stderr,"Error: Symbol must be followed by ':'m otherwise, instruction line should begin with an instruction word\n");
			can_be_assembled=FALSE;
			return;
	}
	if(delim_i==0)
	{
		error() 
		fprintf(stderr,"Error: A line cannot start with ':'\n");
		can_be_assembled=FALSE;	
		return;		

	}
	if(delim_i>0 && isspace(line_copy[delim_i-1]))
	{

		error() 
		fprintf(stderr, "Error: illegal space ':' should come immediately after symbol\n");
		can_be_assembled=FALSE;
	}

	i=0;/*should check that symbol name is ok, and remove attached white space*/
	c=symbol_name[i];
	while(c && isspace(c))
	{
		c=symbol_name[++i];
	}
	strcpy(symbol_name_adjusted,symbol_name+i);
	add_to_symbol_tab(&symbol_list_h, &symbol_list_t,symbol_name_adjusted, TRUE,FALSE);
	if (can_be_assembled==TRUE)
	{
		parse_instruction_line_no_symbol(line+delim_i+1,line_copy+delim_i+1,opcode, TRUE);
	}
}

/*parsing instruction lines with no preceding symbol, adding them to line list*/
void parse_instruction_line_no_symbol(char *line, char *line_copy, int opcode,Boolean is_symbol_first)
{	
	char *token;
	char operand1[MAX_SYMBOL_NAME];
	char operand2[MAX_SYMBOL_NAME];
	char jump_op1[MAX_SYMBOL_NAME];
	char jump_op2[MAX_SYMBOL_NAME];
	char adjusted_token[MAX_SYMBOL_NAME];
	int token_i;		
	int delim_i;
	int c;	
	int temp_c;
	int i;
	int j;
	char *start;
	char *operands_arr[4];
	Boolean jump_syntax=FALSE;
	ADDRESING_MODE mode;
	start=line;	
	
	/*parse according to the number of required parameters*/
	if((opcodes_arr[opcode]).req_parametes==0)
	{	
		/*general note about the dummy value: it does not matter if the user actually uses the string "dummy" as an operand or symbol because if any operand is legal then operand of addresing mode 1 is legal (according to the chart in page 27). And dummy is interpreted as of addressing mode 1 and is legal syntax-wise so everything will work fine*/
		
		/*1 parameter, so fill operands with dummy value*/
		strcpy(operand1,"dummy");
		strcpy(operand2,"dummy");
		strcpy(jump_op1,"dummy");
		strcpy(jump_op2,"dummy");
		token = strtok(line, " ");/*gets the opcode*/
		token_i=(token-start);
		delim_i=token_i+strlen(token);
		c=line_copy[delim_i];
		/*see that nothing else follows the opcode name*/
		while(c && isspace(c))
		{
			c=line_copy[++delim_i];
		}
		if(c!='\0')
		{
			error() 
			fprintf(stderr,"Error: Extraneous text\n");
			can_be_assembled=FALSE;
			return;
		}	
	}

	if((opcodes_arr[opcode]).req_parametes==1)
	{
		strcpy(operand1,"dummy");/*1 parameter - only dest operand is relevant*/
		token = strtok(line, " \t");/*get the opcode name*/
		token_i=(token-start);
		delim_i=token_i+strlen(token);
		c=line_copy[delim_i];
		while(c && isspace(c))
		{
			c=line_copy[++delim_i];
		}
		if(c=='\0')
		{
			error() 
			fprintf(stderr,"Error: Missing operand. \n");
			can_be_assembled=FALSE;
			return;
		}
		if(c==',')
		{
			error() 
			fprintf(stderr,"Error: Illegal comma. \n");
			can_be_assembled=FALSE;
			return;
		}
		if(strchr(line_copy+delim_i,'('))/*1 parameter opcodes could have two types of syntax -jump or not, if there is '(' i expect for jump syntax*/
		{
			token = strtok(NULL, "(");
		}
		else token = strtok(NULL, "");
		i=0;
		temp_c=token[i];
		while(temp_c && isspace(temp_c))
		{
			temp_c=token[++i];
		}
		strcpy(adjusted_token,token+i);/*removing extra space*/
		strcpy(operand2,adjusted_token);
		token_i=(token-start);
		delim_i=token_i+strlen(token);
		c=line_copy[delim_i];
		while(c && isspace(c))
		{	
			c=line_copy[++delim_i];
		}
		if(c=='(')/*the delimiter used signifies jump syntax*/
			jump_syntax=TRUE;
		if(jump_syntax==FALSE)
		{
			strcpy(jump_op1,"dummy");/*not relevant operands*/
			strcpy(jump_op2,"dummy");		
		}
		if(jump_syntax==TRUE)/*parse for the case that it is jump syntax, and later if not*/
		{	
			c=line_copy[++delim_i];	
			while(c && isspace(c))
			{
				c=line_copy[++delim_i];
			}
			if(c=='\0')
			{
				error() 
				fprintf(stderr,"Error: Missing operand \n");
				can_be_assembled=FALSE;
				return;
			}
			if(c==',')
			{
				error() 
				fprintf(stderr,"Error: Illegal comma \n");
				can_be_assembled=FALSE;
				return;
			}
			else if(!isalnum(c) && c!='#')
			{		
				error() 
				fprintf(stderr,"Error: Illegal character '%c' \n",c);
				can_be_assembled=FALSE;
				return;
			}
			token=strtok(NULL,",");/*get second jump operand*/
			i=0;
			temp_c=token[i];
			while(temp_c && isspace(temp_c))
			{
				temp_c=token[++i];
			}
			strcpy(adjusted_token,token+i);/*adjust token (remove preceding white space)*/
			strcpy(jump_op1,adjusted_token);
			token_i=(token-start);
			delim_i=token_i+strlen(token);
			c=line_copy[delim_i];
			while(c && isspace(c) && c!=',')
			{
				c=line_copy[++delim_i];
			}
			if(c!=',')
			{
				error() 
				fprintf(stderr,"Error: Missing comma \n");
				can_be_assembled=FALSE;
				return;			
			}
			c=line_copy[++delim_i];
			while(c && isspace(c))
			{
				c=line_copy[++delim_i];
			}			
			if(c=='\0')
			{
				error() 
				fprintf(stderr,"Error: Missing operand \n");
				can_be_assembled=FALSE;
				return;
			}
			if(c==',')
			{
				error() 
				fprintf(stderr,"Error: Illegal comma \n");
				can_be_assembled=FALSE;
				return;
			}		
			token=strtok(NULL,")");/*get second jump operand*/
			i=0;
			temp_c=token[i];
			while(temp_c && isspace(temp_c))
			{
				temp_c=token[++i];
			}
			strcpy(adjusted_token,token+i);
			strcpy(jump_op2,adjusted_token);
			token_i=(token-start);
			delim_i=token_i+strlen(token);
			c=line_copy[delim_i];	
			while(c && isspace(c) && c!=')')
			{
				c=line_copy[++delim_i];
			}
			if(c!=')')
			{
				error() 
				fprintf(stderr,"Error: Missing ')' \n");
				can_be_assembled=FALSE;
				return;
			}
			c=line_copy[++delim_i];
			while(c && isspace(c))
			{
				c=line_copy[++delim_i];
			}			
			if(c!='\0')
			{
				error() 
				fprintf(stderr,"Error: Extraneous text \n");
				can_be_assembled=FALSE;
				return;
			}
		}
		else /*not jump syntax*/
		{
			c=line_copy[delim_i++];	
		
			while(c && isspace(c))
			{
				c=line_copy[++delim_i];
			}
			if(c!='\0')
			{
				error() 
				fprintf(stderr,"Error: Extraneous text\n");
				can_be_assembled=FALSE;
				return;
			}			

		}
	}/*end of routine for opcodes with 1 operand*/
	if((opcodes_arr[opcode]).req_parametes==2)
	{
		strcpy(jump_op1,"dummy");
		strcpy(jump_op2,"dummy");
		token = strtok(line, " ");
		token_i=(token-start);
		delim_i=token_i+strlen(token);
		c=line_copy[delim_i];
		while(c && isspace(c))
		{
			c=line_copy[++delim_i];
		}
		if(c=='\0')
		{
			error() 
			fprintf(stderr,"Error: Missing operand \n");
			can_be_assembled=FALSE;
			return;
		}
		if(c==',')
		{
			error() 
			fprintf(stderr,"Error: Illegal comma \n");
			can_be_assembled=FALSE;
			return;
		}
		else if(!isalnum(c) && c!='#'&&c!='\0')
		{	
			error() 	
			fprintf(stderr,"Error: Illegal character,'%c' \n",c);
			can_be_assembled=FALSE;
			return;
		}
		token = strtok(NULL, ",");
		i=0;
		temp_c=token[i];
		while(temp_c && isspace(temp_c))
		{
			temp_c=token[++i];
		}
		strcpy(adjusted_token,token+i);
		strcpy(operand1,adjusted_token);
		token_i=(token-start);
		delim_i=token_i+strlen(token);
		c=line_copy[delim_i];
		while(c && isspace(c))
		{
			c=line_copy[++delim_i];
		}
		if(c!=',')
		{
			error() 
			fprintf(stderr,"Error: Missing comma\n");
			can_be_assembled=FALSE;
			return;			
		}
		token=strtok(NULL, " ");
		i=0;
		temp_c=token[i];
		while(temp_c && isspace(temp_c))
		{
			temp_c=token[++i];
		}
		strcpy(adjusted_token,token+i);
		strcpy(operand2,adjusted_token);
		token_i=(token-start);
		delim_i=token_i+strlen(token);
		c=line_copy[delim_i];
		while(c && isspace(c))
		{
			c=line_copy[++delim_i];
		}
		if(c!='\0')
		{
			error() 
			fprintf(stderr,"Error: Extraneous text\n");
			can_be_assembled=FALSE;	
			return;		
		}		
	}/*end of routine for opcodes with 2 operand*/

	/*now scanning and parsing the line is finished. so start checking legality of operands and see if the syntax comply to the opcode by checking the addressing mode table*/
	
	/*add operands to operands array and check if they compy to syntax*/
	operands_arr[0]=operand1;
	operands_arr[1]=operand2;
	operands_arr[2]=jump_op1;
	operands_arr[3]=jump_op2;
	/*check the operands (in any case all operands are present, some are dummy but "syntactically legal")
		and during this procedure remove extra space */
	for(i=0;i<=3;i++)
	{
		Boolean immediate_op_expected=FALSE;/*flag that turns on if we find '#'*/
		Boolean num_found=FALSE;
		Boolean space_found=FALSE;
		int first_space_i=-1;/*for adjusting parameters*/
		char *operand_p=operands_arr[i];/*auxilary pointer*/		
		for(j=0;j<strlen(operand_p);j++)/*go through each operand*/
		{	
			if(isspace(operand_p[j]))
			{
				space_found=TRUE;
				if(first_space_i==-1)
				{
					first_space_i=j;/*for faciliating removing extra space*/
				}
			}
			else if(space_found==TRUE)
			{
				error() 
				fprintf(stderr,"Error: Illegal space\n");
				can_be_assembled=FALSE;
				return;		
			}
			if((!isalnum(operand_p[j])) && (operand_p[j]!='#')&&(operand_p[j]!='-')&& (operand_p[j]!='+') && (!isspace(operand_p[j])))
			{
				error() 	
				fprintf(stderr,"Error: Illegal character '%c' \n",operand_p[j]);
				can_be_assembled=FALSE;
				return;
			}
			if(operand_p[j]=='#')
			{
				if(j!=0)
				{
					error() 
					fprintf(stderr,"Error: '#' should be placed before an immediate operand \n");
					can_be_assembled=FALSE;
					return;
				}
				if(strlen(operand_p)<2)
				{
					error() 
					fprintf(stderr,"Error: '#' should be followed by a digit or a sign \n");
					can_be_assembled=FALSE;
					return;
				}
				immediate_op_expected=TRUE;
			}
			if(((operand_p[j]=='-') ||(operand_p[j]=='+')) &&(immediate_op_expected==FALSE))
			{
					error() 
					fprintf(stderr,"Error: '#' is expected before immediate operands  \n");
					can_be_assembled=FALSE;
					return;			
			}	
			if((j==0) && (isdigit(operand_p[j])))
			{
					error() 
					fprintf(stderr,"Error: '#' is expected before immediate operands\n");
					can_be_assembled=FALSE;
					return;
			}
			if((j==1) && (isalpha(operand_p[j]))&& (!isalpha(operand_p[j-1])) )
			{
				error() 
				fprintf(stderr,"Error: operand preceded by illegal character '%c' \n",operand_p[j-1]);
				can_be_assembled=FALSE;
				return;
			}
			if(isdigit(operand_p[j]))
			{
				num_found=TRUE;
			}
		}/*end of inner for  loop*/
		
		/*check flags compatibility*/
		if((immediate_op_expected==TRUE) && (num_found==FALSE))
		{
			error() 
			fprintf(stderr,"Error: Missing immediate operand\n");
			can_be_assembled=FALSE;
			return;
		}
		if(can_be_assembled==TRUE && space_found==TRUE)
		{
			operand_p[first_space_i]='\0';/*adjusting operand*/
		}
	}/*end of outer for loop*/

	/*now that the operands were checked, see if the addressing modes comply to the addressing modes table according to opcode*/

	/*check addresing modes*/
	if(is_register(operand1)==TRUE)
	{
		mode =REG_DIRECT;
	}	
	if(operand1[0]=='#')
	{	
		mode = IMMEDIATE;
	}
	if((is_register(operand1)==FALSE) && (operand1[0]!='#') )/*then it is a label*/
	{
		mode	= DIRECT;
	}
	/*check addressing mode for src operand, if it is "dummy" do not check it (it is either my intial dummy value or, a user that entered the string "dummy". in the former case it is irrelevant for the opcode and in the latter, if an operand is an option and was copied to operand1 in the parsing, it must be an opcode with an optional operand and therefore addressing mode 1 is legal for it - and dummy would be interpreted as addressing mode 1 because it is not a register and not immediate)*/
	if((strcmp(operand1,"dummy")!=0) && (is_legal_addressing_mode(legal_addresing_modes_src, opcode, mode)==FALSE))
	{
		error() 		
		fprintf(stderr,"Error: Illegal addressing mode for source operand \n");
		can_be_assembled=FALSE;
		return;
	}
	/*do the same things for the second operand*/
	if(is_register(operand2)==TRUE)
	{
		mode =REG_DIRECT;
	}	
	if(operand2[0]=='#')
	{	
		mode = IMMEDIATE;
	}
	if((is_register(operand2)==FALSE) && (operand2[0]!='#') )
	{
		mode	= DIRECT;
	}
	if((strcmp(operand2,"dummy")!=0) && (is_legal_addressing_mode(legal_addresing_modes_dest, opcode, mode)==FALSE))
	{
		error() 
		fprintf(stderr,"Error: Illegal addressing mode for destination operand \n");
		can_be_assembled=FALSE;
		return;
	}
	
	if(can_be_assembled == TRUE)
	{
		add_line(&line_list_h, &line_list_t, INSTRUCTION, is_symbol_first,jump_syntax, opcode, operand1, 		operand2,jump_op1,jump_op2);
		IC += n_machine_words(line_list_t);/*calculate how much machine words needed to code instruction and updated IC*/
	}
}




