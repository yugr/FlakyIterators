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
  Ty = clang_getCanonicalType(Ty);  // Look through type aliases
  auto TypeName = ToStr(clang_getTypeSpelling(Ty));
//  std::cerr << "IsFlakyType: " << TypeName << '\n';
  if (StartsWith(TypeName.c_str(), "std::unordered_map<")
      || StartsWith(TypeName.c_str(), "std::unordered_set<"))
    return true;
  if (StartsWith(TypeName.c_str(), "std::map<")
      || StartsWith(TypeName.c_str(), "std::set<")) {
    // std::map with pointer keys is flaky as well
    int Nargs = clang_Type_getNumTemplateArguments(Ty);
    assert(Nargs > 0);
    auto KeyTy = clang_Type_getTemplateArgumentAsType(Ty, 0);
    if (KeyTy.kind == CXType_Pointer) {
      // But only if default comparator is used
      auto CompareTy = clang_Type_getTemplateArgumentAsType(Ty, Nargs - 2);
      auto CompareTyName = ToStr(clang_getTypeSpelling(CompareTy));
      return StartsWith(CompareTyName.c_str(), "std::less<");
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
      if (Ctx.isInFlakyLoop() && Ctx.isIOCall(Name)) {
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
