/*
Name: M S VIGNESH
Date:28/11/2024
Description: Steganography Project
*/
#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    //Error Handling Based on the Argument count
    if(argc<2)
    {
         return 1;
    }
    
    if (((strcmp(argv[1], "-e")) == 0) && argc < 4) 
    {
        return 1;
    }

    if (((strcmp(argv[1], "-d")) == 0) && argc < 3) 
    {
        return 1;
    }
    //Checking operation type - encoding or decoding
    OperationType opt=check_operation_type(argv);
    //Based on the Operation type do Operation
    if(opt == e_encode)
    {
        if((read_and_validate_encode_args(argv,&encInfo)) == e_success)
        {
            do_encoding(&encInfo);
        }
        else
        {
            printf("Error: In Encoding Read and validate Failed\n");
        }
    }
    else if(opt == e_decode)
    {
        if((read_and_validate_decode_args(argv,&decInfo)) == d_success)
        {
            do_decoding(&decInfo);
        }
        else
        {
            printf("Error: In decoding Read and validate Failed\n");   
        }
    }
    else if(opt == e_unsupported)
    {
        printf("Eroor: The Entered argument is not a Encode or Decode option\n");
    }

    return 0;
}

/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    //IF : checking argv[1] is "-e"->return e_encode
    if((strcmp(argv[1], "-e")) == 0)
    {
        printf("Entered Encoding option\n");
        return e_encode;
    }
    //ELSE IF : checking argv[1] is "-d"->return e_decode 
    else if((strcmp(argv[1], "-d")) == 0)
    {
        printf("Entered Decoding option\n");
        return e_decode;
    }
    //ELSE : return e_unsupported
    else
    {
        printf("Entered unsupported Type\n");
        return e_unsupported;
    }
}
