from . import ds
from .common import Common
from .string_t import String


class Item(Common[ds.Item]):

    _base = ds.Item

    @property
    def name(self) -> String:
        return String(self.value.name())
