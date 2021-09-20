/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2021 Yury Gribov
 * 
 * Use of this source code is governed by The MIT License (MIT)
 * that can be found in the LICENSE.txt file.
 */

#include "clang-utils.h"

#include "utils.h"

#include <stdlib.h>
#include <string.h>

std::string ToStr(CXString S) {
  const char *CS = clang_getCString(S);
  std::string Str(CS ? CS : "");
  clang_disposeString(S);
  return Str;
}

Location ParseLoc(CXSourceLocation Loc, bool Expansion) {
  CXFile F;
  unsigned Line, Col;
  (Expansion ? clang_getExpansionLocation : clang_getSpellingLocation)(Loc, &F, &Line, &Col, 0);
  auto FileName = ToStr(clang_getFileName(F));
  // Code in _Pragma or cmdline defs has empty filenames
//  FileName = FileName.empty() ? FileName : RealPath(FileName.c_str());
  return Location(FileName, Line, Col);

}

namespace {

struct GetChildrenContext {
  std::vector<CXCursor> Children;
};

enum CXChildVisitResult GetChildrenCallback(CXCursor C, CXCursor Parent, CXClientData Data) {
  (void)Parent;
  auto &Ctx = *static_cast<GetChildrenContext *>(Data);
  Ctx.Children.push_back(C);
  return CXChildVisit_Continue;
}

} // anon namespace

std::vector<CXCursor> GetChildren(CXCursor Parent) {
  GetChildrenContext Ctx;
  clang_visitChildren(Parent, GetChildrenCallback, &Ctx);
  return Ctx.Children;
}

unsigned visitParentAndChildren(CXCursor Parent, CXCursorVisitor Visitor, CXClientData Data) {
  Visitor(Parent, Parent, Data);
  return clang_visitChildren(Parent, Visitor, Data);
}
