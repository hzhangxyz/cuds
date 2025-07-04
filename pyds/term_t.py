from __future__ import annotations

__all__ = [
    "Term",
]

from . import ds
from .common import Common
from .variable_t import Variable
from .item_t import Item
from .list_t import List
from .buffer_size import buffer_size


class Term(Common[ds.Term]):

    _base = ds.Term

    @property
    def term(self) -> Variable | Item | List:
        match self.value.get_type():
            case ds.Term.Type.Variable:
                return Variable(self.value.variable())
            case ds.Term.Type.Item:
                return Item(self.value.item())
            case ds.Term.Type.List:
                return List(self.value.list())
            case _:
                raise TypeError(f"Unexpected term type.")

    def __floordiv__(self, other: Term) -> Term | None:
        capacity = buffer_size()
        term = ds.Term.ground(self.value, other.value, capacity)
        if term is None:
            return None
        return Term(term, capacity)
