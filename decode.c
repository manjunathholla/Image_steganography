#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
/*Function definitions*/

/*This function recieves cla and decInfo
  this is validate users input*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if( strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2];
        printf("File is of .bmp extension\n");
        return e_success;
    }

    else
    {
        printf("File is not of .bmp extension\n");
        return e_failure;
    }
}

/*do_decoding finction recieves two arguments
  this function is main decoding part*/

Status do_decoding(char *argv[],DecodeInfo *decInfo)
{
    //remaining all functions will be present
    if( open_files_out(decInfo) == e_success)
    {
        printf("File opening operation is Completed\n");
        if( skip_header(decInfo) == e_success)
        {
            printf("Header completed\n");
        }
        if( decode_magic_string(decInfo) == e_success)

        {
            printf("Magic  string is decoded successfully\n");
        }
        if( decode_secret_file_extn_size(decInfo) == e_success)
        {

            printf("Secret file extn size is decoded successfully\n");
        }
        if( decode_secret_file_extn(argv,decInfo) == e_success)
        {
            printf("Secret file extn is decoded Successfully\n");
        }

        if( decode_secret_file_size(decInfo) == e_success)
        {
            printf("Secret file size is decoded Successfully\n");
        }
        if( decode_secret_file_data(decInfo->size_secret_file,decInfo) == e_success)
        {
            printf("Secret file data is decoded Successfully\n");
        }

    }
    return e_success;
}


/* 
 * Inputs: Stego file
 * Output: FILE pointer for stego file
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_out(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }


    // No failure return e_success
    return e_success;
}

/*This function is used to skip the header part*/

Status skip_header(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    return e_success;
}

/*This function decodes the magic string and returns the status*/
Status decode_magic_string( DecodeInfo *decInfo)
{
    char magic_string[3],buffer[8];
    int i;
    for(i=0;i<2;i++)
    {
        fread(buffer,8,1,decInfo->fptr_stego_image);
        magic_string[i]=decode_byte_to_lsb(buffer);
    }
    magic_string[i]='\0';

    if ( strcmp(magic_string,"#*") == 0)
        return e_success;
    return e_failure;

}

/*This funtion is used to decode from byte to lsb */
char decode_byte_to_lsb(char *image_buffer)
{
    char ch=0;
    int i;
    for(i=0;i<8;i++)
    {
        ch = ((image_buffer[i] & 1)<<i) |  ch ;
    }
    return ch;
}

/*This function decodes the file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer,32, 1, decInfo->fptr_stego_image);
    decInfo->extn_size_secret_file = decode_size_to_lsb(buffer);
    return e_success;
}

/*This function decodes file extension
  And also checks for the output file*/
Status decode_secret_file_extn(char *argv[], DecodeInfo *decInfo)
{
    char buffer[8];
    int i;
    for(i=0;i<decInfo->extn_size_secret_file;i++)
    {

        fread(buffer,8, 1, decInfo->fptr_stego_image);
        decInfo->extn_secret_file[i] = decode_byte_to_lsb(buffer);
    }
    decInfo->extn_secret_file[i] = '\0';
    if(argv[3] != NULL)
    {

        if( strcmp(strstr(argv[3],"."), decInfo->extn_secret_file) == 0)
        {
            decInfo->secret_fname = argv[3];
            decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
            return e_success;
        }
        else
        {

            decInfo->secret_fname = strtok(argv[3],".");
            decInfo->secret_fname = strcat(decInfo->secret_fname,decInfo->extn_secret_file);
            decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");

            if (decInfo->fptr_secret == NULL)
            {
                perror("fopen");
                fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

                return e_failure;
            }

            return e_success;
        }
    }
    else
    {
        strcpy(decInfo->secret_fname,"secreto");
        decInfo->secret_fname = strcat(decInfo->secret_fname,decInfo->extn_secret_file);
        printf("%s\n",decInfo->secret_fname);
        decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");

        if (decInfo->fptr_secret == NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

            return e_failure;
        }

        return e_success;
    }
}

/*This function decodes file size */
Status decode_secret_file_size( DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego_image);
    decInfo->size_secret_file = decode_size_to_lsb(buffer);
    return e_success;
}

/*This function decodes secret data */
Status decode_secret_file_data(int size,  DecodeInfo *decInfo)
{
    char buffer[8];
    char data;
    for(int i=0;i<size;i++)
    {
        fread(buffer,8,1,decInfo->fptr_stego_image);
        data = decode_byte_to_lsb(buffer);
        fwrite(&data,1,1,decInfo->fptr_secret);
    }
    fclose(decInfo->fptr_secret);
    return e_success;
}

/*This function decodes 32 bytes */
int decode_size_to_lsb(char *image_buffer)
{
    int i;
    int ch=0;
    for(i=0;i<32;i++)
    {

        ch = ((image_buffer[i]&1)<<i) |  ch ;
    }
    
    return ch;
}

