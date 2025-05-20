#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <QString>
#include <QFile>
#include <QTextStream>

using namespace std;

class Dictionary {
public:
    Dictionary();
    ~Dictionary();

    void addWord(const string& word);

    bool addWordsFromFile(const QString& filePath);

    bool saveToFile(const QString& filePath);

    bool loadFromFile(const QString& filePath);

    vector<pair<string, int>> getWordsAlphabetically() const;

    vector<pair<string, int>> getWordsByFrequency() const;

    void clear();

    size_t size() const;

private:
    map<string, int> wordMap;

    string normalizeWord(const string& word);
};

#endif // DICTIONARY_H 