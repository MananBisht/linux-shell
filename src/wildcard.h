#ifndef WILDCARD_H
#define WILDCARD_H

#include "parser.h"
#include <stdbool.h>

bool contains_wildcard(const char *str) ;

bool wildcard_match(const char *pattern, const char *filename) ;

int expand_wildcard( Command *cmd);

#endif