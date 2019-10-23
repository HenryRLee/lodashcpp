#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include <any>
#include <variant>
#include <functional>
#include <algorithm>
#include <numeric>

namespace lodash {
  class lodash {
    template<typename T> class Chain;
    template<typename T> struct is_string;
    template<typename T> struct is_primitive;
    template<template<typename...> typename Array, typename V>
      struct transform_array;
   public:
    template<typename T>
    static Chain<T> chain(T value) { return Chain<T>(value); }

    // Array

    constexpr static auto first = [](const auto& array) {
      return array.front();
    };

    constexpr static auto head = first;

    constexpr static auto last = [](const auto& array) {
      return array.back();
    };

    constexpr static auto take = [](int n, const auto& array) {
      typedef typename std::remove_reference<decltype(array)>::type array_type;
      return array_type(array.begin(), array.begin() + n);
    };

    // Object

    constexpr static auto get =
      [](const auto& key, const auto& object) {
      typedef typename
        std::remove_reference<decltype(object)>::type::mapped_type mapped_type;
      if (object.find(key) != object.end()) {
        return object.find(key)->second;
      } else {
        return mapped_type();
      }
    };

    constexpr static auto has =
      [](const auto& key, const auto& object) {
      return object.find(key) != object.end();
    };

    constexpr static auto set =
      [](const auto& key, const auto& value, auto& object) {
      object[key] = value;
      return object;
    };

    // Lang

    constexpr static auto isEqual = [](const auto& other, const auto& value) {
      return value == other;
    };

    constexpr static auto isMatch = [](const auto& source, const auto& object) {
      for (auto it = source.begin(); it != source.end(); it++) {
        const auto& [key, value] = *it;
        if (!lodash::isEqual(lodash::get(key, object), value))
          return false;
      }

      return true;
    };

    // Function

    template<typename Fn,
             std::enable_if_t<std::is_invocable<Fn>::value, int> = 0>
    constexpr static decltype(auto) curry(Fn f) {
        return f();
    }

    template<typename Fn,
             std::enable_if_t<!std::is_invocable<Fn>::value, int> = 0>
    constexpr static decltype(auto) curry(Fn f) {
      return [=](auto&&... x) {
        return curry(
          [=](auto&&... xs) -> decltype(f(x..., xs...)) {
            return f(x..., xs...);
          }
        );
      };
    }

    template<typename Fn,
             std::enable_if_t<std::is_invocable<Fn>::value, int> = 0>
    constexpr static decltype(auto) curryRight(Fn f) {
        return f();
    }

    template<typename Fn,
             std::enable_if_t<!std::is_invocable<Fn>::value, int> = 0>
    constexpr static decltype(auto) curryRight(Fn f) {
      return [=](auto&&... x) {
        return curryRight(
          [=](auto&&... xs) -> decltype(f(xs..., x...)) {
            return f(xs..., x...);
          }
        );
      };
    }

    template<typename Fn, typename... Args,
             std::enable_if_t<std::is_invocable<Fn, Args...>::value, int> = 0>
    constexpr static decltype(auto) partial(Fn f, Args&&... x) {
        return [=]() -> decltype(f(x...)) {
          return f(x...);
        };
    }

    template<typename Fn, typename... Args,
             std::enable_if_t<!std::is_invocable<Fn, Args...>::value, int> = 0>
    constexpr static decltype(auto) partial(Fn f, Args... args) {
      return curry(f)(args...);
    }

    template<typename Fn, typename... Args,
             std::enable_if_t<std::is_invocable<Fn, Args...>::value, int> = 0>
    constexpr static decltype(auto) partialRight(Fn f, Args&&... x) {
        return [=]() -> decltype(f(x...)) {
          return f(x...);
        };
    }

    template<typename Fn, typename... Args,
             std::enable_if_t<!std::is_invocable<Fn, Args...>::value, int> = 0>
    constexpr static decltype(auto) partialRight(Fn f, Args... args) {
      return curryRight(f)(args...);
    }

    // Util

    constexpr static auto identity = [](const auto& any) { return any; };

    constexpr static auto matches = [](const auto& source) {
      return lodash::partial(lodash::isMatch, source);
    };

    template<typename K, typename V,
             std::enable_if_t<is_string<K>::value, int> = 0>
    constexpr static auto matchesProperty(const K& path,
                                          const V& srcValue) {
      const std::map<std::string, V> source{
        {std::string(path), srcValue}
      };
      return lodash::matches(source);
    }

    template<typename K, typename V,
             std::enable_if_t<!is_string<K>::value, int> = 0>
    constexpr static auto matchesProperty(const K& path,
                                          const V& srcValue) {
      const std::map<K, V> source{
        {path, srcValue}
      };
      return lodash::matches(source);
    }

    constexpr static auto property = [](const auto& path) {
      return lodash::partial(lodash::get, path);
    };

    template<typename K, typename V>
    constexpr static auto iteratee(const std::map<K, V>& object) {
      return lodash::matches(object);
    }

    template<typename K, typename V>
    constexpr static auto iteratee(const std::pair<K, V>& pair) {
      return lodash::matchesProperty(pair.first, pair.second);
    }

    template<typename K,
             std::enable_if_t<lodash::is_primitive<K>::value, int> = 0>
    constexpr static auto iteratee(const K& key) {
      return lodash::property(key);
    }

    template<typename Fn,
             std::enable_if_t<!lodash::is_primitive<Fn>::value, int> = 0>
    constexpr static auto iteratee(Fn func) {
      return func;
    }

    // Math

    constexpr static auto add = [](const auto& a, const auto& b) {
      return a + b;
    };

    constexpr static auto subtract = [](const auto& a, const auto& b) {
      return a - b;
    };

    constexpr static auto multiply = [](const auto& a, const auto& b) {
      return a * b;
    };

    template<typename N1, typename N2,
             std::enable_if_t<std::is_integral<N1>::value &&
                              std::is_integral<N2>::value, int> = 0>
    constexpr static auto divide(const N1& a, const N2& b) {
      return (long double)a / b;
    }

    template<typename N1, typename N2,
             std::enable_if_t<!std::is_integral<N1>::value ||
                              !std::is_integral<N2>::value, int> = 0>
    constexpr static auto divide(const N1& a, const N2& b) {
      return a / b;
    }

    constexpr static long long ceil(const long double& num) {
      if (num >= 0) {
        return (num * 10 - 1) / 10 + 1;
      } else {
        return -floor(-num);
      }
    }

    constexpr static long long floor(const long double& num) {
      if (num >= 0) {
        return num;
      } else {
        return -ceil(-num);
      }
    }

    constexpr static auto sumBy = [](const auto& iteratee,
                                     const auto& array) {
      typedef typename std::remove_reference<decltype(array)>::type Array;
      typedef decltype(lodash::iteratee(iteratee)(
          std::declval<typename Array::value_type>()))
            NewValueType;

      NewValueType ret = 0;
      for (auto it = array.begin(); it != array.end(); it++) {
        ret += lodash::iteratee(iteratee)(*it);
      }
      return ret;
    };

    constexpr static auto sum = [](const auto& array) {
      return lodash::sumBy(lodash::identity, array);
    };

    constexpr static auto meanBy = [](const auto& iteratee,
                                      const auto& array) {
      return lodash::divide(lodash::sumBy(iteratee, array), array.size());
    };

    constexpr static auto mean = [](const auto& array) {
      return lodash::meanBy(lodash::identity, array);
    };

    // Collection

    constexpr static auto each = [](const auto& iteratee,
                                    auto& collection) {
      std::transform(collection.begin(), collection.end(),
                     collection.begin(), lodash::iteratee(iteratee));
    };

    constexpr static auto forEach = each;

    template<template<typename...> typename Collection,
             typename Iteratee, typename... Args,
             typename NewValueType =
               decltype(lodash::iteratee(std::declval<Iteratee>())(
                 std::declval<typename Collection<Args...>::value_type>())),
             typename std::enable_if_t<std::is_same<NewValueType,
               typename Collection<Args...>::value_type>::value, int> = 0>
    constexpr static auto map(const Iteratee& iteratee,
                              const Collection<Args...>& collection) {
      Collection<Args...> newCollection(collection.size());

      std::transform(collection.begin(), collection.end(),
                     newCollection.begin(), lodash::iteratee(iteratee));
      return newCollection;
    }

    template<template<typename...> typename Collection,
             typename Iteratee, typename... Args,
             typename NewValueType =
               decltype(lodash::iteratee(std::declval<Iteratee>())(
                 std::declval<typename Collection<Args...>::value_type>())),
             typename std::enable_if_t<!std::is_same<NewValueType,
               typename Collection<Args...>::value_type>::value, int> = 0>
    constexpr static auto map(const Iteratee& iteratee,
                              const Collection<Args...>& collection) {
      typename transform_array<Collection, NewValueType>::type
          newCollection(collection.size());

      std::transform(collection.begin(), collection.end(),
                     newCollection.begin(), lodash::iteratee(iteratee));
      return newCollection;
    }

    template<typename Collection, typename Iteratee>
    constexpr static auto groupBy(const Iteratee& iteratee,
                                  const Collection& collection) {
      typedef typename Collection::value_type ValueType;
      typedef decltype(lodash::iteratee(std::declval<Iteratee>())(
          std::declval<ValueType>())) KeyType;

      std::map<KeyType, std::vector<ValueType>> ret;
      for (auto it = collection.begin(); it != collection.end(); it++) {
        ret[lodash::iteratee(iteratee)(*it)].push_back(*it);
      }
      return ret;
    };

    template<typename Collection,
             typename Iteratee,
             typename ValueType = typename Collection::value_type>
    constexpr static auto reduce(const Iteratee& iteratee,
                                 const ValueType& accumulator,
                                 const Collection& collection) {
      return std::reduce(collection.begin(), collection.end(),
                         accumulator, lodash::iteratee(iteratee));
    }

   private:
    template<typename T>
    class Chain {
     public:
      Chain(T value) : value_(value) {}

      T value() { return value_; }

      auto identity() {
        value_ = lodash::identity(value_);
        return *this;
      }

      // Array

      auto first() {
        return lodash::Chain(lodash::head(value_));
      }

      auto head() {
        return first();
      }

      auto last() {
        return lodash::Chain(lodash::last(value_));
      }

      auto take(int n = 1) {
        value_ = lodash::take(n, value_);
        return *this;
      }

      // Object

      template<typename K>
      auto get(const K& key) {
        return lodash::Chain(lodash::get(key, value_));
      }

      template<typename K>
      auto has(const K& key) {
        return lodash::Chain(lodash::has(key, value_));
      }

      template<typename K, typename V>
      auto set(const K& key, const V& value) {
        return lodash::Chain(lodash::set(key, value, value_));
      }

      template<typename Iteratee>
      auto groupBy(const Iteratee& iteratee) {
        return lodash::Chain(lodash::groupBy(iteratee, value_));
      }

      template<typename Iteratee>
      auto map(const Iteratee& iteratee) {
        return lodash::Chain(lodash::map(iteratee, value_));
      }

      template<typename Iteratee, typename Value>
      auto reduce(const Iteratee& iteratee, const Value& accumulator) {
        return lodash::Chain(lodash::reduce(iteratee, accumulator, value_));
      }

     private:
      T value_;
    };

    template<typename T>
    struct is_string : std::is_convertible<T, std::string> {};

    template<typename T>
    struct is_primitive : std::integral_constant<bool,
                                                 std::is_arithmetic<T>::value ||
                                                 std::is_enum<T>::value ||
                                                 is_string<T>::value> {};

    template<template<typename...> typename Array, typename V>
    struct transform_array {
      typedef Array<V> type;
    };
  };

  lodash _;
}
