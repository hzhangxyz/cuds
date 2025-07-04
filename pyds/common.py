from __future__ import annotations
import typing
from . import ds
from .buffer_size import buffer_size

T = typing.TypeVar("T")


class Common(typing.Generic[T]):

    _base: type[T]

    def __init__(self, value: Common[T] | T | str | bytes, size: int | None = None) -> None:
        self.value: T
        self.capacity: int | None
        if isinstance(value, type(self)):
            self.value = value.value
            self.capacity = value.capacity
            if size is not None:
                raise ValueError("Cannot set capacity when copying from another instance.")
        elif isinstance(value, self._base):
            self.value = value
            self.capacity = size
            if size is None:
                raise ValueError("Size must be specified when initializing from a base type.")
        elif isinstance(value, str):
            self.capacity = size if size is not None else buffer_size()
            self.value = self._base.from_string(value, self.capacity)
            if self.value is None:
                raise ValueError("Initialization from a string failed.")
        elif isinstance(value, bytes):
            self.value = self._base.from_binary(value)
            self.capacity = self.size()
            if size is not None:
                raise ValueError("Cannot set capacity when initializing from bytes.")
        else:
            raise TypeError("Unsupported type for initialization.")

    def __str__(self) -> str:
        result = self._base.to_string(self.value, buffer_size())
        if result == "":
            raise ValueError("Conversion to string failed.")
        return result

    def __repr__(self) -> str:
        return f"{type(self).__name__}[{self}]"

    def data(self) -> bytes:
        return self._base.to_binary(self.value)

    def size(self) -> int:
        return self.value.data_size()

    def __copy__(self) -> Common[T]:
        return type(self)(self.value.clone(), self.size())

    def __hash__(self) -> int:
        return hash(self.data())
