//
//  readline.c
//  dspsdr
//
//  Created by Albin Stigö on 22/12/16.
//  Copyright © 2016 Albin Stigo. All rights reserved.
//

#include "command.h"

int readline(int ch, char *buffer, int len) {
    static int pos = 0;
    int rpos;
    
    if (ch > 0) {
        switch (ch) {
            case '\n':
                break;
            case '\r':
                rpos = pos;
                pos = 0;  // Reset position index ready for next time
                return rpos;
            default:
                if (pos < len-1) {
                    buffer[pos++] = ch;
                    buffer[pos] = 0;
                }
        }
    }
    // No end of line has been found, so return -1.
    return -1;
}

bool strcmp3(const char *str1, const char *str2) {
    if (str1[0] == str2[0] &&
        str1[1] == str2[1] &&
        str1[2] == str2[2]) {
        return true;
    }
    return false;
}
