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

#include "utils.h"
#include "error.h"

#include <clang-c/Index.h>

#include <stdlib.h>
#include <libgen.h>
#include <getopt.h>

#include <string>
#include <vector>
#include <iostream>

static void usage(const char *prog) {
  printf("\
Usage: %s [OPT]... FILE...\n\
Print APIs provided by header(s).\n\
\n\
Options:\n\
  -h, --help                 Print this help and exit.\n\
  -v, --verbose              Enable debug prints.\n\
  -c FLAGS, --cflags FLAGS   Specify CFLAGS to use.\n\
", prog);
  exit(0);
}

int main(int argc, char *argv[]) {
  const char *me = basename((char *)argv[0]);
  SetProgName(me);

  // Parse arguments

  int Verbose = 0;
  std::string FlagsString;

  while (1) {
    static struct option long_opts[] = {
      {"verbose", no_argument, 0, 'v'},
      {"cflags", required_argument, 0, 'c'},
      {"help", no_argument, 0, 'h'},
    };

    int opt_index = 0;
    int c = getopt_long(argc, argv, "vc:h", long_opts, &opt_index);

    if (c == -1)
      break;

    switch (c) {
    case 'v':
      ++Verbose;
      break;
    case 'c':
      FlagsString = optarg;
      break;
    case 'h':
      usage(me);
      break;
    default:
      abort();
    }
  }

  // Analyze --cflags

  std::vector<const char *> FlagsArray;
  for (size_t End = 0; End < std::string::npos; ) {
    size_t Begin = FlagsString.find_first_not_of(" \t", End);
    if (Begin == std::string::npos)
      break;
    End = FlagsString.find_first_of(" \t", Begin + 1);
    if (End != std::string::npos)
      FlagsString[End++] = 0;
    FlagsArray.push_back(&FlagsString[Begin]);
  }

  if (optind >= argc) {
    Error() << "no files specified" << '\n';
    exit(1);
  }

  // Analyze files

  for (int I = optind; I < argc; ++I) {
    auto *Src = argv[I];

    CXIndex Idx = clang_createIndex(0, 0);
    CXTranslationUnit Unit = clang_parseTranslationUnit(
        Idx, Src,
        FlagsArray.empty() ? nullptr : &FlagsArray[0], FlagsArray.size(),
        0, 0, CXTranslationUnit_None);
    if (!Unit) {
      Error() << "failed to parse " << Src << '\n';
      exit(1);
    }

    bool AnyError = false;
    for (unsigned J = 0, N = clang_getNumDiagnostics(Unit); J != N; ++J) {
      CXDiagnostic D = clang_getDiagnostic(Unit, J);
      clang_disposeDiagnostic(D);
      CXDiagnosticSeverity S = clang_getDiagnosticSeverity(D);
      const bool IsError = S >= CXDiagnostic_Error;
      if (IsError || (S >= CXDiagnostic_Warning && Verbose)) {
        std::string Msg = ToStr(clang_formatDiagnostic(D, clang_defaultDiagnosticDisplayOptions()));
        (IsError ? Error() : Warning()) << Msg << '\n';
      }
      AnyError |= IsError;
    }

    if (AnyError)
      return 1;

    FlakyIteratorsContext Ctx(Verbose);
    clang_visitChildren(clang_getTranslationUnitCursor(Unit), analyzeProgram, (CXClientData)&Ctx);

    clang_disposeIndex(Idx);
    clang_disposeTranslationUnit(Unit);
  }

  return 0;
}
