#include "printings.h"
#define print(a) printf(#a "\n");/*for debugging*/

#define error() fprintf(stderr,"In File %s in line %d ",cur_file, line_n);


/*print_principal_instruction(Principal_Instruction_Machine_Code * translation ,FILE *ob_fp)*/
/*printing the bit representation translation object (made by a translator) according to its inner bit fields structure.*/
/*param Principal_Instruction_Machine_Code * translation, pointer to object made by a translator. represents the principal instruction, i.e, a line that includes an opcode */
/*param FILE *ob_fp pointer to the object file */
void print_principal_instruction(Principal_Instruction_Machine_Code * instruction,FILE *ob_fp)
{
	unsigned int msk=1<<(PRINCIPAL_BIT_LOC-1);
	unsigned int machine_code=0;
	machine_code|=((instruction->are)<<ARE_LOC);
	machine_code|=((instruction->addressing_mode_dest)<<DEST_ADDRESING_MODE_LOC);
	machine_code|=((instruction->addressing_mode_src)<<SOURCE_ADDRESING_MODE_LOC);
	machine_code|=((instruction->opcode)<<OPCODE_LOC);
	machine_code|=((instruction->parm1)<<PARAM1_LOC);
	machine_code|=((instruction->parm2)<<PARAM2_LOC);
	fprintf(ob_fp,"%d \t", IC+STARTING_ADDRESS);
	while(msk)
	{
		if(msk&machine_code)
			fprintf(ob_fp,"/");
		else fprintf(ob_fp,".");
		msk>>=1;
	}
	fprintf(ob_fp,"\n");
}


/*print_register_param(Register_Param_Machine_Code * translation,FILE *ob_fp)*/
/*printing the bit representation translation object (made by a translator) according to its inner bit fields structure.*/
/*param Register_Param_Machine_Code * translation, pointer to object made by a translator, represents register parameters */
/*param FILE *ob_fp pointer to the object file */
void print_register_param(Register_Param_Machine_Code * instruction,FILE *ob_fp)
{
	unsigned int msk=1<<(BITS_PER_WORD-1);
	unsigned int machine_code=0;
	machine_code|=((instruction->are)<<ARE_LOC);
	machine_code|=((instruction->dest)<<DEST_REG_LOC);
	machine_code|=((instruction->src)<<SRC_REG_LOC);
	fprintf(ob_fp,"%d \t", IC+STARTING_ADDRESS);
	while(msk)
	{
		if(msk&machine_code)
			fprintf(ob_fp,"/");
		else fprintf(ob_fp,".");
		msk>>=1;
	}
	fprintf(ob_fp,"\n");
}

/*print_regular_translation(Regular_Param_Machine_Code * translation,FILE *ob_fp)*/
/*printing the bit representation of translation object (made by a translator) according to its inner bit fields structure.*/
/*param Regular_Param_Machine_Code * translation, pointer to object made by a translator, represents regular parameters, i.e., immediate parametes and locations of symbols */
/*param FILE *ob_fp pointer to the object file */
void print_regular_translation(Regular_Param_Machine_Code * instruction,FILE *ob_fp)
{
	unsigned int msk=1<<(BITS_PER_WORD-1);
	unsigned int machine_code=0;
	machine_code|=((instruction->are)<<ARE_LOC);
	machine_code|=((instruction->param)<<REGULAR_PARAM_LOC);
	fprintf(ob_fp,"%d \t", IC+STARTING_ADDRESS);
	while(msk)
	{
		if(msk&machine_code)
			fprintf(ob_fp,"/");
		else fprintf(ob_fp,".");
		msk>>=1;
	}
	fprintf(ob_fp,"\n");
}

/*print_data(data_ptr data,FILE *ob_fp)*/
/*printing the bit representation of a data from the data list.*/
/*param data_ptr data, pointer to data in the data list*/
/*param FILE *ob_fp pointer to the object file */
void print_data(data_ptr data,FILE *ob_fp)
{
	unsigned int msk=1<<(BITS_PER_WORD-1);
	unsigned int machine_code=data->data;
	fprintf(ob_fp,"%d \t", IC+STARTING_ADDRESS);
	while(msk)
	{
		if(msk&machine_code)
			fprintf(ob_fp,"/");
		else fprintf(ob_fp,".");
		msk>>=1;
	}
	fprintf(ob_fp,"\n");
	IC++;
}

