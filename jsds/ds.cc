#include <ds/ds.hh>
#include <emscripten/bind.h>

namespace em = emscripten;

template<typename T>
auto from_string(const std::string& text, int buffer_size) -> std::unique_ptr<T> {
    auto result = reinterpret_cast<T*>(operator new(buffer_size));
    auto scan_result = result->scan(text.c_str(), reinterpret_cast<std::byte*>(result) + buffer_size);
    if (scan_result == nullptr) {
        operator delete(result);
        return std::unique_ptr<T>(nullptr);
    }
    return std::unique_ptr<T>(result);
}

template<typename T>
auto to_string(T* value, int buffer_size) -> std::string {
    auto result = reinterpret_cast<char*>(operator new(buffer_size));
    auto print_result = value->print(result, reinterpret_cast<char*>(result) + buffer_size);
    if (print_result == nullptr || print_result - result == buffer_size - 1) {
        operator delete(result);
        return std::string();
    }
    *print_result = 0;
    auto string = std::string(result);
    operator delete(result);
    return string;
}

template<typename T>
auto from_binary(const std::string& text) -> std::unique_ptr<T> {
    auto dst = std::unique_ptr<T>(reinterpret_cast<T*>(operator new(text.size())));
    memcpy(dst.get(), text.data(), text.size());
    return dst;
}

template<typename T>
auto to_binary(T* value) -> em::val {
    return em::val(em::typed_memory_view(value->data_size(), reinterpret_cast<unsigned char*>(value)));
}

template<typename T>
auto clone(T* value) -> std::unique_ptr<T> {
    auto result = std::unique_ptr<T>(reinterpret_cast<T*>(operator new(value->data_size())));
    memcpy(result.get(), value, value->data_size());
    return result;
}

template<typename T>
auto data_size(T* value) -> int {
    return value->data_size();
}

template<typename T>
auto common_declaration(em::class_<T>& t) {
    t.class_function("from_string", from_string<T>);
    t.class_function("to_string", to_string<T>, em::allow_raw_pointers());
    t.class_function("from_binary", from_binary<T>);
    t.class_function("to_binary", to_binary<T>, em::return_value_policy::reference());
    t.function("clone", clone<T>, em::allow_raw_pointers());
    t.function("data_size", data_size<T>, em::allow_raw_pointers());
}

auto term_ground(ds::term_t* term, ds::term_t* dictionary, int length) -> std::unique_ptr<ds::term_t> {
    auto result = reinterpret_cast<ds::term_t*>(operator new(length));
    if (result->ground(term, dictionary, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
        operator delete(result);
        return std::unique_ptr<ds::term_t>(nullptr);
    }
    return std::unique_ptr<ds::term_t>(result);
}

auto rule_ground(ds::rule_t* rule, ds::rule_t* dictionary, int length) -> std::unique_ptr<ds::rule_t> {
    ds::rule_t* result = reinterpret_cast<ds::rule_t*>(operator new(length));
    if (result->ground(rule, dictionary, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
        operator delete(result);
        return std::unique_ptr<ds::rule_t>(nullptr);
    }
    return std::unique_ptr<ds::rule_t>(result);
}

auto rule_match(ds::rule_t* rule_1, ds::rule_t* rule_2, int length) -> std::unique_ptr<ds::rule_t> {
    ds::rule_t* result = reinterpret_cast<ds::rule_t*>(operator new(length));
    if (result->match(rule_1, rule_2, reinterpret_cast<std::byte*>(result) + length) == nullptr) {
        operator delete(result);
        return std::unique_ptr<ds::rule_t>(nullptr);
    }
    return std::unique_ptr<ds::rule_t>(result);
}

EMSCRIPTEN_BINDINGS(ds) {
    auto string_t = em::class_<ds::string_t>("String");
    auto item_t = em::class_<ds::item_t>("Item");
    auto variable_t = em::class_<ds::variable_t>("Variable");
    auto list_t = em::class_<ds::list_t>("List");
    auto term_t = em::class_<ds::term_t>("Term");
    auto rule_t = em::class_<ds::rule_t>("Rule");

    common_declaration(string_t);
    common_declaration(item_t);
    common_declaration(variable_t);
    common_declaration(list_t);
    common_declaration(term_t);
    common_declaration(rule_t);

    item_t.function("name", &ds::item_t::name, em::return_value_policy::reference());

    variable_t.function("name", &ds::variable_t::name, em::return_value_policy::reference());

    list_t.function("length", &ds::list_t::get_list_size);
    list_t.function("getitem", &ds::list_t::term, em::return_value_policy::reference());

    em::enum_<ds::term_type_t>("TermType")
        .value("Variable", ds::term_type_t::variable)
        .value("Item", ds::term_type_t::item)
        .value("List", ds::term_type_t::list)
        .value("Null", ds::term_type_t::null);
    term_t.function("get_type", &ds::term_t::get_type);
    term_t.function("variable", &ds::term_t::variable, em::return_value_policy::reference());
    term_t.function("item", &ds::term_t::item, em::return_value_policy::reference());
    term_t.function("list", &ds::term_t::list, em::return_value_policy::reference());

    rule_t.function("length", &ds::rule_t::premises_count);
    rule_t.function("conclusion", &ds::rule_t::conclusion, em::return_value_policy::reference());
    rule_t.function("getitem", &ds::rule_t::premises, em::return_value_policy::reference());

    term_t.class_function("ground", term_ground, em::return_value_policy::take_ownership());
    rule_t.class_function("ground", rule_ground, em::return_value_policy::take_ownership());
    rule_t.class_function("match", rule_match, em::return_value_policy::take_ownership());
}
