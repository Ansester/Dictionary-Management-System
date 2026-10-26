// Original command-driver header credited Khalid Mengal, version 1.0.
// Coursework repository maintained by Ashmit Mukherjee; see README provenance.
#include "hashtable.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
void help() {
    std::cout << "find <word>\nimport <path>\nexport <language,filename>\n"
                 "add <word,meaning(s),language> (separate meanings with ;)\n"
                 "delTranslation <word,language>\ndelMeaning <word,meaning,language>\ndelWord <word>\nhelp\nexit\n";
}
int main() {
    HashTable dictionary(101);
    try { dictionary.import("en-de.txt"); }
    catch (const std::exception& error) { std::cout << "Notice: " << error.what() << "; starting with an empty dictionary.\n"; }
    std::cout << "Size of HashTable = " << dictionary.getSize() << '\n'; help();
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        try {
            std::istringstream input(line); std::string command, arguments;
            if (!(input >> command)) continue;
            std::getline(input >> std::ws, arguments);
            if (command == "exit" || command == "help") {
                if (!arguments.empty()) throw std::runtime_error("Unexpected arguments");
                if (command == "exit") return 0;
                help(); continue;
            }
            if (command == "find" || command == "delWord" || command == "import") {
                if (arguments.empty()) throw std::runtime_error("Missing argument");
                if (command == "find") dictionary.find(arguments);
                else if (command == "delWord") dictionary.delWord(arguments);
                else dictionary.import(arguments);
                continue;
            }
            std::vector<std::string> parts;
            std::istringstream separated(arguments); std::string part;
            while (std::getline(separated, part, ',')) parts.push_back(part);
            const std::size_t expected = (command == "add" || command == "delMeaning") ? 3 : 2;
            if (parts.size() != expected) throw std::runtime_error("Wrong number of comma-separated arguments");
            if (command == "add") dictionary.addWord(parts[0], parts[1], parts[2]);
            else if (command == "delTranslation") dictionary.delTranslation(parts[0],parts[1]);
            else if (command == "delMeaning") dictionary.delMeaning(parts[0],parts[1],parts[2]);
            else if (command == "export") dictionary.exportData(parts[0],parts[1]);
            else throw std::runtime_error("Unknown command; enter help");
        } catch (const std::exception& error) { std::cout << "Error: " << error.what() << '\n'; }
    }
}
