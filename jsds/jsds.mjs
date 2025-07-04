/* jshint esversion:6 */

import createDs from './ds.mjs';

const ds = await createDs(); // jshint ignore:line

let _buffer_size = 1024;

export function buffer_size(size = 0) {
    const old_buffer_size = _buffer_size;
    if (size != 0) {
        _buffer_size = size;
    }
    return old_buffer_size;
}

class _common_t {
    constructor(type, value, size = 0) {
        this.type = type;
        if (value instanceof _common_t) {
            this.value = value.value;
            this.capacity = value.capacity;
            if (size != 0) {
                throw new Error("Cannot set capacity when copying from another instance.");
            }
        } else if (value instanceof this.type) {
            this.value = value;
            this.capacity = size;
        } else if (value instanceof Uint8Array) {
            this.value = this.type.from_binary(value);
            this.capacity = this.size();
            if (size != 0) {
                throw new Error("Cannot set capacity when initializing from bytes.");
            }
        } else if (typeof value == "string") {
            this.capacity = size != 0 ? size : buffer_size();
            this.value = this.type.from_string(value, this.capacity);
            if (this.value == null) {
                throw new Error("Initialization from a string failed.");
            }
        } else {
            throw new Error("Unsupported type for initialization.");
        }
    }

    toString() {
        const result = this.type.to_string(this.value, buffer_size());
        if (result == "") {
            throw new Error("Conversion to string failed.");
        }
        return result;
    }

    data() {
        return this.type.to_binary(this.value);
    }

    data_string() {
        const decoder = new TextDecoder("utf-8");
        return decoder.decode(this.data());
    }

    size() {
        return this.value.data_size();
    }

    copy() {
        return new this.constructor(this.value.clone(), this.size());
    }
}

export class string_t extends _common_t {
    constructor(value, size = 0) {
        super(ds.String, value, size);
    }
}

export class variable_t extends _common_t {
    constructor(value, size = 0) {
        super(ds.Variable, value, size);
    }

    name() {
        return new string_t(this.value.name());
    }
}

export class item_t extends _common_t {
    constructor(value, size = 0) {
        super(ds.Item, value, size);
    }

    name() {
        return new string_t(this.value.name());
    }
}

export class list_t extends _common_t {
    constructor(value, size = 0) {
        super(ds.List, value, size);
    }

    length() {
        return this.value.length();
    }

    getitem(index) {
        return new term_t(this.value.getitem(index));
    }
}

export class term_t extends _common_t {
    constructor(value, size = 0) {
        super(ds.Term, value, size);
    }

    term() {
        const term_type = this.value.get_type();
        if (term_type == ds.TermType.Variable) {
            return new variable_t(this.value.variable());
        } else if (term_type == ds.TermType.Item) {
            return new item_t(this.value.item());
        } else if (term_type == ds.TermType.List) {
            return new list_t(this.value.list());
        } else {
            throw new TypeError("Unexpected term type.");
        }
    }

    ground(other) {
        const capacity = buffer_size();
        const term = this.type.ground(this.value, other.value, capacity);
        if (term == null) {
            return null;
        }
        return new this.constructor(term, capacity);
    }
}

export class rule_t extends _common_t {
    constructor(value, size = 0) {
        super(ds.Rule, value, size);
    }

    length() {
        return this.value.length();
    }

    getitem(index) {
        return new term_t(this.value.getitem(index));
    }

    conclusion() {
        return new term_t(this.value.conclusion());
    }

    ground(other) {
        const capacity = buffer_size();
        const rule = this.type.ground(this.value, other.value, capacity);
        if (rule == null) {
            return null;
        }
        return new this.constructor(rule, capacity);
    }

    match(other) {
        const capacity = buffer_size();
        const rule = this.type.match(this.value, other.value, capacity);
        if (rule == null) {
            return null;
        }
        return new this.constructor(rule, capacity);
    }
}
