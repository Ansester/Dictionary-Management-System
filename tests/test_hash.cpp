#include "../hashtable.h"
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
int main() {
    std::ostringstream messages;
    auto* output = std::cout.rdbuf(messages.rdbuf());
    HashTable reused(7);
    reused.insert("a", "one", "German"); reused.insert("h", "two", "German");
    reused.delWord("a"); reused.insert("o", "three", "German");
    assert(reused.getSize() == 2 && reused.search("h") && reused.search("o") && !reused.search("a"));
    HashTable table(7);
    for (const char* key : {"a","h","o","v"}) table.insert(key,"meaning","German");
    table.delWord("a"); table.insert("}","replacement","german");
    assert(table.getSize() == 4 && table.search("}") && !table.search("a"));
    // Repeated insert/delete churn, resizing and long collision chains.
    for (int i = 0; i < 1000; ++i) table.insert("word"+std::to_string(i),"meaning","German");
    for (int i = 0; i < 1000; ++i) assert(table.search("word"+std::to_string(i)));
    for (int i = 0; i < 1000; ++i) table.delWord("word"+std::to_string(i));
    for (int i = 0; i < 1000; ++i) assert(!table.search("word"+std::to_string(i)));
    for (int i = 0; i < 1000; ++i) table.insert("replacement"+std::to_string(i),"meaning","German");
    for (int i = 0; i < 1000; ++i) assert(table.search("replacement"+std::to_string(i)));
    assert(table.getSize()==1004);
    std::cout.rdbuf(output);
    std::cout << "hash-table collision, tombstone, resize and 1000-key churn checks passed\n";
}
