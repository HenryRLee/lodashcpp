#include "include/lodash/lodash.h"
#include <vector>
#include <map>
#include <string>
#include <cassert>

using namespace lodash;

void testIdentity() {
  int a = 1;

  assert(a == _.chain(a).identity().value());
}

void testFirst() {
  std::vector<int> a{1, 2, 3};

  assert(1 == _.chain(a).first().value());
}

void testHead() {
  std::vector<int> a{1, 2, 3};

  assert(1 == _.chain(a).head().value());
}

void testLast() {
  std::vector<int> a{1, 2, 3};

  assert(3 == _.chain(a).last().value());
}

void testTake() {
  std::vector<int> a{1, 2, 3};

  std::vector<int> b{1, 2};

  assert(b == _.chain(a).take(2).value());
}

void testGet() {
  std::map<std::string, int> a{{"key1", 1}, {"key2", 2}};

  assert(1 == _.chain(a).get("key1").value());
}

void testHas() {
  std::map<std::string, int> a{{"key1", 1}, {"key2", 2}};

  assert(_.chain(a).has("key1").value());
}

void testSet() {
  std::map<std::string, int> a{{"key1", 1}, {"key2", 2}};
  std::map<std::string, int> b{{"key1", 1}, {"key2", 2}, {"key3", 3}};

  assert(b == _.chain(a).set("key3", 3).value());
}

void testPartial() {
  auto add = [](auto a, auto b) {
    return a + b;
  };

  auto addPartial = _.partial(add);

  assert(3 == addPartial(1, 2));
  assert(3 == addPartial(1)(2));

  auto addOneTwo = _.partial(add, 1, 2);

  assert(3 == addOneTwo());
}

void testCurry() {
  auto add = [](auto a, auto b) {
    return a + b;
  };

  auto addCurry = _.curry(add);

  assert(3 == addCurry(1)(2));
  assert(3 == addCurry(1, 2));
}

int main() {
  testIdentity();
  testFirst();
  testHead();
  testLast();
  testTake();
  testPartial();
  testCurry();
}
