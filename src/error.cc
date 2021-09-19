#include "error.h"

#include <iostream>

namespace {

const char *Prog_ = "<unknown>";

}  // anon namespace

void SetProgName(const char *Prog) {
  Prog_ = Prog;
}

static std::ostream &Print(const char *Prefix) {
  std::cerr << Prog_ << ": " << Prefix << ": ";
  return std::cerr;
}

std::ostream &Error() {
  return Print("error");
}

std::ostream &Warning() {
  return Print("warning");
}

std::ostream &Note() {
  return Print("note");
}
