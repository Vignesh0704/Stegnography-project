/*
Name: M S VIGNESH
Date:28/11/2024
Description: Steganography Project
*/
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

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
    printf("INFO: Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n", encInfo->src_image_fname);
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
    else{
        printf("INFO: Opened %s\n", encInfo->secret_fname);
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
    else{
        printf("INFO: Opened %s\n", encInfo->stego_image_fname);
    }
    printf("Done\n");
    // No failure return e_success
    return e_success;
}

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *bmp;
    if(bmp=strstr(argv[2], ".bmp"))
    {
        if(*(bmp+4)=='\0')
        {
            encInfo -> src_image_fname=argv[2];
            printf("%s is contain .bmp \n",encInfo->src_image_fname);
        }
        else
        {
            printf("The file is argv[2] not valid type not having.bmp\n");
            return e_failure;
        }
    }
    else
    {
        printf("The file is argv[2] not valid type not having.bmp\n");
        return e_failure;
    }
    char *dot_ptr=strstr(argv[3], ".");
    //Find the extension and length of extension
    if(dot_ptr!= NULL)
    {
        int ext_len=strlen(dot_ptr);
        encInfo->extn_size=ext_len;
        encInfo -> secret_fname=argv[3];
        strcpy(encInfo->extn_secret_file,dot_ptr);
        FILE *fp = fopen(argv[3], "r");
        fseek(fp, 0L, SEEK_END);
        long secret_file_size = ftell(fp);
        rewind(fp);
        fclose(fp);
        if (secret_file_size == 0)
        {
            printf("ERROR: Secret file is empty\n");
            return e_failure;
        }
        else
        {
            printf("The %s File having . extension\n",encInfo->secret_fname);
        }
    }
    else
    {
        printf("The file argv[3] is not valid extension type\n");
        return e_failure;
    }
    //check the output file and validate
    if(argv[4] != NULL)
    {
        char *bmp;
        if(bmp=strstr(argv[4], ".bmp"))
        {
            if(*(bmp+4)=='\0')
            {
                printf("Output file is present and have .bmp\n");
                encInfo -> stego_image_fname = argv[4];
            }
            else
            {
                printf("ERROR: The output file doesn't have .bmp extension\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: The output file doesn't have .bmp extension\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO: Output File not mentioned. Creating stego_img.bmp as default\n");
        encInfo -> stego_image_fname = "Stego_img.bmp";
        printf("%s", encInfo->stego_image_fname);
    }
    printf("Succesfully read and validate completed\n");
    return e_success;

}
char magic_string[30];

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf("## Encoding Procedure Started ##\n");
        printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
        /* check capacity */
       if(check_capacity(encInfo) == e_success)
       {
            printf("INFO: Done. FOUND OK\n");
            printf("INFO: Copying Image Header\n");
            /* Copy bmp image header */
            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
            printf("INFO: Done\n");
            printf("INFO: Enter the Magic String with max length 10: \n");
            scanf("%[^\n]",magic_string);
            printf("INFO: Entered Magic string given is %s\n",magic_string);
            printf("INFO: Entered Magic string length is %zu\n", strlen(magic_string));
            printf("INFO: Encoding Magic string signature length\n");
            /* Store Magic String Size */
            if(encode_magic_string_size(strlen(magic_string),encInfo) == e_success)
            {
                printf("INFO: Done\n");
                printf("INFO: Encoding Magic String signature\n");
                /* Store Magic String */
                if(encode_magic_string(magic_string,encInfo) == e_success)
                {
                    printf("INFO: Done\n");
                    printf("INFO: Encoding Secret.txt File Extension size\n");
                    /* Encode secret file extenstion size */
                    if(encode_extn_size(encInfo->extn_size, encInfo) == e_success)
                    {
                        printf("INFO: Done\n");
                        printf("INFO: Encoding Secret.txt File Extension\n");
                        /* Encode secret file extenstion */
                        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            printf("INFO: Done\n");
                            printf("INFO: Encoding Secret.txt File size\n");
                            /* Encode secret file size */
                            if(encode_secret_file_size(strlen(encInfo->secret_data), encInfo) == e_success)
                            {
                                printf("INFO: Done\n");
                                printf("INFO: Encoding Secret.txt File Data\n");
                                /* Encode secret file data*/
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("INFO: Done\n");
                                    printf("INFO: Copying Left Over Data\n");
                                    /* Copy remaining image bytes from src to stego image after encoding */
                                    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("INFO: Done\n");
                                        printf("## Encoding is Done Successfully\n"); 
                                    }
                                    else
                                    {
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    return e_failure;
                                }
                            }
                            else
                            {
                                return e_failure;
                            }
                        }
                        else
                        {
                            return e_failure;
                        }
                    }
                    else
                    {
                        return e_failure;
                    }
                }
                else
                {
                    return e_failure;
                }
            }
            else
            {
                return e_failure;
            }
        }
        else
        {
            return e_failure;
        }
    } 
    else
    {
        printf("Error: THe encoding things are so larger .So,We Can't Encode\n");
        return e_failure;
    } 
    }              
    else
    {
        printf("INFO: Opened %s\n", encInfo->src_image_fname);
    }
}
/* Store Magic String Size */
Status encode_magic_string_size(long magic_string_size, EncodeInfo *encInfo)
{
    char temp_buff[32];
    fread(temp_buff, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb(magic_string_size,temp_buff);
    fwrite(temp_buff, sizeof(char), 32, encInfo->fptr_stego_image);
    return e_success;
}
/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_string_to_image((char *)magic_string,strlen(magic_string),encInfo);
    return e_success;
}
/* Encode secret file extenstion size */
Status encode_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char temp_buff[32];
    printf("%lu",encInfo->extn_size);
    fread(temp_buff, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb(extn_size,temp_buff);
    fwrite(temp_buff, sizeof(char), 32, encInfo->fptr_stego_image); 
    return e_success;  
}
 /* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    encode_string_to_image((char *)encInfo->extn_secret_file,strlen(file_extn),encInfo);
    return e_success;
}
/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    int size;
    fseek(encInfo->fptr_secret, 0, SEEK_END);   
    size = ftell(encInfo->fptr_secret);    
    rewind(encInfo->fptr_secret);
    encInfo->size_secret_file=size;
    char str[32];
    fread(str, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(encInfo->size_secret_file, str);
    fwrite(str, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}
/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo) 
{
    int size=encInfo->size_secret_file;          
    char buffer[size];
    fread(buffer, 1, size, encInfo->fptr_secret);  
    encode_string_to_image(buffer, size, encInfo); 
    return e_success;   
}
//Check capacity that image capacity is ok to encode things
Status check_capacity(EncodeInfo *encInfo)
{
    uint Image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    uint Len_sec_msg = get_file_size(encInfo->fptr_secret);
    uint Encoding_things = 54 + ( 4 + strlen(magic_string) + 4 + strlen(encInfo->extn_secret_file) + 4 + Len_sec_msg) * 8;
    if(Image_capacity > Encoding_things)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
//get size of the File
uint get_file_size(FILE *fptr)
{
    uint file_size;
    fseek(fptr,0L,SEEK_END);
    file_size=ftell(fptr);
    fseek(fptr,0L,SEEK_SET);
    return file_size;
}
//Copy the bmp header
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0L, SEEK_SET);
    char header[54]; 
    fread(header, sizeof(char), 54, fptr_src_image);
    fwrite(header, sizeof(char), 54, fptr_dest_image);
    return e_success;
}
//Encode integer lsb bit
Status encode_size_to_lsb(long data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        uint get_bit = data & 1; 
        uint clear = image_buffer[i] & (~1); 
        image_buffer[i] = clear | get_bit;
        data >>= 1; 
    }
    return e_success;
}
//Encode string to image
Status encode_string_to_image(char *data, int size, EncodeInfo *encInfo)
{
    char temp_buff[8];

    for(int i=0;i<size;i++)
    {
        fread(temp_buff, sizeof(char), 8, encInfo->fptr_src_image);
        for(int j=0;j<8;j++)
        {
            uint get_bit = *data & 1; 
            uint clear = temp_buff[j] & (~1); 
            temp_buff[j] = clear | get_bit;
            *data >>= 1;
        }
        fwrite(temp_buff, sizeof(char), 8, encInfo->fptr_stego_image);
        data++;
    }
    return e_success;
}
/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	int ch;
    while ((ch = getc(fptr_src)) != EOF) 
    {
        putc(ch, fptr_dest);
    }
    return e_success;
}



