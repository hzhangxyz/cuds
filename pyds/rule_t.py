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
        rule = ds.Rule.ground(self.value, other.value, buffer_size())
        if rule is None:
            return None
        return Rule(rule)

    def __matmul__(self, other: Rule) -> Rule | None:
        rule = ds.Rule.match(self.value, other.value, buffer_size())
        if rule is None:
            return None
        return Rule(rule)

    def __repr__(self) -> str:
        return f"Rule[\n{self}]"
