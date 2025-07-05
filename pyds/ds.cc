#include <ds/ds.hh>
#include <pybind11/pybind11.h>

namespace py = pybind11;

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
auto to_string(T* value, int buffer_size) -> py::str {
    auto result = reinterpret_cast<char*>(operator new(buffer_size));
    auto print_result = value->print(result, reinterpret_cast<char*>(result) + buffer_size);
    if (print_result == nullptr || print_result - result == buffer_size - 1) {
        operator delete(result);
        return py::str();
    }
    *print_result = 0;
    auto string = py::str(result);
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
auto to_binary(T* value) -> py::bytes {
    return py::bytes(reinterpret_cast<char*>(value), value->data_size());
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
auto common_declaration(py::class_<T>& t) {
    t.def_static("from_string", from_string<T>);
    t.def_static("to_string", to_string<T>);
    t.def_static("from_binary", from_binary<T>);
    t.def_static("to_binary", to_binary<T>);
    t.def("clone", clone<T>);
    t.def("data_size", data_size<T>);
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

PYBIND11_MODULE(ds, m) {
    auto string_t = py::class_<ds::string_t>(m, "String");
    auto item_t = py::class_<ds::item_t>(m, "Item");
    auto variable_t = py::class_<ds::variable_t>(m, "Variable");
    auto list_t = py::class_<ds::list_t>(m, "List");
    auto term_t = py::class_<ds::term_t>(m, "Term");
    auto rule_t = py::class_<ds::rule_t>(m, "Rule");

    common_declaration(string_t);
    common_declaration(item_t);
    common_declaration(variable_t);
    common_declaration(list_t);
    common_declaration(term_t);
    common_declaration(rule_t);

    item_t.def("name", &ds::item_t::name, py::return_value_policy::reference_internal);

    variable_t.def("name", &ds::variable_t::name, py::return_value_policy::reference_internal);

    list_t.def("__len__", &ds::list_t::get_list_size);
    list_t.def("__getitem__", &ds::list_t::term, py::return_value_policy::reference_internal);

    py::enum_<ds::term_type_t>(term_t, "Type")
        .value("Variable", ds::term_type_t::variable)
        .value("Item", ds::term_type_t::item)
        .value("List", ds::term_type_t::list)
        .value("Null", ds::term_type_t::null)
        .export_values();
    term_t.def("get_type", &ds::term_t::get_type);
    term_t.def("variable", &ds::term_t::variable, py::return_value_policy::reference_internal);
    term_t.def("item", &ds::term_t::item, py::return_value_policy::reference_internal);
    term_t.def("list", &ds::term_t::list, py::return_value_policy::reference_internal);

    rule_t.def("__len__", &ds::rule_t::premises_count);
    rule_t.def("conclusion", &ds::rule_t::conclusion, py::return_value_policy::reference_internal);
    rule_t.def("__getitem__", &ds::rule_t::premises, py::return_value_policy::reference_internal);

    term_t.def_static("ground", term_ground);
    rule_t.def_static("ground", rule_ground);
    rule_t.def_static("match", rule_match);
}
