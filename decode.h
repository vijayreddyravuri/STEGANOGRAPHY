#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"   //Contains e_success, e_failure, and Status enum
	
/* structure to store information required for decoding */

typedef struct _DecodeInfo
{

	//Stego Image info
	char *stego_image_fname;
	FILE *fptr_stego_image;


	//Output File Info
	char  secret_image_fname[20];
	FILE *fptr_secret_image;

	//Secret File Extension
	char extn_secret_file[10];
	int extn_size;

	//Secret size
	int secret_file_size;

}DecodeInfo;

/* Checking decoding operation type */
OperationType check_operation_type(char *argv[]);

/* Read and Validate the Decode Arguments  */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decodeInfo);

/* performing Decoding */
Status do_decoding(DecodeInfo *decodeInfo);

/* decode MAGIC STRING */
Status decode_magic_string(DecodeInfo *decode);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decodeInfo);

/*Decode secret file extension */
Status decode_secret_file_extn(char *extn, int size, DecodeInfo *decodeInfo);

/*Decode Secret file size */
Status decode_secret_file_size(DecodeInfo *decodeInfo);

/*Decode Secret File data*/
Status decode_secret_file_data(DecodeInfo *decodeInfo);

/*Decode byte from LSBs of image data */
char decode_byte_to_lsb(char *ch, char *image_buffer);

/* Decode an intger from LSBs of image data */
int decode_int_to_lsb(int *value, char *image_buffer);

/* Skip BMP header(54 bytes) */
Status skip_bmp_header(FILE *fptr_stego_image);

/* open file for decoding */
Status open_file(DecodeInfo *decodeInfo);

#endif //DECODE_H

