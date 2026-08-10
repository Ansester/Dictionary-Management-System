# Multi-language Dictionary Management System

A C++ application for managing multi-language dictionary entries using a custom open-addressing hash table with quadratic probing.

---

## Context & Provenance

* **Project Context**: Coursework project for Data Structures (NYU Abu Dhabi).
* **Author**: Ashmit Mukherjee.

---

## Technical Details & Architecture

### Custom Hash Table Implementation (`HashTable.h` / `HashTable.cpp`)

* **Collision Resolution**: Open addressing with quadratic probing.
* **Hash Function**: Polynomial rolling hash for string key indexing.
* **Supported Operations**:
  * `find <word>`: $O(1)$ average-time lookup for word definitions and translations.
  * `add <word,meaning,language>`: Inserts new word definitions and language associations.
  * `delWord <word>`: Removes word entries and associated translations.
  * `import <file>` / `export <lang,file>`: File I/O serialization for loading and saving dictionary state.

---

## Running Locally

```bash
# Compile with Makefile
make

# Execute CLI
./translator
```

Example interactive session:
```text
find heat
add algorithm,step-by-step procedure,german
find algorithm
export german,exported_german.txt
exit
```

---

## License

This repository's source code is licensed under the [MIT License](LICENSE).
