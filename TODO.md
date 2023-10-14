# TODO

- [x] Finish making code readable for doxygen
- [x] Debug qattr & qdefs
- [x] Add a dictionary tying each `QDataType_t` to a data size. These should be dynamically written upon initialization and before anything else via `sizeof`.
  - Note: this was implemented differently, namely by way of a new function `qdata_type_size_get`.
- [x] Flesh out qfile
- [x] Debug qfile
- [x] Finish linting work
- [x] Rewrite qattr such that it controls its qdatameta pointers. If necessary, make a separate version of qdatameta\_t for the sake of indirect control.
- [x] Resume work on qwalk
- [x] complete qwalkl rough draft
  - [ ] add read/write functionality to qwalkl
  - [ ] give qwalkl a cursory debugging
- [ ] complete qwalkio rough draft
  - [ ] give qwalkio a cursory debugging
- [ ] thoroughly debug the entirety of qwalk
