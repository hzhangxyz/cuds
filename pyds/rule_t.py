from __future__ import annotations
from . import ds
from .common import Common
from .term_t import Term
from .buffer_size import buffer_size


class Rule(Common[ds.Rule]):

    _base = ds.Rule

    def __len__(self) -> int:
        return len(self.value)

    def __getitem__(self, index: int) -> Term:
        return Term(self.value[index])

    @property
    def conclusion(self) -> Term:
        return Term(self.value.conclusion())

    def __floordiv__(self, other: Rule) -> Rule | None:
        capacity = buffer_size()
        rule = ds.Rule.ground(self.value, other.value, capacity)
        if rule is None:
            return None
        return Rule(rule, capacity)

    def __matmul__(self, other: Rule) -> Rule | None:
        capacity = buffer_size()
        rule = ds.Rule.match(self.value, other.value, capacity)
        if rule is None:
            return None
        return Rule(rule, capacity)

    def __repr__(self) -> str:
        return f"Rule[\n{self}]"
