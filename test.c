/*NAME : MANJUNATHA HOLLA T S
DATE : 27/10/2023
DESCRITION : LEAST SIGNIFICANT BIT IMAGE STEGANOGRAPHY
SAMPLE INPUT : 
SAMPLE OUTPUT:
 */
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

/*main function where encoding or decoding is called.
 Here the users arguments are also checked*/
int main(int argc, char *argv[])
{

    if( check_operation_type(argv) == e_encode)
    {

        if(argc < 4)
        {
            printf("Error : Invalid arguments\nUsage : ./a.out -e beautiful.bmp secret.txt\n");
            return e_failure;
        }
        EncodeInfo encInfo;
        printf("Starting to perform encoding operation\n");
        if( read_and_validate_encode_args(argv, &encInfo) == e_success )
        {
            printf("Read and validation for encoding is successful\n");
            if( do_encoding(&encInfo) == e_success)
            {
                printf("Encoding is Completed\n");
                return e_success;
            }
        }
    }

    else if( check_operation_type(argv) == e_decode)
    {
        if(argc < 3)
        {
            printf("Error : Invalid arguments\nUsage : ./a.out -d stego.bmp \n");
            return e_failure;
        }
        DecodeInfo decInfo;
        printf("Starting to perform decoding operation\n");
        if( read_and_validate_decode_args(argv, &decInfo) == e_success )
        {
            printf("Read and validation for decoding is successful\n");
            if( do_decoding(argv,&decInfo) == e_success)
            {
                printf("Decoding is Completed\n");
                return e_success;
            }
        }
    }
    else if( check_operation_type(argv) == e_unsupported)
    {
        printf("Error : Invalid arguments\nPlease specify -e or -d\n");
        return e_failure;
    }
}                  


