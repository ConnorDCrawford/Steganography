//
//  stego.h
//  Steganography
//
//  Created by Connor Crawford on 10/18/15.
//  Copyright © 2015 Connor Crawford. All rights reserved.
//

#ifndef stego_h
#define stego_h

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "WriteMsg.h"
#include "ReadMsg.h"

#define TRUE 1
#define FALSE 0
#define STD_COLOR_DEPTH 255
#define SXTN_COLOR_DEPTH 65535

int readHeader(FILE *fp, size_t * bytes, size_t * colors);

#endif /* stego_h */
