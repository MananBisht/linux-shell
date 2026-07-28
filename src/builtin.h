#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser.h"
#include <stdbool.h>

bool is_builtin(const Command *cmd);

int execute_builtin(const Command *cmd);

#endif