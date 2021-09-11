/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2021 Yury Gribov
 * 
 * Use of this source code is governed by The MIT License (MIT)
 * that can be found in the LICENSE.txt file.
 */

#ifndef CLANG_UTILS_H
#define CLANG_UTILS_H

#include <clang-c/Index.h>

#include <string>
#include <vector>
#include <iostream>

std::string ToStr(CXString S);

struct Location {
  std::string Filename;
  unsigned Line, Col;
  Location(std::string Filename, unsigned Line, unsigned Col)
    : Filename(Filename), Line(Line), Col(Col) {}
};

Location ParseLoc(CXSourceLocation Loc, bool Expansion);

inline std::ostream &operator<<(std::ostream &OS, Location Loc) {
  OS << Loc.Filename << ':' << Loc.Line << ':' << Loc.Col;
  return OS;
}

struct PrintCursor {
  CXCursor C;
  PrintCursor(CXCursor C) : C(C) {}
};

inline std::ostream &operator<<(std::ostream &OS, PrintCursor P) {
  auto L = clang_getCursorLocation(P.C);
  auto Loc = ParseLoc(L, true);
  auto KindStr = ToStr(clang_getCursorKindSpelling(clang_getCursorKind(P.C)));
  OS << Loc << ' ' << KindStr;
  return OS;
}

std::vector<CXCursor> GetChildren(CXCursor Parent);

unsigned visitParentAndChildren(CXCursor Parent, CXCursorVisitor Visitor, CXClientData data);

#endif
