#include <stdio.h>
#include <unordered_map>

void bar(int i);
std::unordered_map<int, int> m;

void foo() {
  for (auto [k, v] : m) {
    printf("%d", k);
  }
}
