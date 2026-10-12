#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class Json {
  public:
    using value_type = std::variant<std::monostate,
                                    int,
                                    double,
                                    std::string,
                                    bool,
                                    std::shared_ptr<Json>>;

  private:
    std::unordered_map<std::string, value_type> map;

    static thread_local inline const value_type s_null_value =
        std::monostate{};
    static thread_local inline value_type s_mutable_null_value =
        std::monostate{};

    class Value {
      private:
        value_type& ref;

      public:
        explicit Value(value_type& v) : ref(v) {
        }
        template <typename T>
        Value& operator=(const T& v) {
            ref = v;
            return *this;
        }
        Value& operator=(const char* v) {
            ref = std::string(v);
            return *this;
        }
        Value& operator=(const Json& js) {
            ref = std::make_shared<Json>(js);
            return *this;
        }
        Value operator[](const std::string& key) {
            if (std::holds_alternative<std::monostate>(ref)) {
                ref = std::make_shared<Json>();
            }
            if (auto* ptr = std::get_if<std::shared_ptr<Json>>(&ref)) {
                if (!*ptr) {
                    *ptr = std::make_shared<Json>();
                }
                return (**ptr)[key];
            }
            return Value(s_mutable_null_value);
        }

        template <typename T>
        operator T() const {
            return std::get<T>(ref);
        }
    };
    class ConstValue {
      private:
        const value_type& ref;

      public:
        explicit ConstValue(const value_type& v) : ref(v) {
        }
        ConstValue operator[](const std::string& key) const {
            if (const auto* ptr =
                    std::get_if<std::shared_ptr<Json>>(&ref)) {
                if (*ptr) {
                    const Json& inner = **ptr;
                    return ConstValue(inner[key]);
                }
            }
            return ConstValue(s_null_value);
        }
        template <typename T>
        operator T() const {
            if (const auto* ptr = std::get_if<T>(&ref)) {
                return *ptr;
            }
            return T{};
        }
    };

  public:
    Value operator[](const std::string key) {
        return Value(map[key]);
    }
    ConstValue operator[](const std::string key) const {
        auto it = map.find(key);
        if (it == map.end()) {
            // throw
        }
        return ConstValue(it->second);
    }
};