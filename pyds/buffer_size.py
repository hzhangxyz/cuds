_buffer_size = 1024


def buffer_size(size: int = 0) -> int:
    global _buffer_size
    old_buffer_size = _buffer_size
    if size > 0:
        _buffer_size = size
    return old_buffer_size
