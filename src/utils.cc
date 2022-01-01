/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2021 Yury Gribov
 * 
 * Use of this source code is governed by The MIT License (MIT)
 * that can be found in the LICENSE.txt file.
 */

#include "utils.h"
#include "error.h"

#include <stdlib.h>
#include <string.h>

#include <fstream>

std::string RealPath(const char *p) {
  char *tmp = realpath(p, 0);
  std::string Res(tmp);
  free(tmp);
  return Res;
}

bool StartsWith(const char *S, const char *Prefix) {
  return strncmp(S, Prefix, strlen(Prefix)) == 0;
}

void trim(std::string &S) {
  const char *ws = " \t";
  S.erase(0, S.find_first_not_of(ws));
  auto Last = S.find_last_not_of(ws);
  if (Last != std::string::npos)
    S.erase(Last + 1);
}

std::string readPatterns(const std::string &Filename, int Verbose) {
  std::string Line, Result;
  std::ifstream F(Filename);
  while (std::getline(F, Line)) {
    // Strip comments and whites
    if (auto Pos = Line.find_first_of('#'); Pos != std::string::npos)
      Line.erase(Pos);
    trim(Line);
    if (Line.empty())
      continue;
    // Append pattern
    if (!Result.empty())
      Result += '|';
    Result += '(';
    Result += Line;
    Result += ')';
  }
  if (F.bad()) {
    Error() << "failed to read file " << Filename << '\n';
    exit(1);
  }
  if (Verbose) {
    std::cerr << "Read patterns from file " << Filename << ": " << Result << '\n';
  }
  return Result;
}
