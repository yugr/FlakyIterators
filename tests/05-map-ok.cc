#include <stdio.h>
#include <map>

void bar(int i);
std::map<int, int> m;
std::map<int *, int> m2;

void foo() {
  for (auto [k, v] : m) {
    bar(v);
    printf("%d", v);
  }
  for (auto [k, v] : m2) {
    bar(v);
  }
}
