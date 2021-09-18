#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

#include <iostream>

void SetProgName(const char *Prog);

// TODO: automatically append '\n' and call exit(1) in Error()
std::ostream &Error();

std::ostream &Warning();

std::ostream &Note();

#endif
