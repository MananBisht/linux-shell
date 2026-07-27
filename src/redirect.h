#ifndef REDIRECT_H
#define REDIRECT_H

#include "parser.h"

int redirect_output(const Command *cmd);
int redirect_input(const Command *cmd);


#endif