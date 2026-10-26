#ifndef DICTIONARY_HASHTABLE_H
#define DICTIONARY_HASHTABLE_H
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
class Translation {
    std::string language;
    std::vector<std::string> meanings;
    friend class Entry;
    friend class HashTable;
public:
    Translation(std::string meaning, std::string language);
    void addMeaning(std::string meaning);
    void display() const;
};
class Entry {
    std::string word;
    std::vector<Translation> translations;
    bool deleted = false;
    friend class HashTable;
public:
    Entry(std::string word, std::string meaning, std::string language);
    void addTranslation(std::string meaning, std::string language);
    void print() const;
};
class HashTable {
    std::vector<std::unique_ptr<Entry>> buckets;
    std::size_t size = 0, used = 0, collisions = 0;
    std::size_t locate(const std::string& word, bool countCollisions);
    void rehash(std::size_t capacity);
public:
    explicit HashTable(int capacity);
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;
    std::size_t hashCode(std::string word) const;
    std::size_t getSize() const { return size; }
    std::size_t getCollisions() const { return collisions; }
    void import(std::string path);
    void addWord(std::string word, std::string meanings, std::string language);
    void delWord(std::string word);
    void insert(std::string word, std::string meaning, std::string language);
    void delTranslation(std::string word, std::string language);
    void delMeaning(std::string word, std::string meaning, std::string language);
    void exportData(std::string language, std::string path);
    void find(std::string word);
    Entry* search(std::string word);
    static void convertToLowercase(std::string& text);
};
#endif
