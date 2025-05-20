#include "dictionary.h"
#include "logger.h"
#include <cctype>
#include <locale>
#include <algorithm>
#include <QFileInfo>

using namespace std;

Dictionary::Dictionary() {
    Logger::log(Logger::Info, "Dictionary created");
}

Dictionary::~Dictionary() {
    Logger::log(Logger::Info, "Dictionary destroyed");
}

void Dictionary::addWord(const string& word) {
    if (word.empty()) return;

    string normalizedWord = normalizeWord(word);
    if (!normalizedWord.empty()) {
        wordMap[normalizedWord]++;
        Logger::log(Logger::Debug, "Added word: " + normalizedWord);
    }
}

bool Dictionary::addWordsFromFile(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable()) {
        Logger::log(Logger::Error, "Cannot open file: " + filePath.toStdString());
        return false;
    }

    try {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Logger::log(Logger::Error, "Failed to open file: " + filePath.toStdString());
            return false;
        }

        QTextStream in(&file);
        QString line;
        int wordCount = 0;

        while (!in.atEnd()) {
            line = in.readLine();
            istringstream iss(line.toStdString());
            string word;

            while (iss >> word) {
                addWord(word);
                wordCount++;
            }
        }

        file.close();
        Logger::log(Logger::Info, "File processed: " + filePath.toStdString() +
                   ", words added: " + to_string(wordCount));
        return true;
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Exception while reading file: " + string(e.what()));
        return false;
    }
}

bool Dictionary::saveToFile(const QString& filePath) {
    try {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            Logger::log(Logger::Error, "Failed to save dictionary to file: " +
                       filePath.toStdString());
            return false;
        }

        QTextStream out(&file);

        for (const auto& [word, count] : wordMap) {
            out << QString::fromStdString(word) << " " << count << Qt::endl;
        }

        file.close();
        Logger::log(Logger::Info, "Dictionary saved to file: " + filePath.toStdString() +
                   ", total words: " + to_string(wordMap.size()));
        return true;
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Exception while saving dictionary: " + string(e.what()));
        return false;
    }
}

bool Dictionary::loadFromFile(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable()) {
        Logger::log(Logger::Error, "Cannot open dictionary file: " + filePath.toStdString());
        return false;
    }

    try {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Logger::log(Logger::Error, "Failed to open dictionary file: " + filePath.toStdString());
            return false;
        }

        clear();

        QTextStream in(&file);
        QString line;
        int wordCount = 0;

        while (!in.atEnd()) {
            line = in.readLine();
            istringstream iss(line.toStdString());
            string word;
            int count = 0;

            if (iss >> word >> count) {
                wordMap[word] = count;
                wordCount++;
            }
        }

        file.close();
        Logger::log(Logger::Info, "Dictionary loaded from file: " + filePath.toStdString() +
                   ", total words: " + to_string(wordCount));
        return true;
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Exception while loading dictionary: " + string(e.what()));
        return false;
    }
}

vector<pair<string, int>> Dictionary::getWordsAlphabetically() const {
    vector<pair<string, int>> words(wordMap.begin(), wordMap.end());
    sort(words.begin(), words.end(),
         [](const auto& a, const auto& b) { return a.first < b.first; });

    Logger::log(Logger::Debug, "Retrieved alphabetically sorted word list");
    return words;
}

vector<pair<string, int>> Dictionary::getWordsByFrequency() const {
    vector<pair<string, int>> words(wordMap.begin(), wordMap.end());
    sort(words.begin(), words.end(),
         [](const auto& a, const auto& b) {
             return a.second > b.second || (a.second == b.second && a.first < b.first);
         });

    Logger::log(Logger::Debug, "Retrieved frequency sorted word list");
    return words;
}

void Dictionary::clear() {
    size_t oldSize = wordMap.size();
    wordMap.clear();
    Logger::log(Logger::Info, "Dictionary cleared, previous size: " + to_string(oldSize));
}

size_t Dictionary::size() const {
    return wordMap.size();
}

string Dictionary::normalizeWord(const string& word) {
    string result;
    result.reserve(word.size());

    for (char c : word) {
        if (isalpha(c, locale()) || isdigit(c) || c == '_') {
            if (isalpha(c, locale())) {
                result.push_back(tolower(c, locale()));
            } else {
                result.push_back(c);
            }
        }
    }

    return result;
}
