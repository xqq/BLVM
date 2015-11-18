CodeStyle
=========

We use Chromium-like google style.

Difference

- We use lowercase with undersources`_` file naming, e.g. `memory_buffer_ref.cpp`
- We use `.cpp` suffix for C++ source, `.hpp` suffix for C++ header.
- We relax the line length restrictions to `120` characters long. (origin: 80)
- We use `4-spaces` indent, because 2-spaces indent is ugly and unnecessary.
- Indent the content inside a namespace.

Emphasis

- We do not use C++ exceptions and RTTI.
- We use `LF` for line separator. Set `autocrlf` to true or input in your git-config if you're under Windows.
- Avoid Non-ASCII characters in source files, and must be `UTF-8` encoding.
