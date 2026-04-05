# Sneklang (Learning Project)

In this project I'm explore how to build tiny object system from scratch. We implement a few basic object types, build a list of objects, and experiment with a manual reference counting garbage‑collection style of memory management.

**Objects**
- `INTEGER`: stores an `int`. Supports `snek_add` with integers/floats and has length `1`.
- `FLOAT`: stores a `float`. Supports `snek_add` with integers/floats and has length `1`.
- `STRING`: stores a heap‑allocated C string. Supports `snek_add` (concatenation) and length via `strlen`.
- `VECTOR3`: stores three object pointers (`x`, `y`, `z`). Supports component‑wise `snek_add` and has length `3`.
- `ARRAY`: a fixed‑size list of object pointers. Supports `snek_array_set`, `snek_array_get`, `snek_add` (concatenation), and length equal to its size.

**Manual Reference Counting**
Each `snek_object_t` has a `refcount`:
- Constructors start at `refcount = 1`.
- `refcount_inc` is used when a new owner holds a reference.
- `refcount_dec` releases a reference; when it hits `0`, `refcount_free` recursively frees contained objects (strings, vectors, arrays).
- Containers like `VECTOR3` and `ARRAY` increment refcounts when storing elements and decrement them when the container is freed.

**Tests**
`main.c` is a small smoke‑test harness that:
- Creates each object type.
- Verifies `snek_add` and `snek_length`.
- Exercises vector/array behavior.
- Prints PASS/FAIL output plus a summary.

**Build and Run (clang)**
We compile and run using `clang` for this project.
```bash
clang -std=c11 -Wall -Wextra -O2 -o program main.c snekobject.c
./program
```
