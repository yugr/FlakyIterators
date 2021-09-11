/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2021 Yury Gribov
 * 
 * Use of this source code is governed by The MIT License (MIT)
 * that can be found in the LICENSE.txt file.
 */

#include "utils.h"

#include <stdlib.h>
#include <string.h>

std::string RealPath(const char *p) {
  char *tmp = realpath(p, 0);
  std::string Res(tmp);
  free(tmp);
  return Res;
}

bool StartsWith(const char *S, const char *Prefix) {
  return strncmp(S, Prefix, strlen(Prefix)) == 0;
}
