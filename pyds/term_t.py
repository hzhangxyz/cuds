from __future__ import annotations
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
        value = self.value.term()

        if isinstance(value, ds.Variable):
            return Variable(value)
        elif isinstance(value, ds.Item):
            return Item(value)
        elif isinstance(value, ds.List):
            return List(value)
        else:
            raise TypeError(f"Unexpected term type")

    def __floordiv__(self, other: Term) -> Term | None:
        term = ds.Term.ground(self.value, other.value, buffer_size())
        if term is None:
            return None
        return Term(term)
