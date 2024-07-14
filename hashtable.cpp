#include "hashtable.h"

//======================================================================================================================
// Translation Class
//======================================================================================================================
Translation::Translation(string meanings, string language) {
	this->language = language;  // Assigns the language to the instance variable
	this->meanings.push_back(meanings);  // Adds the meaning to the meanings vector
}

//---------------------------------------------------------------------------------

void Translation::addMeaning(string newMeanings) {
	HashTable::convertToLowercase(newMeanings);
	for (string& i : meanings) {
		HashTable::convertToLowercase(i);
		if (i == newMeanings) {
			return;  // If the meaning already exists, return without adding
		}
	}
	this->meanings.push_back(newMeanings);  // Adds new meaning to the meanings vector
}

//---------------------------------------------------------------------------------

void Translation::display() {
	cout << language << " : ";  // Displays the language followed by meanings
	for (size_t i = 0; i < meanings.size(); ++i) {
		cout << meanings[i];  // Prints each meaning
		if (i < meanings.size() - 1) cout << "; ";  // Adds semicolon between meanings
	}
	cout << endl;  // New line at the end of the display
}

//======================================================================================================================
// Entry Class
//======================================================================================================================
Entry::Entry(string word, string meanings, string language) {
	Translation newWord(meanings, language);  // Creates a new Translation object
	this->word = word;  // Sets the word for this entry
	this->translations.push_back(newWord);  // Adds the new translation to the vector
	deleted = false;  // Marks the entry as not deleted
}

//---------------------------------------------------------------------------------

void Entry::addTranslation(string newMeanings, string language) {
	for (Translation& trans : translations) {  // Iterates over all translations
		if (trans.language == language) {
			trans.addMeaning(newMeanings);  // Adds meaning to the existing translation
			return;
		}
	}
	translations.push_back(Translation(newMeanings, language));  // Adds a new translation if not found
}

//---------------------------------------------------------------------------------

void Entry::print() {
	for (Translation& i : translations) {
		i.display();  // Displays each translation
	}
}

//======================================================================================================================
// HashTable Class
//======================================================================================================================
HashTable::HashTable(int capacity) {
	buckets = new Entry*[capacity];  // Creates an array of Entry pointers
	for (int i = 0; i < capacity; i++)
		buckets[i] = nullptr;  // Initializes all bucket pointers to nullptr
	
	this->capacity = capacity;  // Sets the capacity of the hash table
	this->size = 0;  // Initializes the size of the hash table
	this->collisions = 0;  // Initializes the collision count
}

//---------------------------------------------------------------------------------

unsigned int HashTable::getSize() {
	return size;  // Returns the number of active entries
}

//---------------------------------------------------------------------------------

unsigned int HashTable::getCollisions() {
	return collisions;  // Returns the total number of collisions
}

//---------------------------------------------------------------------------------

//unsigned long HashTable::hashCode(string word) {
//	unsigned base = 31;  // The base for polynomial rolling hash function
//	unsigned long mod = 1e9 + 9;  // Large prime number for modulo operation
//	unsigned long hash = 0;  // Initialize hash to 0
//	unsigned long power_of_base = 1;  // Base raised to the power of 0 initially
//
//	for (char c : word) {
//		hash = (hash + (c - 'a' + 1) * power_of_base) % mod;  // Polynomial rolling hash calculation
//		power_of_base = (power_of_base * base) % mod;  // Update power_of_base
//	}
//
//	return hash % capacity;  // Return the hash value modulo the capacity
//}

//unsigned long HashTable::hashCode(string word){
//	unsigned long hash = 0;
//	for (char c : word) {
//		// Rotate hash to the left by 5 bits and then XOR with the character
//		hash = (hash << 5) | (hash >> (32 - 5));  // Perform bitwise rotation assuming 32-bit unsigned long
//		hash ^= c;  // XOR character into the hash value
//	}
//	return hash % capacity;  // Return the hash modulo the size of the hash table
//}


unsigned long HashTable::hashCode(string word) {
	unsigned long hash = 5381;  // Magic constant - a good prime number to start with
	for (char c : word) {
		// Hash * 33 + c
		hash = ((hash << 5) + hash) + c;  // hash * 33 + c, (hash << 5) + hash is a fast way to do hash * 33
	}
	return hash % capacity;  // Use modulo to ensure it fits within the table's capacity
}


//---------------------------------------------------------------------------------

void HashTable::insert(string word, string meanings, string language) {
	unsigned long hash_code = hashCode(word);  // Compute the hash code for the word
	unsigned long index = hash_code;  // Initialize the index for insertion
	unsigned long i = 0;  // Initialize probe sequence counter
	convertToLowercase(language);  // Convert the language to lowercase
	
	while (buckets[index] != nullptr && buckets[index]->word != word) {
		collisions++;  // Increment collision count
		i++;  // Increment probe counter
		index = (hash_code + i * i) % capacity;  // Compute next index using quadratic probing
		if (i == capacity) {  // If all slots have been probed
			cerr << "Hash table is full, unable to insert new entry." << endl;
			return;  // Exit if the table is full
		}
	}
	
	if (buckets[index] != nullptr && !buckets[index]->deleted && buckets[index]->word == word) {
		buckets[index]->addTranslation(meanings, language);  // Adds new translation to existing entry
		return;
	}
	
	if (buckets[index] == nullptr || buckets[index]->deleted) {
		if (buckets[index] && buckets[index]->deleted) {
			delete buckets[index];  // Free the memory if the slot was marked as deleted
		}
		buckets[index] = new Entry(word, meanings, language);  // Create a new entry in the hash table
		size++;  // Increment the size for the new entry
	}
}

//---------------------------------------------------------------------------------

void HashTable::addWord(string word, string meanings, string language) {
	convertToLowercase(language);  // Convert the language to lowercase
	
	// Split the meanings string into individual meanings using a semicolon as the delimiter
	istringstream meaningsStream(meanings);
	string singleMeaning;
	
	Entry* existingEntry = search(word);  // Search for the word in the hash table
	
	if (existingEntry != nullptr) {
		// If the word already exists, just add the new translations if they are unique
		while (getline(meaningsStream, singleMeaning, ';')) {
			// Directly handle leading and trailing whitespace
			singleMeaning.erase(0, singleMeaning.find_first_not_of(" \t\n\r\f\v"));  // Remove leading whitespace
			singleMeaning.erase(singleMeaning.find_last_not_of(" \t\n\r\f\v") + 1);  // Remove trailing whitespace
			convertToLowercase(singleMeaning);  // Convert each meaning to lowercase for consistent handling
			if (!singleMeaning.empty()) {
				existingEntry->addTranslation(singleMeaning, language);
			}
		}
		cout << word << " successfully updated in the dictionary.\n";
	} else {
		// If the word does not exist, insert it as a new entry
		bool first = true;
		while (getline(meaningsStream, singleMeaning, ';')) {
			// Handle leading and trailing whitespace
			singleMeaning.erase(0, singleMeaning.find_first_not_of(" \t\n\r\f\v"));
			singleMeaning.erase(singleMeaning.find_last_not_of(" \t\n\r\f\v") + 1);
			convertToLowercase(singleMeaning);  // Also convert to lowercase
			if (!singleMeaning.empty()) {
				if (first) {
					insert(word, singleMeaning, language);  // Insert the first meaning with the word
					first = false;
				} else {
					existingEntry = search(word);  // Make sure we have the entry
					if (existingEntry != nullptr) {
						existingEntry->addTranslation(singleMeaning, language);
					}
				}
			}
		}
		if (!first) {  // Check if at least one meaning was added
			cout << word << " successfully added to the dictionary.\n";
		} else {
			cout << "No valid meanings provided to add.\n";
		}
	}
}


//---------------------------------------------------------------------------------

void HashTable::delWord(string key) {
	unsigned long hash_code = hashCode(key);  // Compute hash code for the key
	unsigned long index = hash_code;  // Initialize index for removal
	int noOfComp = 0;  // Comparison counter
	
	do {
		Entry* currentEntry = buckets[index];
		if (currentEntry == nullptr) {
			cerr << "Entry not found: " << key << endl;  // Notify if entry not found
			return;  // Exit if slot is empty
		} else if (!currentEntry->deleted && currentEntry->word == key) {
			currentEntry->deleted = true;// Mark entry as deleted
			currentEntry->translations.clear();
			cout << key << " successfully deleted from the dictionary \n";
			size--;  // Decrement size since an entry is marked deleted
			return;
		}
		noOfComp++;  // Increment comparison counter
		index = (hash_code + noOfComp * noOfComp) % capacity;  // Quadratic probing for next index
	} while (index != hash_code);  // Loop until start index is reached again
	
	cerr << "Entry not found: " << key << endl;  // Notify if entry is not found after full loop
}

//---------------------------------------------------------------------------------

void HashTable::delTranslation(string word, string language) {
	convertToLowercase(language);  // Convert the language to lowercase
	Entry* foo = search(word);  // Search for the word
	if (foo != nullptr) {
		for (auto it = foo->translations.begin(); it != foo->translations.end(); it++) {
			if (it->language == language) {
				foo->translations.erase(it);  // Erase the translation
				cout << "Translation successfully deleted from the dictionary \n";
				return;  // Exit after deletion
			}
		}
		cerr << "No translations found \n";  // If no translations were found
	} else {
		cerr << "Word not found \n";  // If the word was not found
	}
}

//---------------------------------------------------------------------------------

Entry* HashTable::search(string key) {
	unsigned long hash_code = hashCode(key);  // Compute hash code for the key
	unsigned long index = hash_code;  // Initialize index for search
	int noOfComp = 0;  // Comparison counter
	
	do {
		Entry* currentEntry = buckets[index];
		if (currentEntry == nullptr) {
			return nullptr;  // Return not found if slot is empty
		} else if (!currentEntry->deleted && currentEntry->word == key) {
			return currentEntry;  // Return the entry if found and not deleted
		}
		noOfComp++;  // Increment comparison counter
		index = (hash_code + noOfComp * noOfComp) % capacity;  // Quadratic probing for next index
	} while (index != hash_code);  // Loop until start index is reached again
	
	return nullptr;  // Return not found if full loop completed without match
}

//---------------------------------------------------------------------------------

void HashTable::delMeaning(string word, string meaning, string language) {
	convertToLowercase(language);  // Convert the language to lowercase
	Entry* foo = search(word);  // Use search to find the entry
	if (foo != nullptr) {  // Check if the entry exists
		for (auto it = foo->translations.begin(); it != foo->translations.end(); ++it) {
			if (it->language == language) {  // Match the language
				for (auto loo = it->meanings.begin(); loo != it->meanings.end(); ) {  // Use a loop without increment here
					if (*loo == meaning) {
						loo = it->meanings.erase(loo);  // Erase and get the new iterator
						cout << "Meaning successfully deleted from the dictionary \n";
						return;  // Exit after deleting the meaning
					} else {
						++loo;  // Only increment if not erased
					}
				}
			}
		}
		cerr << "No such meanings found \n";  // If no such meanings were found
	} else {
		cerr << word << " not found in the dictionary\n";  // Print error if word not found
	}
}

//---------------------------------------------------------------------------------

void HashTable::find(string word) {
	unsigned long hash_code = hashCode(word);  // Compute hash code for the word
	unsigned long index = hash_code % capacity;  // Initialize index for search
	int noOfComp = 0;  // Comparison counter
	
	while (buckets[index] != nullptr && noOfComp < capacity) {  // Ensure slot isn't empty and not exceeding capacity
		Entry* currentEntry = buckets[index];
		noOfComp++;  // Increment comparison counter for each slot checked
		if (!currentEntry->deleted && currentEntry->word == word) {
			cout << "Word found: " << word << endl;
			currentEntry->print();  // Print the translations
			cout << "Comparisons made: " << noOfComp << endl;
			return;  // Exit after printing details
		}
		index = (hash_code + noOfComp * noOfComp) % capacity;  // Quadratic probing for next index
	}
	
	cout << "Word not found: " << word << endl;
	cout << "Comparisons made: " << noOfComp << endl;  // Print the total comparisons made even if not found
}

//---------------------------------------------------------------------------------

void HashTable::import(string path) {
	ifstream file(path);  // Open the file at the specified path
	if (!file.is_open()) {
		cerr << "Failed to open file: " << path << endl;
		return;
	}
	
	string language;
	if (!getline(file, language)) {  // Read the first line to get the language
		cerr << "Failed to read the language from the file: " << path << endl;
		file.close();
		return;
	}
	
	string line;
	while (getline(file, line)) {  // Read each subsequent line from the file
		size_t colonPos = line.find(':');  // Find the position of the colon
		if (colonPos == string::npos) {
			cerr << "Invalid format in line, no colon found: " << line << endl;
			continue;
		}
		
		string word = line.substr(0, colonPos);  // Extract the word
		string meaningsStr = line.substr(colonPos + 1);  // Extract the meanings string
		
		stringstream ss(meaningsStr);
		string meaning;
		bool isFirstMeaning = true;
		
		while (getline(ss, meaning, ';')) {  // Split meanings by semicolon
			if (isFirstMeaning) {
				// Insert the first translation and create the Entry
				insert(word, meaning, language);
				isFirstMeaning = false;
			} else {
				// For additional meanings, we also use insert as it should handle adding to existing entries
				insert(word, meaning, language);
			}
		}
	}
	
	file.close();  // Close the file
}

//---------------------------------------------------------------------------------

void HashTable::exportData(string language, string filePath) {
	// Ensure directory exists, omitted for brevity - please implement this separately
	ofstream file(filePath);  // Open file for writing
	if (!file.is_open()) {
		cerr << "Failed to open file for writing: " << filePath << endl;
		return;
	}
	
	file << language << endl;
	
	unsigned int entriesExported = 0;  // Initialize counter for exported entries
	
	for (unsigned int i = 0; i < capacity; ++i) {  // Iterate over all buckets
		if (buckets[i] != nullptr && !buckets[i]->deleted) {  // Check if the bucket is active
			for (Translation& translation : buckets[i]->translations) {
				if (translation.language == language) {  // Check if the translation is in the desired language
					// Write the word followed by its translations
					file << buckets[i]->word << ':';  // Note the use of ':' instead of " : "
					bool firstMeaning = true;
					for (const string& meaning : translation.meanings) {
						if (!firstMeaning) {
							file << ';';  // Separate meanings with a semicolon
						}
						file << meaning;
						firstMeaning = false;
					}
					entriesExported++;
					file << endl;  // New line for next word
					
				}
			}
		}
	}
	
	// Optionally, print how many entries were exported for debugging
	cout << "Total entries exported: " << entriesExported << endl;
	
	file.close();  // Close the file
}

//---------------------------------------------------------------------------------

HashTable::~HashTable() {
	for (unsigned int i = 0; i < capacity; ++i) {
		if (buckets[i] != nullptr) {  // Check if the bucket is not null
			delete buckets[i];  // Delete the entry object to free the memory
			buckets[i] = nullptr;  // Set the pointer to nullptr to avoid dangling pointer issues
		}
	}
	delete[] buckets;  // Delete the array of pointers
	buckets = nullptr;  // Set the pointer to nullptr to avoid dangling pointer issues
}

//---------------------------------------------------------------------------------

void HashTable::convertToLowercase(string &str) {
	transform(str.begin(), str.end(), str.begin(),
			  [](unsigned char c) { return tolower(c); });  // Convert string to lowercase using transform
}

//---------------------------------------------------------------------------------
