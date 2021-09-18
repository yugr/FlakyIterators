/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2021 Yury Gribov
 * 
 * Use of this source code is governed by The MIT License (MIT)
 * that can be found in the LICENSE.txt file.
 */

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <clang-c/Index.h>
#include <pcrecpp.h>

#include <vector>
#include <string>

class FlakyIteratorsContext {
  struct LoopInfo {
    bool Flaky;
    LoopInfo(bool Flaky) : Flaky(Flaky) {}
  };
  const pcrecpp::RE &Pat;
  std::vector<LoopInfo> Loops;
  int Verbose;

public:

  FlakyIteratorsContext(const pcrecpp::RE &Pat, int Verbose) : Pat(Pat), Verbose(Verbose) {
    Loops.push_back(LoopInfo(false));
  }

  int v() const { return Verbose; }

  bool isInFlakyLoop() const {
    return Loops.back().Flaky;
  }

  void push(bool Flaky) {
    Loops.push_back(isInFlakyLoop() || Flaky);
  }

  void pop() {
    Loops.pop_back();
  }

  bool isIOCall(const std::string &Name) const {
    return Pat.PartialMatch(Name);
  }
};

enum CXChildVisitResult analyzeProgram(CXCursor C, CXCursor Parent, CXClientData Data);

#endif
