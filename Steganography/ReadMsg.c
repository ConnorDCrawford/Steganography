//
//  ReadMsg.c
//  Steganography
//
//  Created by Connor Crawford on 10/18/15.
//  Copyright © 2015 Connor Crawford. All rights reserved.
//

#include "ReadMsg.h"

unsigned int readHiddenByte(FILE *fp, size_t colors);

int readMessage(FILE *fp) {
    size_t colors;
    if (!readHeader(fp, NULL, &colors))
        return FALSE;
    
    // Read message length
    size_t length = readHiddenByte(fp, colors);
    if (length > STD_COLOR_DEPTH) {
        fprintf(stderr, "INVALID MESSAGE LENGTH.\n");
        return 1;
    }
    
    // Read message
    char message[length];
    int i;
    for (i = 0; i < length; i++)
        message[i] = readHiddenByte(fp, colors);
    printf("%.*s\n", (int)length, message);
    fclose(fp);
    return 0;
}

/*
 * Obtains the next hidden byte from the stream referenced by fp
 * returned as an unsigned char converted to an int. If the stream
 * is at end-of-file or a read error occurs, returns EOF
 */
unsigned int readHiddenByte(FILE *fp, size_t colors) {
    int size = sizeof(char) * 8;
    char c = 0;
    
    if (colors == STD_COLOR_DEPTH) {
        int i;
        for (i = 0; i < size; i++) {
            char orig = fgetc(fp);
            // Return EOF end of stream is reached
            if (feof(fp))
                return EOF;
            char bit = orig & 1; // Get LSB form original byte
            c <<= 1; // Left shift c by 1
            c += bit; // Add LSB to c, constructing the char
        }
    } else if (colors == SXTN_COLOR_DEPTH) {
        int i;
        for (i = 0; i < size/2; i++) {
            fgetc(fp); // Skip first byte
            if (feof(fp)) return EOF; // Return EOF end of stream is reached
            char orig = fgetc(fp);
            if (feof(fp)) return EOF;
            char bits = orig & 3; // Get two LSBs from original byte
            c <<= 2; // Left shift c by 2
            c += bits; // Add LSB to c, constructing the char
        }

    } else {
        fprintf(stderr, "INVALID COLOR DEPTH.\n");
        return FALSE;
    }
    
    return c;
}