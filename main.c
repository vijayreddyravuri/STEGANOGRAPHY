/* R.VIJAY BHASKAR REDDY
  25008_045
  Project Title:- STEAGANOGRAPHY
  */
#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define BLUE    "\033[1;34m"
#define RESET   "\033[0m"

int main(int argc,char *argv[])
{
   EncodeInfo encodeInfo;
   DecodeInfo decodeInfo;
   
   // Check if the correct number of arguments are provided
   if(argc >= 3 && argc <= 5)
   {
      OperationType operation_type = check_operation_type(argv);
     // Check if operation is encoding
     if(operation_type == e_encode)
     {  
	// Validate file names and store in EncodeInfo structure
        if(read_and_validate_encode_args(argv,&encodeInfo) == e_success)
	{ 
	  if(do_encoding(&encodeInfo) ==e_success)
	  {
	    printf(GREEN "[SUCCESS]" RESET " Encoding completed successfully.\n");
	  }
	  else
	  {
	   printf(RED "\n[FAILURE]" RESET " Encoding is failed.\n");
	  }
	}
     }// Check if operation is decoding
     else if (operation_type== e_decode)
     { 
            // Validate file names and store in EncodeInfo structure
       if (read_and_validate_decode_args(argv, &decodeInfo) == e_success)
       {
          // Perform decoding
          if (do_decoding(&decodeInfo) == e_success)
          {
            printf(GREEN "[SUCCESS]" RESET " Decoding completed successfully.\n");
          }
          else
          {
            printf(RED  "\n[FAILURE]" RESET " Decoding failed.\n");
          }
        }
     }
   }
   else
   {
     printf("Sample command formats:\n");
     printf("./a.out -e <source_image.bmp> <secret_file> <output_image.bmp>\n");
     printf("./a.out -d <stego_image.bmp> <output_file>\n");
     return e_failure; 
   }
   return 0;
}

// Determine the operation: encode or decode
OperationType check_operation_type(char *argv[])
{
  if(strcmp(argv[1], "-e") == 0)
  {
    printf(GREEN "*** Steganography Encoding is Started ***\n" RESET);
    return e_encode;
  }
  else if(strcmp(argv[1], "-d") == 0)
  {
    printf(BLUE "*** Steganography Decoding is Started ***\n" RESET);
    return e_decode;
  }
  else
  {
    printf(RED "[ERROR] Unsupported operation type. Use -e for encoding or -d for decoding.\n" RESET);
    return e_unsupported; 
  }
}

// Validate and assign encode arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encodeInfo)
{
    int length1 = strlen(argv[2]);
    printf(" [INFO] Validating source image file: %s\n", argv[2]);

    // Validate source image file extension (.bmp)
    if (length1 >= 4 && strcmp(&argv[2][length1 - 4], ".bmp") == 0)
    {
        encodeInfo->src_image_fname = argv[2];
        printf(" [SUCCESS] Source image validation successful.\n");
         
        if(argv[3] != NULL)
	{
            int length2 = strlen(argv[3]);
            printf(" [INFO]    Validating secret file: %s\n", argv[3]);
   
            // Validate secret file extension (.txt, .c, .sh)
            if ((length2 >= 4 && strcmp(&argv[3][length2 - 4], ".txt") == 0) ||
               (length2 >= 2 && strcmp(&argv[3][length2 - 2], ".c") == 0)   ||
               (length2 >= 3 && strcmp(&argv[3][length2 - 3], ".sh") == 0))
            {
                encodeInfo->secret_fname = argv[3];
                printf(" [SUCCESS] Secret file validation successful.\n");
  
                // Set stego image file name (default: stego.bmp)
                 if (argv[4] != NULL)
                 {
   		   int length3=strlen(argv[4]);
  	 	   if (length3 >= 4 && strcmp(&argv[4][length3 - 4], ".bmp") == 0)
		   {
                     encodeInfo->stego_image_fname = argv[4];
                     printf(" [INFO]    Output stego image file set to: %s\n", encodeInfo->stego_image_fname);
		   }
		   else
                   {
                     printf(RED " [ERROR]   Output image must be a .bmp file.\n" RESET);
                     printf(RED "\n[FAILURE]" RESET " Encoding is failed.\n");
		     return e_failure;
                   }
                 }
		 else
		 {
		     encodeInfo->stego_image_fname = "Encoded_Image.bmp";
                     printf(" [INFO]    Using default output stego image file : %s\n", encodeInfo->stego_image_fname);
		 }
	     }
             else
             {
               printf(RED " [ERROR]   Secret file must have a .txt, .c, or .sh extension.\n" RESET);
               printf(RED "\n[FAILURE]" RESET " Encoding is failed.\n");
               return e_failure;
             }
	 }
	 else
	 {
	   printf(RED " [ERROR]   Secret file name is required.\n" RESET);
           printf(RED "\n[FAILURE]" RESET " Encoding is failed.\n");
	   return e_failure;
         }	 

          printf(" [SUCCESS] Command-line arguments validation completed.\n");
          return e_success;
     
  }
    else
    {
        printf(RED " [ERROR] Source image must be a .bmp file.\n" RESET);
        printf(RED "\n[FAILURE]" RESET " Encoding is failed.\n");
	return e_failure;
    }
}

// To perform encodiing
Status do_encoding(EncodeInfo *encodeInfo)
{
    if (open_files(encodeInfo) != e_success)
    {
        printf(RED "[ERROR] Failed to open source/stego/secret files\n" RESET);
        return e_failure;
    }

    if (check_capacity(encodeInfo) != e_success)
    {
        printf(RED "[ERROR] Insufficient image capacity\n" RESET);
        return e_failure;
    }

    if (copy_bmp_header(encodeInfo->fptr_src_image, encodeInfo->fptr_stego_image) != e_success)
    {
        printf(RED "[ERROR] Failed to copy BMP header\n" RESET);
        return e_failure;
    }

    if (encode_magic_string(MAGIC_STRING, encodeInfo) != e_success)
    {
        printf(RED "[ERROR] Failed to encode magic string\n" RESET);
        return e_failure;
    }

    if (encode_secret_file_extn_size(strlen(strrchr(encodeInfo->secret_fname, '.')), encodeInfo) != e_success)
    {
        printf(RED "[ERROR] Failed to encode secret file extension size\n" RESET);
        return e_failure;
    }

    if (encode_secret_file_extn(strrchr(encodeInfo->secret_fname, '.'), encodeInfo) != e_success)
    {
        printf(RED "[ERROR] Failed to encode secret file extension\n" RESET);
        return e_failure;
    }

    if (encode_secret_file_size(get_file_size(encodeInfo->fptr_secret), encodeInfo) != e_success)
    {
        printf(RED "[ERROR] Failed to encode secret file size\n" RESET);
        return e_failure;
    }

    if (encode_secret_file_data(encodeInfo) != e_success)
    {
        printf(RED "[ERROR] Failed to encode secret file data\n" RESET);
        return e_failure;
    }

    if (copy_remaining_img_data(encodeInfo->fptr_src_image, encodeInfo->fptr_stego_image) != e_success)
    {
        printf(RED "[ERROR] Failed to copy remaining image data\n" RESET);
        return e_failure;
    }

    return e_success;
}



// Check if image has enough capacity to hide the data
Status check_capacity(EncodeInfo *encodeInfo)
{
   printf("\n [INFO] Checking image capacity for encoding...\n");
   int total_size=0;
   total_size += strlen(MAGIC_STRING)*8;   //Magic String
   total_size += sizeof(int)*8;            //Length of the extension
   total_size += strlen(strrchr(encodeInfo->secret_fname,'.'))*8; //Extension
   total_size += sizeof(int)*8;            //Size of data
   total_size += get_file_size(encodeInfo->fptr_secret)*8; //Data

   printf(" [INFO] Required capacity (bits): %d\n", total_size);
   int image_capacity=get_image_size_for_bmp(encodeInfo->fptr_src_image);
   printf(" [INFO] Available image capacity (Height*Width*3)(bits): %d\n", image_capacity);
    
   if(image_capacity > total_size)
   {
     printf(" [SUCCESS] Sufficient image capacity. Proceeding with encoding.\n");
     return e_success;
   }
   printf(RED "[ERROR] Not enough capacity in image\n" RESET); 
   return e_failure;
}

//To get secret image file size
uint get_file_size(FILE *fptr)
{
  fseek(fptr, 0, SEEK_END);
  int size = ftell(fptr);
  rewind(fptr);
  return size;
}

// Copy 54-byte BMP header from source to destination
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    char buffer[54];
    rewind(fptr_src_image);  //rewind:- To set the file pointer at begining of file.

    printf("\n [INFO] Copying BMP header (54 bytes)...\n");
    // Read 54 bytes from source image
    if (fread(buffer, 1, 54, fptr_src_image) != 54)
    {
	printf(RED "[ERROR] Failed to read BMP header from source image.\n" RESET);
        return e_failure;  // Read failed
    }

    // Write 54 bytes to destination image
    if (fwrite(buffer, 1, 54, fptr_dest_image) != 54)
    {
        printf(RED "[ERROR] Failed to write BMP header from source image.\n" RESET);
        return e_failure;  // Write failed
    }

    printf(" [SUCCESS] BMP header copied successfully.\n");
    return e_success;
}

// Encoding the magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encodeInfo)
{
   printf("\n [INFO] Encoding magic string into image...\n");
   encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encodeInfo->fptr_src_image,encodeInfo->fptr_stego_image);
   printf(" [SUCCESS] Magic string encoded successfully.\n");
   return e_success;
}

// Encoding the secret file extension size
Status encode_secret_file_extn_size(int file_extn_size, EncodeInfo *encInfo)
{
  char image_buffer[32];
  printf("\n [INFO] Encoding secret file extension size: %d bytes\n", file_extn_size);
  fread(image_buffer, 32, 1, encInfo->fptr_src_image);      // Read 32 bytes from source image
  int_encode_byte_to_lsb(file_extn_size, image_buffer);     // Encode extension size into LSBs
  fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);   // Write 32 bytes to stego image
  printf(" [SUCCESS]   Secret file extension size encoded successfully.\n");
  return e_success; 
}

// Encoding the secret file extension 
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encodeInfo)
{
    printf("\n [INFO] Encoding secret file extension: %s\n", file_extn);
    encode_data_to_image(file_extn,strlen(file_extn),encodeInfo->fptr_src_image,encodeInfo->fptr_stego_image);
    printf(" [SUCCESS] Secret file extension encoded successfully.\n");
    return e_success;
}

// Encoding the secret file size
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
  char image_buffer[32];
  printf("\n [INFO] Encoding secret file size: %d\n", file_size);
  fread(image_buffer, 32, 1,encInfo->fptr_src_image);
  int_encode_byte_to_lsb(file_size, image_buffer);  
  fwrite(image_buffer,32,1, encInfo->fptr_stego_image);
  printf(" [SUCCESS] Secret file size encoded successfully.\n");
  return e_success;
}

// Encoding the secret file message
Status encode_secret_file_data(EncodeInfo *encodeInfo)
{
   int size = get_file_size(encodeInfo->fptr_secret);
   char buffer[size];
   printf("\n [INFO] Encoding secret file data (%d bytes)\n", size);
   rewind(encodeInfo->fptr_secret);
   fread(buffer,size,1, encodeInfo->fptr_secret);
   encode_data_to_image(buffer, size, encodeInfo->fptr_src_image, encodeInfo->fptr_stego_image);
   printf(" [SUCCESS] Secret file data encoded successfully.\n");
   return e_success;
}

// Copying the remaining data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
   char buffer;
   printf("\n [INFO] Copying remaining image data...\n");
   while (fread(&buffer, 1, 1, fptr_src) == 1)
   {
    fwrite(&buffer, 1, 1, fptr_dest);
   }
   printf(" [SUCCESS] Remaining image data copied successfully.\n");
   return e_success;
}

// Encoding data to image
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
  char image_buffer[8];
  for (int i = 0; i < size; i++)
  {
     fread(image_buffer, 8,1, fptr_src_image);                        // Read 8 bytes
     data_encode_byte_to_lsb(data[i], image_buffer);                  // Modify LSBs
     fwrite(image_buffer, 8,1, fptr_stego_image);                   // Write back
  }
  return e_success;
}

// Function to encode a single character's bits into the LSB of 8 bytes
Status data_encode_byte_to_lsb(char data, char *image_buffer)
{
  for(int i=0;i<=7;i++)
  {
    image_buffer[i]= image_buffer[i] & ~ 1; // Clear the LSB of each byte
    image_buffer[i]=image_buffer[i] | ( (data >> (7-i)) & 1 );  // Set each byte's LSB to corresponding bit of data (MSB → LSB)
  }
  return e_success;
}

// Function to encode an integer's bits into the LSB of 32 bytes
Status int_encode_byte_to_lsb(int data, char *image_buffer)
{
  for(int i=0;i<=31;i++)
  {
    image_buffer[i]= image_buffer[i] & ~ 1; // Clear the LSB of each byte
    image_buffer[i]=image_buffer[i] | ( (data >> (31-i)) & 1 ); // Set each byte's LSB to corresponding bit of data (MSB → LSB)
  }
  return e_success;
}

// Validate and assign decode arguments
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decodeInfo)
{
    int length = strlen(argv[2]);
    
    printf(" [INFO] Validating stego image file: %s\n", argv[2]);    
    // Check if source image file ends with .bmp
    if (length >= 4 && strcmp(&argv[2][length - 4], ".bmp") == 0)
    {
        // Store the source image file name
        decodeInfo->stego_image_fname = argv[2];
	printf(" [SUCCESS]Stego image validation successful.\n");
        // Store the secret file name
        if(argv[3] != NULL)
	{
		strcpy(decodeInfo->secret_image_fname, argv[3]);
		printf( " [INFO] Decoded Output file set to: %s\n", argv[3]);
	}
	else
	{
		strcpy(decodeInfo->secret_image_fname, "Decoded_file");
		printf(" [INFO] Decoded output file specified. Using default: %s\n ", decodeInfo->secret_image_fname);
	}
	printf("[SUCCESS] Decode arguments validaton completed.\n");
        return e_success; 
    }
    else
    {
        printf(RED " [ERROR] Source image must be a .bmp file\n" RESET);
	printf(RED "\n[FAILURE]" RESET " Decoding is failed.\n");
        return e_failure;
    }
}

//To perform decoding
Status do_decoding(DecodeInfo *decodeInfo)
{
    if (open_file(decodeInfo) != e_success)
    {
        printf(RED " [ERROR] Failed to open stego image file\n" RESET);
        return e_failure;
    }

    if (skip_bmp_header(decodeInfo->fptr_stego_image) != e_success)
    {
        printf(RED " [ERROR] Failed to skip BMP header\n" RESET);
        return e_failure;
    }

    if (decode_magic_string(decodeInfo) != e_success)
    {
        printf(RED " [ERROR] Magic string mismatch. Not a valid stego image\n" RESET);
        return e_failure;
    }

    if (decode_secret_file_extn_size(decodeInfo) != e_success)
    {
        printf(RED " [ERROR] Failed to decode secret file extension size\n" RESET);
        return e_failure;
    }

    if (decode_secret_file_extn(decodeInfo->extn_secret_file, decodeInfo->extn_size, decodeInfo) != e_success)
    {
        printf(RED " [ERROR] Failed to decode secret file extension\n" RESET);
        return e_failure;
    }

    if (decode_secret_file_size(decodeInfo) != e_success)
    {
        printf(RED " [ERROR] Failed to decode secret file size\n" RESET);
        return e_failure;
    }

    if (decode_secret_file_data(decodeInfo) != e_success)
    {
        printf(RED " [ERROR] Failed to decode secret file data\n" RESET);
        return e_failure;
    }

    return e_success;
}

//To open source file
Status open_file(DecodeInfo *decodeInfo)
{
    // Src Image file
    printf("\n Opening input image file: %s\n", decodeInfo->stego_image_fname);
    decodeInfo->fptr_stego_image = fopen(decodeInfo->stego_image_fname, "r");
    // Do Error handling
    if (decodeInfo->fptr_stego_image == NULL)
    {
        perror(" fopen");
        fprintf( stderr,  RED" [ERROR] Unable to open file:%s\n" RESET, decodeInfo->stego_image_fname);
        return e_failure;
    }
    printf(" [INFO] Input image file opened successfully.\n");
    return e_success;
}

// Function to skip bmp header upto 54 bytes
Status skip_bmp_header(FILE *fptr_stego_image)
{
  printf("\n [INFO] Skipping BMP header (54 bytes)...\n");
  fseek(fptr_stego_image,54,SEEK_SET);
  if(ftell(fptr_stego_image) == 54)
  {
    printf(" [SUCCESS] BMP header skipped successfully.\n");
    return e_success;
  }
  return e_failure;
}

// Function to perform decoding of magic string
Status decode_magic_string(DecodeInfo *decodeInfo)
{
  printf("\n [INFO] Decoding magic string from image...\n");

  char buffer[8];
  int len=strlen(MAGIC_STRING);
  char magic_string[len+ 1];
  char ch;
  for(int i=0;i<len;i++)
  {
   fread(buffer,8,1,decodeInfo->fptr_stego_image);
   decode_byte_to_lsb(&ch,buffer);
   magic_string[i]=ch;
  }
  magic_string[len]='\0';
  printf(" [INFO] Decoded magic string: %s\n", magic_string);

  if(strcmp(MAGIC_STRING,magic_string)==0)
  {
    printf(" [SUCCESS] Magic string validation successful.\n");
    return e_success;
  }
  return e_failure;
}

// Function to perform decoding file extn size
Status decode_secret_file_extn_size(DecodeInfo *decodeInfo)
{
  printf("\n [INFO] Decoding file extension size...\n");

  char buffer[32];
  int size;
  fread(buffer, 1, 32, decodeInfo->fptr_stego_image);
  decode_int_to_lsb(&size,buffer);

  decodeInfo->extn_size = size;
  printf(" [INFO] Secret file extension size decoded: %d bytes\n", size);
  
  return e_success;
}

// Function to perform decoding file extn 
Status decode_secret_file_extn(char *extn,int size, DecodeInfo *decodeInfo)
{
  printf("\n [INFO] Decoding secret file extension (%d bytes)...\n", size);

  char buffer[8];
  char ch;
  for(int i=0;i<size;i++)
  {
   fread(buffer,8,1,decodeInfo->fptr_stego_image);
   decode_byte_to_lsb(&ch,buffer);
   extn[i]=ch;
  }
  extn[size]='\0';
  printf(" [INFO] Secret file extension decoded: %s\n", extn);
  strcat(decodeInfo->secret_image_fname,extn);
  printf(" [INFO] Full decoded secret file name:%s\n",decodeInfo->secret_image_fname);
  return e_success;
}

// Function to perform decoding file size
Status decode_secret_file_size(DecodeInfo *decodeInfo)
{
  printf("\n [INFO] Decoding secret file size...\n");

  char buffer[32];
  int size;
  fread(buffer, 1, 32, decodeInfo->fptr_stego_image);
  decode_int_to_lsb(&size,buffer);
  decodeInfo->secret_file_size = size;
  printf(" [INFO] Secret file size decoded: %d bytes\n", size);
  return e_success;
}

// Function to perform decoding file data
Status decode_secret_file_data(DecodeInfo *decodeInfo)
{
 printf("\n [INFO] Decoding secret file data (%d bytes)...\n", decodeInfo->secret_file_size);
  FILE *fptr_secret_image = fopen(decodeInfo->secret_image_fname, "w");
    if (fptr_secret_image == NULL)
    {
        perror(" fopen");
        fprintf(stderr, RED"[ERROR] Unable to open file %s\n"RESET, decodeInfo->secret_image_fname);
        return e_failure;
    }

    char buffer[8];
    char ch;

    for (int i = 0; i < decodeInfo->secret_file_size; i++)
    {
        fread(buffer, 8, 1, decodeInfo->fptr_stego_image);
        decode_byte_to_lsb(&ch, buffer);
        fwrite(&ch, 1, 1, fptr_secret_image);
    }

    fclose(fptr_secret_image);
    printf(" [SUCCESS] Secret file decoded successfully.\n");
    return e_success;
}

// Decode 1 byte from 8 LSBs of the image buffer
char decode_byte_to_lsb(char *ch,char *buffer)
{
  *ch=0;
  for(int i=0;i<8;i++)
  {
     // Shift bits left and add current LSB bit from buffer
    *ch=(*ch << 1) |(buffer[i] & 1);
  }
}

// Decode 4 byte from 8 LSBs of the image buffer
int decode_int_to_lsb(int *size,char *buffer)
{
  *size = 0;  // Initialize the integer to 0
  for (int i = 0; i < 32; i++)
  {
    // Shift bits left and add current LSB bit from buffer
    *size = (*size << 1) | (buffer[i] & 1);
  }
}
