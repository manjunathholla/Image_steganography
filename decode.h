#ifndef DECODE_H
#define DECODE_H
#include<string.h>
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret fie
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    long extn_size_secret_file;
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    

} DecodeInfo;


/* Decoding function prototype */


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding( char *argv[],DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_out(DecodeInfo *decInfo);

/* Skip header */
Status skip_header(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string( DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_secret_file_extn_size( DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn( char *argv[], DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size( DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(int size,DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
char decode_byte_to_lsb(char *image_buffer);

/* Decode int into LSB of image data array */
int decode_size_to_lsb( char *image_buffer);


#endif
