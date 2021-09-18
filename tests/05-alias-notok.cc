#include <stdio.h>
#include <map>

using map1 = std::map<int *, int>;
map1 m1;

void foo1() {
  for (auto [k, v] : m1) {
    printf("%d", v);
  }
}

typedef std::map<int *, int> map2;
map2 m2;

void foo2() {
  for (auto [k, v] : m2) {
    printf("%d", v);
  }
}
