__all__ = [
    "Variable",
]

from . import ds
from .common import Common
from .string_t import String


class Variable(Common[ds.Variable]):

    _base = ds.Variable

    @property
    def name(self) -> String:
        return String(self.value.name())
