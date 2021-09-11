#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

#include <iostream>

void SetProgName(const char *Prog);

std::ostream &Error();

std::ostream &Warning();

std::ostream &Note();

#endif
