#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* To check operation type */
OperationType check_operation_type(char *argv[])
{
    if( strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if( strcmp(argv[1], "-d") == 0)
        return e_decode;
    return e_unsupported;
}

/*This function recieves arguments and validates it */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if( strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
        printf("Source file is of .bmp extension\n");
        if( strcmp(strstr(argv[3], "."), ".txt") == 0 || strcmp(strstr(argv[3], "."), ".c") == 0 || strcmp(strstr(argv[3], "."), ".sh") == 0)
        {
            encInfo->secret_fname =argv[3];
            printf("Secret file  is of .txt or .c or .sh extension\n");
            if( argv[4] !=NULL)
            {
                if( strcmp(strstr(argv[4], "."), ".bmp") == 0)
                {
                    encInfo->stego_image_fname =argv[4];
                    return e_success;
                }
                else
                {
                    printf("Output file is not of .bmp extension\n");
                    return e_failure;
                }
            }
            else
            {
                encInfo->stego_image_fname = "default.bmp";
                return e_success;
            }
        }
        else
        {
            printf("Secret file is not of .txt or .c or .sh extension\n");
            return e_failure;
        }
    }
    else
    {
        printf("Source file is not of .bmp extension\n");
        return e_failure;
    }
}

/*This is the actual encoding.
  From here all the other functions are called.
  */
Status do_encoding(EncodeInfo *encInfo)
{
    //remaining all functions will be present
    if( open_files(encInfo) == e_success)
    {
        printf("Opening the files operation is successful\n");
        if( check_capacity(encInfo) == e_success)
        {
            printf("Capacity checking of source image is Completed\n");
           if(  copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
           {
               printf("Copying of header is Successful\n");
               if( encode_magic_string(MAGIC_STRING,encInfo) == e_success)

               {
                   printf("Magic  string encoded Successfully\n");
               }


              if( encode_secret_file_extn_size(strlen(strstr(encInfo->secret_fname,".")),encInfo) == e_success)
              {
                  printf("Secret file extn size is encoded successfully\n");
              }
             if( encode_secret_file_extn(strstr(encInfo->secret_fname,"."),encInfo) == e_success)
              {
                  printf("Secret file extn is encoded successfully\n");
              }

              if( encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
              {
                  printf("Secret file size is encoded successfully\n");
              }
            if( encode_secret_file_data(encInfo) == e_success)
              {
                  printf("Secret file data encoded successfully\n");
              }
           if( copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
            {
                printf("Copied remaining data Successfully\n");
            }
                  
           }

        }

    }
    return e_success;
}

/*This function checks the capacity of the source file.
  To ensure encoding can be done on that file.
  */
Status check_capacity(EncodeInfo *encInfo)
{
    int num;
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    num=54+((2+4+4+4+encInfo->size_secret_file)*8);
    if( encInfo->image_capacity > num)
    {
        printf("Image has the capacity to be encoded\n");
        return e_success;
    }
    printf("Sorry image doesn't have the capacity to be encoded\n");
    return e_failure;
}

/*This function returns the secret file size*/
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*This function encodes the magic string. */
Status encode_magic_string( char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string,strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
   // printf("Magic string is encoded\n");
        return e_success;
}


Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    int i;
    for(i=0;i<size;i++)
    {
        fread(buffer,8,1,fptr_src_image);
        encode_byte_to_lsb(data[i],buffer);
        fwrite(buffer,8,1,fptr_stego_image);
    }
    return e_success;
}

/*This function encodes 1 byte of data in lsb */ 
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i;
    for(i=0;i<8;i++)
    {
        image_buffer[i]=(image_buffer[i]&0xfe)  | ((data&(1<<i)) >> i);
    }
    return e_success;
}

/*This function encodes secret file extension size */
Status encode_secret_file_extn_size( int file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    
    fread(buffer,32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer,32,1, encInfo->fptr_stego_image);
    return e_success;
}


/*This function encodes the secret file extension */
Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo)
{

    encode_data_to_image(file_extn,strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

/*This function encodes 32 bytes of data in lsb */
Status encode_size_to_lsb(int data, char *image_buffer)
{
    int i;
    for(i=0;i<32;i++)
    {
        image_buffer[i]=(image_buffer[i]&0xfe)  | ((data&(1<<i)) >> i);
    }
   // printf("Encoding from byte to lsb successful\n");
    return e_success;
}

/*This function is used to copy the header of the source image
  And paste in stego image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    return e_success;
}

/*This function encodes secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    
    fread(buffer,32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer,32,1, encInfo->fptr_stego_image);
    return e_success;
}

/*This function is used to  actually encode the secret data */ 
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file];

    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
    encode_data_to_image(buffer,encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

/*This function copies the of remaining data of source img 
  which is left after encoding and paste to stego img. */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[1];
    while(fread(buffer,1,1,fptr_src))
    {
    fwrite(buffer,1,1,fptr_dest);
    }
    return e_success;
}
