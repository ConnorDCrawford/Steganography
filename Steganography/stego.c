//
//  stego.c
//  Steganography
//
//  Created by Connor Crawford on 10/18/15.
//  Copyright © 2015 Connor Crawford. All rights reserved.
//
#include "stego.h"

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        fprintf(stderr, "%s: Expected arguement.\n", argv[0]);
    } else if (argc == 2) {
        FILE *fp;
        if ((fp = fopen(argv[1], "r")) == NULL) {
            printf("UNABLE TO FIND FILE.\n");
            return 1;
        }
        // One arguement, so only reading file
        readMessage(fp);
    } else if (argc == 4) {
        FILE *fp;
        if ((fp = fopen(argv[2], "r")) == NULL) {
            printf("UNABLE TO FIND FILE.\n");
            return 1;
        }
        // Three arguements, so writing file
        writeMessage(argv[1], fp, argv[3]);
    } else
        fprintf(stderr, "INVALID NUMBER OF ARGUEMENTS.\n");
    return 0;
}

int readHeader(FILE *fp, size_t * bytes, size_t * colors) {
    unsigned int x = 0, y = 0;
    size_t color = 0;
    char current;
    
    // Check to make sure fp isn't null
    if (fp == NULL) {
        fprintf(stderr, "ATTEMPTING TO READ UNOPENED FILE.\n");
        return FALSE;
    }
    rewind(fp);
    
    // Check if first to chars are 'P' & '6'
    if (fgetc(fp) != 'P' || fgetc(fp) != '6') {
        fprintf(stderr, "NOT A VALID PPM FILE.\n");
        return FALSE;
    }
    while (fgetc(fp) != '\n');
    // Check for comments
    current = fgetc(fp);
    if (current == '#') {
        // Skip any comments
        while (current != '\n')
            current = fgetc(fp);
    }
    // Put current back
    ungetc(current, fp);
    // Read in two ints for x & y dimensions
    if (fscanf(fp, "%d %d", &x, &y) != 2) {
        fprintf(stderr, "COULD NOT READ DIMENSIONS.\n");
        return FALSE;
    }

    // Check for invalid dimensions (x or y = 0)
    if (x < 1 || y < 1) {
        fprintf(stderr, "INVALID DIMENSIONS. MUST BE AT LEAST 1X1 PIXELS.\n");
        return FALSE;
    }
    // Read in an int for color depth
    if (fscanf(fp, "%zd", &color) != 1) {
        fprintf(stderr, "COULD NOT READ COLOR DEPTH.\n");
        return FALSE;
    }
    
    // Check for valid color depth
    if (color != STD_COLOR_DEPTH && color != SXTN_COLOR_DEPTH) {
        fprintf(stderr, "COLOR DEPTH MUST BE 8-BIT OR 16-BIT.\n");
        return FALSE;
    }
    if (colors != NULL)
        *colors = color;
    
    // Skip everything up until data
    while (fgetc(fp) != '\n');
    
    if (bytes != NULL)
        // Determine number of bytes based on color depth
        // Total # bytes = 3 (for R, G & B) * row * col or 6 (for 2 X R, G & B) * row * col
        *bytes = *colors == STD_COLOR_DEPTH ? 3 * y * x : 6 * y * x;
    return TRUE;
}