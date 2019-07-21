#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include <any>
#include <variant>
#include <functional>
#include <algorithm>

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

    // Math

    template<typename Number>
    constexpr static long long ceil(const Number& num) {
      if (num >= 0) {
        return (num * 10 - 1) / 10 + 1;
      } else {
        return -floor(-num);
      }
    }

    template<typename Number>
    constexpr static long long floor(const Number& num) {
      if (num >= 0) {
        return num;
      } else {
        return -ceil(-num);
      }
    }

    // Array

    constexpr static auto first = [](const auto& array) {
      return array.front();
    };

    constexpr static auto head = first;

    constexpr static auto last = [](const auto& array) {
      return array.back();
    };

    constexpr static auto take = [](const auto& array, int n = 1) {
      typedef typename std::remove_reference<decltype(array)>::type array_type;
      return array_type(array.begin(), array.begin() + n);
    };

    // Object

    constexpr static auto get =
      [](const auto& object, const auto& key) {
      typedef typename
        std::remove_reference<decltype(object)>::type::mapped_type mapped_type;
      if (object.find(key) != object.end()) {
        return object.find(key)->second;
      } else {
        return mapped_type();
      }
    };

    constexpr static auto has =
      [](const auto& object, const auto& key) {
      return object.find(key) != object.end();
    };

    constexpr static auto set =
      [](auto& object, const auto& key, const auto& value) {
      object[key] = value;
      return object;
    };

    // Lang

    constexpr static auto isEqual = [](const auto& value, const auto& other) {
      return value == other;
    };

    constexpr static auto isMatch = [](const auto& object, const auto& source) {
      for (auto it = source.begin(); it != source.end(); it++) {
        const auto& [key, value] = *it;
        if (!lodash::isEqual(lodash::get(object, key), value))
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
      return lodash::partialRight(lodash::isMatch, source);
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
      return lodash::partialRight(lodash::get, path);
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

    // Collection

    constexpr static auto each = [](auto& collection,
                                    const auto& iteratee = lodash::identity) {
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
    constexpr static auto map(const Collection<Args...>& collection,
                              const Iteratee& iteratee) {
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
    constexpr static auto map(const Collection<Args...>& collection,
                              const Iteratee& iteratee) {
      typename transform_array<Collection, NewValueType>::type
          newCollection(collection.size());

      std::transform(collection.begin(), collection.end(),
                     newCollection.begin(), lodash::iteratee(iteratee));
      return newCollection;
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
        value_ = lodash::take(value_, n);
        return *this;
      }

      // Object

      template<typename K>
      auto get(const K& key) {
        return lodash::Chain(lodash::get(value_, key));
      }

      template<typename K>
      auto has(const K& key) {
        return lodash::Chain(lodash::has(value_, key));
      }

      template<typename K, typename V>
      auto set(const K& key, const V& value) {
        return lodash::Chain(lodash::set(value_, key, value));
      }

      template<typename Iteratee>
      auto map(const Iteratee& iteratee) {
        return lodash::Chain(lodash::map(value_, iteratee));
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
