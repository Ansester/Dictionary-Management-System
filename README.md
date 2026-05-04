# Multi-language Dictionary Management System

## Overview
This project implements a HashTable-based dictionary management system in C++. The system allows users to manage a multi-language dictionary through various operations such as importing and exporting dictionaries, adding, deleting, and finding words and their meanings.

## Quick Start

```bash
make
./translator
```

The program automatically imports `en-de.txt`, a small sample German dictionary included in the repository. Example commands once the prompt opens:

```text
find heat
add algorithm,step-by-step procedure,german
find algorithm
export german,exported_german.txt
exit
```

## Table of Contents
- [Overview](#overview)
- [Technical Details](#technical-details)
  - [Class: HashTable](#class-hashtable)
  - [Main Program](#main-program)
  - [User Commands](#user-commands)
- [Usage Instructions](#usage-instructions)
- [Conclusion](#conclusion)

## Technical Details

### Class: HashTable
The `HashTable` class is the core of the system, encapsulating all functionalities related to dictionary management. Here are the primary methods:

- `import`: Reads dictionary data from a specified file and imports it into the system.
- `exportData`: Exports the current dictionary data to a specified file.
- `find`: Searches for a word in the dictionary and displays its meanings and translations.
- `addWord`: Adds a new word and its meanings and translations to the dictionary.
- `delWord`: Deletes a word and all its translations from the dictionary.
- `delTranslation`: Deletes a specific translation of a word from the dictionary.
- `delMeaning`: Deletes a specific meaning of a word from the dictionary.

### Main Program
The main program in `main.cpp` initializes a `HashTable` object and then enters a loop to process user commands. It uses the following functions and features:

- `help`: Displays the list of available commands and their descriptions.
- Command parsing and execution: The program reads user input, parses the command and its parameters, and calls the corresponding `HashTable` method.

### User Commands
The system supports the following commands:

- `find <word>`: Search for a word and its meanings in the dictionary.
- `import <path>`: Import a dictionary file.
- `export <path>`: Export the dictionary to a file.
- `add <word,meaning(s),language>`: Add a word and its meanings (separated by `;`) to the dictionary.
- `delTranslation <word,language>`: Delete a specific translation of a word from the dictionary.
- `delMeaning <word,meaning,language>`: Delete a specific meaning of a word from the dictionary.
- `delWord <word>`: Delete a word and all its translations from the dictionary.
- `export <language,filename>`: Export a given language dictionary to a file.
- `exit`: Exit the program.

## Usage Instructions
1. Compile the program with `make`.
2. Run the compiled program (`./translator`).
3. Use the provided commands to manage the dictionary.

## Conclusion
This dictionary management system project demonstrates the implementation of a functional multi-language dictionary using a hash table. It includes features for dictionary management, providing a comprehensive tool for handling multiple languages.
