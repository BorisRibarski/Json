#include "Json.hh"

Json::Value& Json::Value::operator=(const char* v) {
    ref = std::string(v);
    return *this;
}
Json::Value& Json::Value::operator=(const Json& js) {
    ref = std::make_shared<Json>(js);
    return *this;
}
Json::Value Json::Value::operator[](const std::string& key) {
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
Json::ConstValue
Json::ConstValue::operator[](const std::string& key) const {
    if (const auto* ptr = std::get_if<std::shared_ptr<Json>>(&ref)) {
        if (*ptr) {
            const Json& inner = **ptr;
            return ConstValue(inner[key]);
        }
    }
    return ConstValue(s_null_value);
}

Json::Value Json::operator[](const std::string key) {
    return Value(map[key]);
}
Json::ConstValue Json::operator[](const std::string key) const {
    auto it = map.find(key);
    if (it == map.end()) {
        // throw
    }
    return ConstValue(it->second);
}