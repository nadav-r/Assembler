#include "main.h"
#include "data_structures.h"
#include "parsing.h"
#include "assembling.h"

int IC=0; /*instruction counter*/
int DC=0;/*data counter*/

Boolean can_be_assembled = TRUE;/*status flag*/

/*for symbols/labels declared in the source code*/
symbol_ptr symbol_list_h=NULL;/*head node for symbols list*/
symbol_ptr symbol_list_t=NULL;/*tail node for symbols list*/

/*for representing lines in the source code*/
line_ptr line_list_h=NULL;/*head node for line list*/
line_ptr line_list_t=NULL;/*tail node for line list*/

/*data list*/
data_ptr data_list_h=NULL;/*head node for data list*/
data_ptr data_list_t=NULL;/*tail node for data list*/

int main(int argc, char *argv[])
{
	int i=1;/*index for iterating over the files' names*/
	if(argc==1)
	{
		fprintf(stderr,"At least one file name must be entered\n");
		exit(EXIT_FAILURE);
	}
	/*assemble the files specified in the cmd line*/
	while(--argc>0)
	{	
		printf("Processing %s.as\n",argv[i]);
		/*parse the source file and save valuable data in the appropriate lists*/
		parse_src_file(argv[i]);
		if(can_be_assembled==TRUE)
		{	/*if everything is ok after parsing assemble the source file*/			
			assemble_src_file(argv[i]);
			if(can_be_assembled==TRUE)/*if the assembling routine succeeded inform the user*/
			{
				printf("File %s.as was assembled succesfully.\n\n",argv[i]);
			}
			else
			{
				printf("Failue: File %s.as could not be assembled.\n\n",argv[i]);
			}
		}
		else
		{
			printf("Failue: File %s.as could not be assembled.\n\n",argv[i]);
		}
		/*prepare for processing the next file*/
		i++;/*update files index*/ 	
		free_symbol_tab(&symbol_list_h);
		free_line_list(&line_list_h);
		free_data_list(&data_list_h);
		IC=0;
		DC=0;
		can_be_assembled=TRUE;
	}
	return 0;
}


