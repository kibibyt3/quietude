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
  make a separate version of `Qdatameta_t` for the sake of indirect control.
- [x] Resume work on qwalk
- [x] complete qwalkl rough draft
  - [x] add write functionality to qwalkl
  - [x] add read functionality to qwalkl
  - [x] give qwalkl a cursory debugging
    - Note: a more thorough debugging was done anyway
- [x] complete qwalkio rough draft
  - [x] give qwalkio a cursory debugging
- [x] thoroughly debug the entirety of qwalk
- [x] look into using the `/*@abstract@*/` `splint` annotation
  - In hindsight, this will prove too unwieldy to impelement.
- [x] rewrite the `Qdatameta_t` interface such that datap is never calloc'd 
  directly; it should just store a pointer to the beginning of the data
  - [x] rewrite `qfile`'s read function such that it creates a new
    `Qdatameta_t`.
    Note: This was implemented differently. Instead, it merely creates a new 
    `Qdata_t *` to give to the `Qdatameta_t` constructor function.
- [x] make qwalk be able to generate a save file from a human-readable data file 
  - Note: in the end, it proved more sensible to not implement this functionality.
- [x] add editing functionality to devel\_walk.
- [x] add support for the escape key in devel\_walk.
  - Note: This was instead implemented using F1.
- [x] add save/exit functionality to devel\_walk.
- [x] make the title of the area window display the name of the current layer in
  devel\_walk
- [x] CDK's alphalist is awful on account of it uses ESC (i.e. the fucking
  escape character) so the program seems to hang for 1 second or so while it
  waits. rewrite alphalist and make it better!
- [ ] Maybe let's add parameter names to function prototypes?
- [x] Finish the rough draft of the dialogue module.
  - [x] Debug the initialization function therein.
  - [x] Finish (and perhaps relocate to its own module) the logic section.
  - [x] Finish the I/O section.
- [ ] Rewrite `dialogue_file_string_to_tree()` such that it's divided into
  subfunctions all of which take a substring of whatever they need to work on.
  e.g. `DialogueObject_t dialogue_file_string_to_object(char *file_substring)`.
- [x] Add destructor functions to dialogue.
- [x] Add ability to add/remove object attributes in devel\_walk.
- [x] Add the ability to show the tree title in the dialogue I/O.
- [x] In `devel_attr_list_default_create()` replace defaulting functionality w/
  `qwalk_attr_list_attr_set_default()` wherever possible.
- [ ] Isolate the else-ifs in `devel_walkio_in()` into their own individual
  functions.
- [x] Change all destructor functions such that they return `void`.
  - [x] Make `qdatameta_destroy` return `void`.
- [x] Change qwalk such that the argument provided in the init function refers
  to a specific area data file, rather than the area itself.
- [x] Add the alternative controls (as implemented in `devel_walk`) into `q`.
- [x] Implement player-object interaction.
- [x] Implement dialogue `become` command.
  - [x] Finish qdefault.
- [x] Make the environment log work.
- [x] Make everything w/ defaulting behaviour use qdefault.
  - [x] Make devel\_walk use qdefault.
- [x] After `loc_attr_list_set` is rewritten in the broader conext of qwalk,
  make devel\_walk use it, too.
  - NOTE: the rewrite was implemented differently such that this no longer
    applies.
- [x] Make qwalk's dialogue interface use the distance-finding function from
  `qutils.h`.
- [x] Add object-inspection functionality.
  - [x] This includes being able to hover over an object to see the name and
    brief description.
- [x] Fix the object inspection only looking at the earth layer.
  - [x] Lint the new functionality.
- [ ] Create the player module.
