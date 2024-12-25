/*
Name: M S VIGNESH
Date: 28/11/2024
Description: Steganography Project
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "decode.h"

/* Function Definitions */
/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files\n");
    // Stego Image file
    decInfo->fptr_d_src_image = fopen(decInfo->d_src_image_fname, "r");
    // Error handling
    if (decInfo->fptr_d_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->d_src_image_fname);
        return d_failure;
    }
    printf("INFO: Opened %s\n",decInfo->d_src_image_fname);
    
    return d_success;
}
//Validate the arguments 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] == NULL)
    {
        printf("Not enough arguments for decoding\n");
        return d_failure;
    }
    
    if (strstr(argv[2], ".bmp") != NULL)
    {
        char *bmp;
        if(bmp=strstr(argv[2], ".bmp"))
        {
        if(*(bmp+4)=='\0')
        {
            decInfo->d_src_image_fname = argv[2];
            printf("argv[2] has .bmp\n");
        }
        else
        {
            printf("argv[2] is not a valid image file\n");
            return d_failure;
        }
        }
    }
    else
    {
        printf("argv[2] is not a valid image file\n");
        return d_failure;
    }
    if (argv[3] != NULL)
    {
        printf("Using specified output file name\n");
        strcpy(decInfo->d_secret_fname, argv[3]);
    }
    else
    {
        printf("Using default output file name\n");
        strcpy(decInfo->d_secret_fname, "decode");
    }
    printf("Read and validation done successfully\n");
    return d_success;
}
/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("## Decoding Procedure started ##\n");
    //Open the output file 
    if (open_files_decode(decInfo) == d_success)
    {
        //skip the 54 bytes which is BMP header
        fseek(decInfo->fptr_d_src_image, 54, SEEK_SET);
        //Decode the Magic string size
        if (decode_magic_string_size(decInfo) == d_success)
        {
            printf("INFO: Decoding Magic string signature\n");
            //Decode the Magic string
            if(decode_magic_string(decInfo->magic_string_size,decInfo) == d_success)
            {
                printf("INFO: Done\n");
                //Decode Secret FIle extension length
                if(decode_secret_file_extn_len(decInfo) == d_success)
                {
                    printf("INFO: Decoding Output File Extension\n");
                    //Decode Secret File Extension
                    if(decode_secret_file_extn(decInfo->extn_size, decInfo) == d_success)
                    {
                        printf("INFO: Done\n");
                        printf("INFO: Decoding %s File Size\n",decInfo->d_secret_fname);
                        //Open the Secret File
                        decInfo->fptr_d_secret = fopen(decInfo->d_secret_fname, "w");
                        if (decInfo->fptr_d_secret == NULL)
                        {
                            perror("fopen");
                            fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->d_secret_fname);
                            return d_failure;
                        }
                        printf("INFO: Opened %s\n",decInfo->d_secret_fname);
                        // If no failure, return d_success
                        printf("Files opened successfully\n");
                        //Decode Secret File data len
                        if(decode_secret_file_data_len(decInfo) == d_success)
                        {
                            printf("INFO: Done\n");
                            printf("INFO: Decoding %s File Data\n",decInfo->d_secret_fname);
                            //Decode Secret data
                            if(decode_secret_file_data(decInfo->size_secret_file,decInfo) == d_success)
                            {
                                
                                printf("INFO: Done\n");
                                printf("## Decoding Done Successfully ##\n");
                            }
                            else
                            {
                                return d_failure;
                            }
                        }
                        else
                        {
                            return d_failure;
                        }
                    }
                    else
                    {
                        return d_failure;
                    }
                }
                else
                {
                    return d_failure;
                }  
            }
            else
            {
                return d_failure;
            }
        }
        else
        {
            printf("Error: The given argument Output File is not the Encoded Output File\n");
            return d_failure;
        }
    }
    else
    {
        return d_failure;
    }
    
}
/* decode magic string size*/
Status decode_magic_string_size(DecodeInfo *decInfo)
{
   printf("INFO: Decoding magic string Length\n");
   unsigned char buffer[32];
   decInfo->magic_string_size=0;
   fread(buffer, sizeof(char), 32, decInfo->fptr_d_src_image);
   decode_size_from_lsb(buffer,&decInfo->magic_string_size);
   
   if(decInfo->magic_string_size > 30)
   {
      return d_failure;
   }
   return d_success;
}
char Magic_str[30];
/* Decode and Store Magic String */
Status decode_magic_string(int size, DecodeInfo *decInfo)
{
    char str[size+1];
    char buffer[size];
    decode_string_from_image(size, decInfo->fptr_d_src_image, buffer);
    strcpy(str, buffer);
    str[size]= '\0';

    // Get the password from the user
    printf("Req : Enter the password : ");
    scanf(" %[^\n]", Magic_str);
    //compare the decoded and user Entered Passwords
    if(strcmp(str, Magic_str) == 0)
    {
        printf("INFO: User entered magic string and decoded magic string are same\n");
        return d_success;
    }
    else
    {
        printf("Entered Password is incorrect\n");
        return d_failure;
    }
}
/* Decode secret file extenstion length */
Status decode_secret_file_extn_len(DecodeInfo *decInfo)
{
    char str[32];
    long int length;
    fread(str, sizeof(char),32, decInfo->fptr_d_src_image);
    decode_size_from_lsb(str, &length);
    decInfo->extn_size = length;
    return d_success;
}
/* Decode secret file extenstion */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
    char str[size+1];
    char buffer[size];
    decode_string_from_image(size, decInfo->fptr_d_src_image, buffer);
    strcpy(str, buffer);
    str[size] = '\0'; 
    strcat(decInfo->d_secret_fname,str);
    return d_success;
}
/* Decode secret file data length*/
Status decode_secret_file_data_len(DecodeInfo *decInfo)
{
    char image_buffer[32];
    fread(image_buffer,sizeof(char),32,decInfo->fptr_d_src_image);
    decode_size_from_lsb(image_buffer,&decInfo->size_secret_file);
    return d_success;
}

/* Decode secret file data*/
Status decode_secret_file_data(int size, DecodeInfo *decInfo)
{
    char sec_data[size+1];
    char buffer[size];
    decode_string_from_image(size, decInfo->fptr_d_src_image, buffer);
    strcpy(sec_data , buffer);
    sec_data [size] = '\0'; 
    fwrite(sec_data,1,size,decInfo->fptr_d_secret);
    printf("The secret data is %s\n",sec_data );
    return d_success;
}
/* Decode a byte ifrom LSB of image data array */ 
Status decode_size_from_lsb(char *buffer, long int *size)
{
    *size = 0;
    for (int i = 0; i < 32; i++)
    {
        *size |= (buffer[i] & 1) << i;
    }
    return d_success;
}
/* Decode function, which does the real decoding */
Status decode_string_from_image(int size, FILE *fptr_stego_image, char *buffer)
{
    char image_buf[8];
    int i;
    for(i=0;i< size;i++)
    {
        fread(image_buf,sizeof(char),8,fptr_stego_image);
        buffer[i] = 0;

        for(int j=0;j<8;j++)
        {
            int getbit=(image_buf[j] & 1);
            buffer[i] |= (getbit << j);
        }
    }
    return d_success;
}


