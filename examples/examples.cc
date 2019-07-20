#include "lodash/lodash.h"
#include <vector>
#include <map>
#include <string>
#include <cassert>

using namespace lodash;

void First() {
  std::vector<int> a{1, 2, 3};

  assert(1 == _.chain(a).first().value());
}

void Head() {
  std::vector<int> a{1, 2, 3};

  assert(1 == _.chain(a).head().value());
}

void Last() {
  std::vector<int> a{1, 2, 3};

  assert(3 == _.chain(a).last().value());
}

void Take() {
  std::vector<int> a{1, 2, 3};

  std::vector<int> b{1, 2};

  assert(b == _.chain(a).take(2).value());
}

void Get() {
  std::map<std::string, int> a{{"key1", 1}, {"key2", 2}};

  assert(1 == _.chain(a).get("key1").value());
}

void Has() {
  std::map<std::string, int> a{{"key1", 1}, {"key2", 2}};

  assert(_.chain(a).has("key1").value());
}

void Set() {
  std::map<std::string, int> a{{"key1", 1}, {"key2", 2}};
  std::map<std::string, int> b{{"key1", 1}, {"key2", 2}, {"key3", 3}};

  assert(b == _.chain(a).set("key3", 3).value());
}

void IsEqual() {
  std::map<std::string, int> a = {{"a", 1}, {"b", 2}};
  std::map<std::string, int> b = {{"a", 1}, {"b", 2}};

  assert(_.isEqual(a, b));
}

void IsMatch() {
  std::map<std::string, int> a = {{"a", 1}, {"b", 2}, {"c", 3}};
  std::map<std::string, int> b = {{"a", 1}, {"b", 2}};
  std::map<std::string, int> c = {{"b", 1}, {"c", 2}};

  assert(_.isMatch(a, b));
  assert(!_.isMatch(a, c));
}

void Curry() {
  auto add = [](auto a, auto b) {
    return a + b;
  };

  auto addCurry = _.curry(add);

  assert(3 == addCurry(1)(2));
  assert(3 == addCurry(1, 2));
}

void CurryRight() {
  auto abc = [](int a, int b, int c) {
    return std::vector<int>{a, b, c};
  };

  auto abcCurry = _.curryRight(abc);

  std::vector<int> a{1, 2, 3};

  assert(a == abcCurry(3)(2)(1));
  assert(a == abcCurry(2, 3)(1));
  assert(a == abcCurry(1, 2, 3));
}

void Partial() {
  auto add = [](auto a, auto b) {
    return a + b;
  };

  auto addPartial = _.partial(add);

  assert(3 == addPartial(1, 2));
  assert(3 == addPartial(1)(2));

  auto addOne = _.partial(add, 1);

  assert(3 == addOne(2));

  auto addOneTwo = _.partial(add, 1, 2);

  assert(3 == addOneTwo());
}

void PartialRight() {
  auto abc = [](int a, int b, int c) {
    return std::vector<int>{a, b, c};
  };

  std::vector<int> a{1, 2, 3};

  assert(a == _.partialRight(abc, 1, 2, 3)());
  assert(a == _.partialRight(abc)(3)(2)(1));

  auto abcPartial = _.partialRight(abc, 3);

  assert(a == abcPartial(2)(1));
  assert(a == abcPartial(1, 2));
}

void Identity() {
  int a = 1;

  assert(a == _.chain(a).identity().value());
}

void Matches() {
  std::map<std::string, int> a = {{"a", 1}, {"b", 2}, {"c", 3}};
  std::map<std::string, int> b = {{"a", 1}, {"b", 2}};
  std::map<std::string, int> c = {{"b", 1}, {"c", 2}};

  auto matchesB = _.matches(b);
  assert(matchesB(a));

  auto matchesC = _.matches(c);
  assert(!matchesC(a));
}

void MatchesProperty() {
  std::map<std::string, int> a = {{"a", 1}, {"b", 2}, {"c", 3}};

  assert(_.matchesProperty("b", 2)(a));
  assert(!_.matchesProperty("c", 2)(a));

  std::map<int, int> b = {{1, 10}, {2, 20}, {3, 30}};

  assert(_.matchesProperty(1, 10)(b));
  assert(!_.matchesProperty(2, 30)(b));
}

void Property() {
  std::map<std::string, int> a = {{"a", 1}, {"b", 2}, {"c", 3}};

  assert(1 == _.property("a")(a));
  assert(2 == _.property("b")(a));
  assert(3 == _.property("c")(a));
}

void Iteratee() {
  std::map<std::string, int> a = {{"a", 1}, {"b", 2}, {"c", true}};

  auto matches = _.iteratee(std::map<std::string, int>{{"a", 1}});
  assert(matches(a));

  auto matchesProperty = _.iteratee(std::make_pair("a", 1));
  assert(matchesProperty(a));

  auto property = _.iteratee("c");
  assert(matchesProperty(a));

  auto func = _.iteratee([](const auto& object) {
    return _.get(object, "a") + _.get(object, "b");
  });
  assert(3 == func(a));
}

void ForEach() {
  std::vector<int> a = {1, 2, 3, 4};
  std::vector<int> b = {2, 3, 4, 5};

  _.forEach(a, [](int n) { return n + 1; });

  assert(a == b);
}

void Map() {
  std::vector<int> a = {1, 2, 3, 4};
  std::vector<int> b = {2, 3, 4, 5};
  std::vector<std::map<std::string, int>> c = {
    {{"a", 1}, {"b", 2}},
    {{"a", 2}, {"b", 3}},
    {{"a", 3}, {"b", 4}},
    {{"a", 4}, {"b", 5}},
  };
  assert(a == _.map(c, "a"));
}

int main() {
  First();
  Head();
  Last();
  Take();
  Get();
  Has();
  Set();
  IsEqual();
  IsMatch();
  Curry();
  CurryRight();
  Partial();
  PartialRight();
  Identity();
  Matches();
  MatchesProperty();
  Property();
  Iteratee();
  ForEach();
  Map();
}
