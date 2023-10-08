# TODO

- [x] Finish making code readable for doxygen
- [x] Debug qattr & qdefs
- [x] Add a dictionary tying each `QDataType_t` to a data size. These should be dynamically written upon initialization and before anything else via `sizeof`.
  - Note: this was implemented differently, namely by way of a new function `qdata_type_size_get`.
- [x] Flesh out qfile
- [x] Debug qfile
- [x] Finish linting work
- [ ] Resume work on qwalk
