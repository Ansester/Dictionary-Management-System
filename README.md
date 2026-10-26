# Multi-language dictionary management system

A C++ Data Structures coursework project implementing a multi-language dictionary with a custom open-addressing hash table. It supports multiple meanings for each word and language, deletion, and text import/export.

## Build and run

Requires a C++17 compiler and Make; Python 3 is needed for the CLI checks. No external libraries are required.

```bash
make
./translator
```

Run from the repository directory to load the six-entry `en-de.txt` demonstration fixture automatically. If it is absent, the program explains that it is starting with an empty dictionary; files can then be imported explicitly.

Example session:

```text
find heat
add algorithm,step-by-step procedure;Verfahren,German
find algorithm
export German,exported_german.txt
exit
```

## Commands and file format

| Command | Effect |
| --- | --- |
| `find <word>` | Print a word's translations |
| `add <word,meaning(s),language>` | Add a word or extend it; separate meanings with `;` |
| `delWord <word>` | Delete the word and all translations |
| `delTranslation <word,language>` | Delete one language association |
| `delMeaning <word,meaning,language>` | Delete an exact meaning |
| `import <path>` | Read a dictionary file |
| `export <language,path>` | Write one language to a file |
| `help` / `exit` | Show commands or exit |

A dictionary file has a language header followed by `word:meaning;meaning` records:

```text
German
hello:hallo;guten tag
water:Wasser
```

Whitespace around fields is trimmed. Language labels are normalized to ASCII lowercase on every operation, so `German` and `german` select the same language. Words and meanings preserve their spelling and UTF-8 bytes and are **case-sensitive**; full Unicode case folding is not implemented. Identical meanings within a language are not duplicated. Semicolons separate meanings; word keys cannot contain colons or semicolons. The CLI uses commas as argument separators and has no escaping syntax for comma-containing fields. Filenames may contain spaces.

Malformed import syntax is validated before records are inserted. Removing the final meaning removes that empty language association; a word may remain with no translations until `delWord` is used. Only languages with meanings are exported. Export order reflects hash-table order and can change after resizing. The bundled file is a small demonstration vocabulary, not a comprehensive or independently validated translation resource.

## Hash-table design

The hash is the unsigned **djb2** recurrence, `h = 33*h + byte`, starting at 5381. Collisions use quadratic probing `h + i²` over a prime capacity. Tombstones preserve search chains and can be reused. The table starts small, rebuilds deleted slots when needed, and grows before occupied slots exceed half capacity. Probe arithmetic avoids signed multiplication overflow. Owned entries are released automatically.

Expected average lookup is constant time under a suitable key distribution and controlled load; worst-case lookup remains linear. This is a teaching implementation, not a benchmarked alternative to `std::unordered_map`.

## Checks

```bash
make test
make clean
make CXXFLAGS="-std=c++17 -O1 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer" test
```

Checks cover colliding keys, tombstone reuse, growth, 1,000-key insert/delete/reinsert churn, import/export round trips, language normalization, spelling preservation, deletion, malformed imports, and EOF. GitHub Actions runs the suite with sanitizers on Ubuntu.

## Provenance and license

This is Ashmit Mukherjee's NYU Abu Dhabi Data Structures coursework repository. The original `main.cpp` driver header explicitly credited **Khalid Mengal**; that credit is retained in the maintained driver. The available files do not establish the precise original scaffold/student boundary or the scaffold's separate licensing permission. No claim of sole authorship over that historical driver is made.

The [MIT License](LICENSE) applies to the repository owner's contributions. It does not establish rights over any originally supplied scaffold. Later maintenance corrects probing/deletion, load management, input validation, normalization, serialization, and ownership; historical attribution remains as recorded above.
