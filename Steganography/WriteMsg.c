//
//  WriteMsg.c
//  Steganography
//
//  Created by Connor Crawford on 10/18/15.
//  Copyright © 2015 Connor Crawford. All rights reserved.
//

#include "WriteMsg.h"

void copyFileContents(FILE *in, FILE *out, size_t bytes);
void write_hidden_byte(char c, FILE *fp, size_t colors);

int writeMessage(const char message[], FILE *in, const char filename[]){
    FILE *out;
    size_t maxLen, colors, size = (int)strlen(message);
    
    // Open output file or create one if it does not exist already
    if ((out = fopen(filename, "wb+")) == NULL) {
        fclose(in); fclose(out);
        return FALSE;
    }
    // Read header of input file, close files if unsuccessful
    if (!readHeader(in, &maxLen, &colors)) {
        fclose(in); fclose(out);
        return FALSE;
    }
    
    // Copy contents of input file into output file
    copyFileContents(in, out, maxLen);
    
    // Determine if message is an acceptable length
    maxLen /= 8; // Since each char takes 8 bytes to store, the max length of message is (# bytes / 8)
    maxLen = maxLen > STD_COLOR_DEPTH ? STD_COLOR_DEPTH : maxLen; // Set maxLen to itself or STD_COLOR_DEPTH, whichever is lower
    if (size > maxLen) {
        fprintf(stderr, "MESSAGE TOO LONG. MAX SIZE: %zu CHARACTERS. MESSAGE LENGTH: %zu.\n", maxLen, size);
        fclose(in); fclose(out);
        return FALSE;
    }
    
    // Write message
    readHeader(out, NULL, NULL); // Go to start of pixel data
    write_hidden_byte(size, out, colors); // Write the size in first byte
    int i;
    for (i = 0; i < size; i++) // Write each char in message in image data
        write_hidden_byte(message[i], out, colors);
    fclose(in); fclose(out);
    return TRUE;
}

void copyFileContents(FILE *in, FILE *out, size_t bytes) {
    char c;
    if (in == NULL || out == NULL) {
        fprintf(stderr, "COPY: IN AND OUT CANNOT BE NULL.\n");
        return;
    }
    rewind(in);
    
    // Reopen the output file to clear contents
    if ((out = freopen(NULL, "wb+", out)) == NULL) {
        fclose(out);
        return;
    }
    
    // Copy file header
    while ((c = fgetc(in)) != EOF)
        fputc(c, out);
    // Copy pixel data
    int i;
    for (i = 0; i < bytes; i++, c = fgetc(in))
        fputc(c, out);
    rewind(out);
}

void write_hidden_byte(char c, FILE *fp, size_t colors) {
    int size = sizeof(char) * 8;
    if (colors == STD_COLOR_DEPTH) {
        int i;
        for (i = size - 1; i >= 0; i--) { // Insert bits of c from left to right so it is human readable
            unsigned char mask = -2; // Mask all bits other than LSB 0x11111110
            char bit = ((c & (1 << i)) >> i);
            char orig = fgetc(fp); // Get original byte
            if (feof(fp)) return; // Return if we reach end of stream
            fseek(fp, -1, SEEK_CUR); // Go back to position before original
            fputc((orig & mask) + bit, fp); // Replace original with modified byte
        }
    } else if (colors == SXTN_COLOR_DEPTH) {
        int i;
        for (i = size - 1; i >= 0; i--) { // Insert bits of c from left to right so it is human readable
            unsigned char mask = -4; // Mask all bits other than 2 LSBs 0x11111100
            char bits = ((c & (1 << i)) >> (i - 1)); // Put ith bit of c in the 2nd LSB
            i--; // Go to next bit in c
            bits += ((c & (1 << i)) >> i); // Put ith bit of c in LSB
        
            fgetc(fp); // Skip the first byte of color value
            if (feof(fp)) {
                fprintf(stderr, "REACHED EOF BEFORE MESSAGE COMPLETE.\n");
                return; // Return if we reach end of stream
            }
            char orig = fgetc(fp); // Get original byte
            if (feof(fp)) {
                fprintf(stderr, "REACHED EOF BEFORE MESSAGE COMPLETE.\n");
                return;
            }
            fseek(fp, -1, SEEK_CUR); // Go back to position before original
            fputc((orig & mask) + bits, fp); // Replace original with modified byte
        }
    } else {
        fprintf(stderr, "INVALID COLOR DEPTH.\n");
        return;
    }
}

