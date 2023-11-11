# TODO

- [x] Finish making code readable for doxygen
- [x] Debug qattr & qdefs
- [x] Add a dictionary tying each `QDataType_t` to a data size. These should be
  dynamically written upon initialization and before anything else via `sizeof`.
  - Note: this was implemented differently, namely by way of a new function
  `qdata_type_size_get`.
- [x] Flesh out qfile
- [x] Debug qfile
- [x] Finish linting work
- [x] Rewrite qattr such that it controls its qdatameta pointers. If necessary,
  make a separate version of qdatameta\_t for the sake of indirect control.
- [x] Resume work on qwalk
- [x] complete qwalkl rough draft
  - [x] add write functionality to qwalkl
  - [x] add read functionality to qwalkl
  - [x] give qwalkl a cursory debugging
    - Note: a more thorough debugging was done anyway
- [x] complete qwalkio rough draft
  - [x] give qwalkio a cursory debugging
- [x] thoroughly debug the entirety of qwalk
- [ ] look into using the `/*@abstract@*/` `splint` annotation
- [x] rewrite the `Qdatameta\_t` interface such that datap is never calloc'd 
  directly; it should just store a pointer to the beginning of the data
  - [x] rewrite `qfile`'s read function such that it creates a new Qdatameta\_t.
    Note: This was implemented differently. Instead, it merely creates a new 
    `Qdata\_t *` to give to the `Qdatameta_t` constructor function.
- [x] make qwalk be able to generate a save file from a human-readable data file 
  - Note: in the end, it proved more sensible to not implement this functionality.
- [x] add editing functionality to devel\_walk.
- [x] add support for the escape key in devel\_walk.
  - Note: This was instead implemented using F1.
- [x] add save/exit functionality to devel\_walk.
- [x] make the title of the area window display the name of the current layer in
  devel\_walk
- [ ] CDK's alphalist is awful on account of it uses ESC (i.e. the fucking
  escape character) so the program seems to hang for 1 second or so while it
  waits. rewrite alphalist and make it better!
- [ ] Maybe let's add parameter names to function prototypes?
- [ ] Finish the rough draft of the dialogue module.
- [ ] Rewrite `dialogue_file_string_to_tree()` such that it's divided into
  subfunctions all of which take a substring of whatever they need to work on.
  e.g. `DialogueObject_t dialogue_file_string_to_object(char *file_substring)`.
