/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2021 Yury Gribov
 * 
 * Use of this source code is governed by The MIT License (MIT)
 * that can be found in the LICENSE.txt file.
 */

#include "analysis.h"

#include "clang-utils.h"

#include "error.h"
#include "utils.h"

#include <string.h>
#include <assert.h>

namespace {

bool IsFlakyType(CXType Ty) {
  // TODO: do I need clang_getCanonicalType ?
  auto TypeName = ToStr(clang_getTypeSpelling(Ty));
//  std::cerr << "IsFlakyType: " << TypeName << '\n';
  if (StartsWith(TypeName.c_str(), "std::unordered_map<"))
    return true;
  if (StartsWith(TypeName.c_str(), "std::map<")) {
    // std::map with pointer keys is flaky as well
    int Nargs = clang_Type_getNumTemplateArguments(Ty);
    assert(Nargs > 0);
    auto KeyTy = clang_Type_getTemplateArgumentAsType(Ty, 0);
    if (KeyTy.kind == CXType_Pointer)
      return true;
  }
  return false;
}

bool isIOCall(const std::string &Name) {
  // TODO: get list of relevant APIs from file
  // TODO: add std::cout/err and stream operators in general
  const char *Prefixes[] = {
    "print", "Print",
    "dump", "Dump",
  };
  for (size_t i = 0; i < sizeof(Prefixes)/sizeof(Prefixes[0]); ++i) {
    if (strstr(Name.c_str(), Prefixes[i])) {
      return true;
    }
  }
  return false;
}

} // anon namespace

enum CXChildVisitResult analyzeProgram(CXCursor C, CXCursor Parent, CXClientData Data) {
  (void)Parent;
  auto &Ctx = *static_cast<FlakyIteratorsContext *>(Data);

  auto L = clang_getCursorLocation(C);
  auto ExpLoc = ParseLoc(L, true);
//  auto SpellLoc = ParseLoc(RawLoc, false);

  // Skip system includes
  const char SysPrefix[] = "/usr/";
  if (StartsWith(ExpLoc.Filename.c_str(), SysPrefix))
    return CXChildVisit_Continue;

  if (Ctx.v() > 1) {
    Note() << "analyzeProgram: " << PrintCursor(C) << '\n';
  }

  switch (C.kind) {
  case CXCursor_CallExpr: {
      std::string Name = ToStr(clang_getCursorSpelling(C));
      if (Ctx.v())
        Note() << "calling function '" << Name << "'\n";
      if (Ctx.isInFlakyLoop() && isIOCall(Name)) {
        Warning() << "calling IO function '" << Name << "' inside flaky loop\n";
      }
      return CXChildVisit_Continue;
    }
  // TODO: CXCursor_ForStmt, CXCursor_WhileStmt
  case CXCursor_CXXForRangeStmt: {
      auto Children = GetChildren(C);
      if (Children.size() != 3) {
        Error() << "failed to parse loop at " << ExpLoc << '\n';
        break;
      }
      if (Ctx.v()) {
        Note() << "found CXXForRangeStmt:"
          << "\n  0: " << PrintCursor(Children[0])
          << "\n  1: " << PrintCursor(Children[1])
          << "\n  2: " << PrintCursor(Children[2])
          << '\n';
      }
      CXType Ty = clang_getCursorType(Children[1]);
      if (Ty.kind == CXType_Invalid) {
        Error() << "failed to analyze iteration object in loop at " << ExpLoc << '\n';
        break;
      }
      Ctx.push(IsFlakyType(Ty));  // Check if iteration order is flaky
      visitParentAndChildren(Children[2], analyzeProgram, (CXClientData)&Ctx);
      Ctx.pop();
      return CXChildVisit_Continue;
    }
  default:
    break;
  }

  return CXChildVisit_Recurse;
}
