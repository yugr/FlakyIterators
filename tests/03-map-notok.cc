#include <stdio.h>
#include <map>

std::map<int *, int> m;

void foo() {
  for (auto [k, v] : m) {
    printf("%d", v);
  }
}
