#include <iostream>
#include <unordered_set>

void bar(int i);
std::unordered_set<int> m;

void foo() {
  for (auto k : m) {
    std::cout << k;
  }
}
