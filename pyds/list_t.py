from __future__ import annotations
import typing
from . import ds
from .common import Common


class List(Common[ds.List]):

    _base = ds.List

    def __len__(self) -> int:
        return len(self.value)

    def __getitem__(self, index: int) -> Term:
        from .term_t import Term
        return Term(self.value[index])
