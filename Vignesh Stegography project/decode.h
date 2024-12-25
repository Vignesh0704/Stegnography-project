/*
Name: M S VIGNESH
Date: 28/11/2024
Description: Steganography Project
*/
#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)

typedef struct _DecodeInfo
{
    /* Stego image Info */
    char *d_src_image_fname;
    FILE *fptr_d_src_image;

    char d_image_data[MAX_IMAGE_BUF_SIZE];
    long magic_string_size;
    char *magic_data;
    long extn_size;
    char *d_extn_secret_file;
    int size;
    char *d_secret_data;

    long int size_secret_file;
    FILE *fptr_d_dest_image;

    char d_secret_fname[30];
    FILE *fptr_d_secret;
} DecodeInfo;

/* Decoding function prototype */

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* decode magic string size*/
Status decode_magic_string_size(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(int size, DecodeInfo *decInfo);

/* Decode secret file extenstion length */
Status decode_secret_file_extn_len(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* Decode secret file data length*/
Status decode_secret_file_data_len(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(int size, DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_string_from_image(int size, FILE *fptr_stego_image, char *buffer);

/* Decode a byte ifrom LSB of image data array */ 
Status decode_size_from_lsb(char *buffer, long int *size);

#endif
