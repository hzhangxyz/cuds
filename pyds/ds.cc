#include <pybind11/pybind11.h>

#include <ds/item.hh>
#include <ds/list.hh>
#include <ds/rule.hh>
#include <ds/string.hh>
#include <ds/term.hh>
#include <ds/utility.hh>
#include <ds/variable.hh>

namespace py = pybind11;

template<typename T>
auto from_string(std::string& text, int buffer_size) -> ds::unique_malloc_ptr<T> {
    auto result = reinterpret_cast<T*>(malloc(buffer_size));
    auto scan_result = result->scan(text.c_str(), reinterpret_cast<std::byte*>(result) + buffer_size);
    if (scan_result == nullptr) {
        free(result);
        return ds::unique_malloc_ptr<T>(nullptr);
    }
    return ds::unique_malloc_ptr<T>(result);
}

template<typename T>
auto to_string(T* value, int buffer_size) -> py::str {
    auto result = reinterpret_cast<char*>(malloc(buffer_size));
    auto print_result = value->print(result, reinterpret_cast<char*>(result) + buffer_size);
    if (print_result == nullptr || print_result - result == buffer_size - 1) {
        free(result);
        return py::str();
    }
    *print_result = 0;
    auto string = py::str(result);
    free(result);
    return string;
}

template<typename T>
auto from_binary(std::string& text) -> ds::unique_malloc_ptr<T> {
    auto dst = ds::unique_malloc_ptr<T>(reinterpret_cast<T*>(malloc(text.size())));
    memcpy(dst.get(), text.data(), text.size());
    return dst;
}

template<typename T>
auto to_binary(T* value) -> py::bytes {
    return py::bytes(reinterpret_cast<char*>(value), value->data_size());
}

template<typename T>
auto clone(T* value) -> ds::unique_malloc_ptr<T> {
    auto result = ds::unique_malloc_ptr<T>(reinterpret_cast<T*>(malloc(value->data_size())));
    memcpy(result.get(), value, value->data_size());
    return result;
}

template<typename T>
auto common_declaration(py::class_<T>& t) {
    t.def_static("from_string", from_string<T>);
    t.def_static("to_string", to_string<T>);
    t.def_static("from_binary", from_binary<T>);
    t.def_static("to_binary", to_binary<T>);
    t.def("clone", clone<T>);
    t.def("data_size", [](T* v) { return v->data_size(); });
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

    item_t.def(
        "name",
        [](ds::item_t* item) { return item->name(); },
        py::return_value_policy::reference_internal
    );

    variable_t.def(
        "name",
        [](ds::variable_t* variable) { return variable->name(); },
        py::return_value_policy::reference_internal
    );

    list_t.def("__len__", [](ds::list_t* list) { return list->get_list_size(); });
    list_t.def(
        "__getitem__",
        [](ds::list_t* list, int index) { return list->term(index); },
        py::return_value_policy::reference_internal
    );

    term_t.def(
        "term",
        [](ds::term_t* term) {
            if (term->get_type() == ds::term_type_t::variable) {
                return py::cast(term->variable());
            } else if (term->get_type() == ds::term_type_t::item) {
                return py::cast(term->item());
            } else if (term->get_type() == ds::term_type_t::list) {
                return py::cast(term->list());
            } else {
                return py::cast(nullptr);
            }
        },
        py::return_value_policy::reference_internal
    );

    rule_t.def("__len__", [](ds::rule_t* rule) { return rule->premises_count(); });
    rule_t.def(
        "conclusion",
        [](ds::rule_t* rule) { return rule->conclusion(); },
        py::return_value_policy::reference_internal
    );
    rule_t.def(
        "__getitem__",
        [](ds::rule_t* rule, int index) { return rule->premises(index); },
        py::return_value_policy::reference_internal
    );

    term_t.def_static("ground", [](ds::term_t* term, ds::term_t* dictionary, int buffer_size) { return ds::ground(term, dictionary, buffer_size); });
    rule_t.def_static("ground", [](ds::rule_t* rule, ds::rule_t* dictionary, int buffer_size) { return ds::ground(rule, dictionary, buffer_size); });
    rule_t.def_static("match", [](ds::rule_t* rule_1, ds::rule_t* rule_2, int buffer_size) { return ds::match(rule_1, rule_2, buffer_size); });
}
