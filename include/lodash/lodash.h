#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include <any>
#include <variant>

namespace lodash {
  class lodash {
    template<typename T>
    class chain_type;
   public:
    template<typename T>
    static chain_type<T> chain(T value) { return chain_type<T>(value); }

    constexpr static auto identity = [](const auto& any) { return any; };

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
      [](const auto& object, const std::any& path) {
      typedef typename
        std::remove_reference<decltype(object)>::type::key_type key_type;
      typedef typename
        std::remove_reference<decltype(object)>::type::mapped_type mapped_type;
      auto key = std::any_cast<key_type>(path);
      if (object.find(key) != object.end()) {
        return object.find(key)->second;
      } else {
        return mapped_type();
      }
    };

    constexpr static auto has =
      [](const auto& object, const std::any& path) {
      typedef typename
        std::remove_reference<decltype(object)>::type::key_type key_type;
      return object.find(std::any_cast<key_type>(path)) != object.end();
    };

    constexpr static auto set =
      [](auto& object, const std::any& path, const std::any& value) {
      typedef typename
        std::remove_reference<decltype(object)>::type::key_type key_type;
      typedef typename
        std::remove_reference<decltype(object)>::type::mapped_type mapped_type;
      object[std::any_cast<key_type>(path)] = std::any_cast<mapped_type>(value);
      return object;
    };

   private:
    template<typename T>
    class chain_type {
     public:
      chain_type(T value) : value_(value) {}

      T value() { return value_; }

      auto identity() {
        value_ = lodash::identity(value_);
        return *this;
      }

      // Array
      auto first() {
        return lodash::chain_type(lodash::head(value_));
      }

      auto head() {
        return first();
      }

      auto last() {
        return lodash::chain_type(lodash::last(value_));
      }

      auto take(int n = 1) {
        value_ = lodash::take(value_, n);
        return *this;
      }

      // Object
      auto get(const std::any& path) {
        return lodash::chain_type(lodash::get(value_, path));
      }

      auto has(const std::any& path) {
        return lodash::chain_type(lodash::has(value_, path));
      }

      auto set(const std::any& path, const std::any& value) {
        return lodash::chain_type(lodash::set(value_, path, value));
      }

     private:
      T value_;
    };
  };

  lodash _;
}
