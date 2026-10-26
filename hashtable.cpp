#include "hashtable.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <utility>
namespace {
std::string trim(std::string text) {
    const auto start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return {};
    return text.substr(start, text.find_last_not_of(" \t\r\n") - start + 1);
}
std::string languageName(std::string text) {
    text = trim(std::move(text)); HashTable::convertToLowercase(text);
    if (text.empty()) throw std::runtime_error("Language cannot be empty");
    return text;
}
std::vector<std::string> splitMeanings(const std::string& text) {
    std::vector<std::string> result;
    std::istringstream input(text); std::string meaning;
    while (std::getline(input, meaning, ';')) {
        meaning = trim(std::move(meaning));
        if (!meaning.empty()) result.push_back(std::move(meaning));
    }
    if (result.empty()) throw std::runtime_error("Provide at least one meaning");
    return result;
}
bool prime(std::size_t n) {
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;
    for (std::size_t d = 3; d <= n / d; d += 2) if (n % d == 0) return false;
    return true;
}
std::size_t nextPrime(std::size_t n) {
    n = std::max<std::size_t>(n, 7);
    while (!prime(n)) {
        if (n == std::numeric_limits<std::size_t>::max()) throw std::length_error("Hash table capacity overflow");
        ++n;
    }
    return n;
}
// a and b are reduced modulo m: avoid overflowing even for a long probe chain.
std::size_t addMod(std::size_t a, std::size_t b, std::size_t m) {
    return a >= m - b ? a - (m - b) : a + b;
}
}
Translation::Translation(std::string meaning, std::string language) : language(std::move(language)) {
    meanings.push_back(std::move(meaning));
}
void Translation::addMeaning(std::string meaning) {
    if (std::find(meanings.begin(), meanings.end(), meaning) == meanings.end()) meanings.push_back(std::move(meaning));
}
void Translation::display() const {
    std::cout << language << " : ";
    for (std::size_t i = 0; i < meanings.size(); ++i) std::cout << (i ? "; " : "") << meanings[i];
    std::cout << '\n';
}
Entry::Entry(std::string word, std::string meaning, std::string language) : word(std::move(word)) {
    translations.emplace_back(std::move(meaning), std::move(language));
}
void Entry::addTranslation(std::string meaning, std::string language) {
    for (auto& translation : translations) if (translation.language == language) {
        translation.addMeaning(std::move(meaning)); return;
    }
    translations.emplace_back(std::move(meaning), std::move(language));
}
void Entry::print() const { for (const auto& translation : translations) translation.display(); }
HashTable::HashTable(int capacity) {
    if (capacity <= 0) throw std::invalid_argument("Capacity must be positive");
    buckets.resize(nextPrime(static_cast<std::size_t>(capacity)));
}
std::size_t HashTable::hashCode(std::string word) const {
    // djb2 recurrence; unsigned overflow is intentional and well-defined.
    std::size_t hash = 5381;
    for (unsigned char character : word) hash = hash * 33 + character;
    return hash % buckets.size();
}
std::size_t HashTable::locate(const std::string& word, bool countCollisions) {
    std::size_t index = hashCode(word), step = 1, tombstone = buckets.size();
    for (std::size_t attempt = 0; attempt < buckets.size(); ++attempt) {
        const auto& entry = buckets[index];
        if (!entry) return tombstone != buckets.size() ? tombstone : index;
        if (entry->deleted) {
            if (tombstone == buckets.size()) tombstone = index;
        } else if (entry->word == word) return index;
        if (countCollisions) ++collisions;
        index = addMod(index, step, buckets.size());
        step = addMod(step, 2, buckets.size()); // successive odd steps produce h+i^2.
    }
    return tombstone;
}
void HashTable::rehash(std::size_t capacity) {
    std::vector<std::unique_ptr<Entry>> replacement(nextPrime(capacity));
    auto old = std::move(buckets); buckets = std::move(replacement);
    size = used = 0;
    for (auto& entry : old) if (entry && !entry->deleted) {
        const auto index = locate(entry->word, false);
        buckets[index] = std::move(entry); ++size; ++used;
    }
}
Entry* HashTable::search(std::string word) {
    word = trim(std::move(word));
    const auto index = locate(word, false);
    return index != buckets.size() && buckets[index] && !buckets[index]->deleted && buckets[index]->word == word
        ? buckets[index].get() : nullptr;
}
void HashTable::insert(std::string word, std::string meaning, std::string language) {
    word = trim(std::move(word)); meaning = trim(std::move(meaning)); language = languageName(std::move(language));
    if (word.empty() || word.find_first_of(":;\n\r") != std::string::npos || meaning.empty() ||
        meaning.find_first_of(";\n\r") != std::string::npos || language.find_first_of(";,:\n\r") != std::string::npos)
        throw std::runtime_error("Word, meaning or language is empty or contains a file-format delimiter");
    auto index = locate(word, false);
    if (index != buckets.size() && buckets[index] && !buckets[index]->deleted && buckets[index]->word == word) {
        buckets[index]->addTranslation(std::move(meaning), std::move(language)); return;
    }
    // Keeping occupied slots (including tombstones) below half a prime capacity
    // guarantees an empty quadratic-probe position. Rebuild tombstones or grow.
    if (used + 1 > buckets.size() / 2) {
        std::size_t capacity = buckets.size();
        if (size + 1 > capacity / 2) {
            if (capacity > (std::numeric_limits<std::size_t>::max() - 1) / 2) throw std::length_error("Hash table capacity overflow");
            capacity = capacity * 2 + 1;
        }
        rehash(capacity);
    }
    index = locate(word, true);
    if (index == buckets.size()) throw std::runtime_error("No available hash-table probe position");
    auto entry = std::make_unique<Entry>(std::move(word), std::move(meaning), std::move(language));
    if (!buckets[index]) ++used;
    buckets[index] = std::move(entry); ++size;
}
void HashTable::addWord(std::string word, std::string meanings, std::string language) {
    const auto parts = splitMeanings(meanings);
    for (const auto& meaning : parts) insert(word, meaning, language);
    std::cout << trim(word) << " successfully added/updated in the dictionary.\n";
}
void HashTable::delWord(std::string word) {
    auto* entry = search(word);
    if (!entry) { std::cout << "Word not found: " << word << '\n'; return; }
    entry->deleted = true; entry->translations.clear(); --size;
    std::cout << trim(word) << " successfully deleted from the dictionary.\n";
}
void HashTable::delTranslation(std::string word, std::string language) {
    language = languageName(std::move(language)); auto* entry = search(word);
    if (entry) for (auto it = entry->translations.begin(); it != entry->translations.end(); ++it)
        if (it->language == language) { entry->translations.erase(it); std::cout << "Translation deleted.\n"; return; }
    std::cout << "Translation not found.\n";
}
void HashTable::delMeaning(std::string word, std::string meaning, std::string language) {
    language = languageName(std::move(language)); meaning = trim(std::move(meaning)); auto* entry = search(word);
    if (entry) for (auto& translation : entry->translations) if (translation.language == language) {
        const auto it = std::find(translation.meanings.begin(), translation.meanings.end(), meaning);
        if (it != translation.meanings.end()) {
            translation.meanings.erase(it);
            // A language with no meanings is removed, keeping exports round-trippable.
            if (translation.meanings.empty()) {
                entry->translations.erase(std::remove_if(entry->translations.begin(), entry->translations.end(),
                    [](const Translation& value) { return value.meanings.empty(); }), entry->translations.end());
            }
            std::cout << "Meaning deleted.\n"; return;
        }
    }
    std::cout << "Meaning not found.\n";
}
void HashTable::find(std::string word) {
    if (auto* entry = search(word)) { std::cout << "Word found: " << trim(word) << '\n'; entry->print(); }
    else std::cout << "Word not found: " << trim(word) << '\n';
}
void HashTable::import(std::string path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot open dictionary file: " + path);
    std::string language, line;
    if (!std::getline(file, language)) throw std::runtime_error("Missing dictionary language header");
    language = languageName(std::move(language));
    std::vector<std::pair<std::string, std::string>> records;
    while (std::getline(file, line)) {
        if (trim(line).empty()) continue;
        const auto colon = line.find(':');
        if (colon == std::string::npos || trim(line.substr(0,colon)).empty())
            throw std::runtime_error("Invalid dictionary row: " + line);
        const auto word = trim(line.substr(0, colon));
        if (word.find(';') != std::string::npos) throw std::runtime_error("Invalid dictionary word");
        for (const auto& meaning : splitMeanings(line.substr(colon + 1))) records.emplace_back(word, meaning);
    }
    if (!file.eof()) throw std::runtime_error("Error reading dictionary file");
    for (const auto& [word, meaning] : records) insert(word, meaning, language);
}
void HashTable::exportData(std::string language, std::string path) {
    language = languageName(std::move(language));
    std::ofstream file(path);
    if (!file) throw std::runtime_error("Cannot open export file: " + path);
    file << language << '\n'; std::size_t exported = 0;
    for (const auto& entry : buckets) if (entry && !entry->deleted)
        for (const auto& translation : entry->translations) if (translation.language == language && !translation.meanings.empty()) {
            file << entry->word << ':';
            for (std::size_t i = 0; i < translation.meanings.size(); ++i) file << (i ? ";" : "") << translation.meanings[i];
            file << '\n'; ++exported;
        }
    file.close(); if (!file) throw std::runtime_error("Error writing dictionary file");
    std::cout << "Total entries exported: " << exported << '\n';
}
void HashTable::convertToLowercase(std::string& text) {
    // Normalize ASCII language labels only; retain spelling and UTF-8 bytes in entries.
    for (char& character : text) if (character >= 'A' && character <= 'Z') character += 'a' - 'A';
}
