#include <stdio.h>
#include <map>

template<typename T>
struct CompareValues {
  bool operator()(const T *lhs, const T *rhs) const {
    return *lhs < *rhs;
  }
};

std::map<int *, int, CompareValues<int>> m;

void foo() {
  for (auto [k, v] : m) {
    printf("%d", v);
  }
}
