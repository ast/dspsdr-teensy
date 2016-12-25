//
//  readline.h
//  dspsdr
//
//  Created by Albin Stigö on 22/12/16.
//  Copyright © 2016 Albin Stigo. All rights reserved.
//

#ifndef readline_h
#define readline_h

#include <stdio.h>

typedef void (*command_handler_t)(const char *arg, int len);

typedef struct
{
    char cmd[4];
    command_handler_t fun;
} command_t;

int readline(int ch, char *buffer, int len);
bool strcmp3(const char *str1, const char *str2);

#endif /* readline_h */
