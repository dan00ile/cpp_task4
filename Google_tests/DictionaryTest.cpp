#include "gtest/gtest.h"
#include "../dictionary.h"
#include <fstream>
#include <QTemporaryFile>
#include <QTemporaryDir>

class DictionaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = new QTemporaryDir();
        ASSERT_TRUE(tempDir->isValid());

        dict = new Dictionary();
    }

    void TearDown() override {
        delete dict;
        delete tempDir;
    }

    QString createTempTextFile(const QString& content) {
        QString filePath = tempDir->path() + "/test_text.txt";
        QFile file(filePath);
        
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << content;
            file.close();
            return filePath;
        }
        
        return QString();
    }

    QString createTempDictFile(const QString& content) {
        QString filePath = tempDir->path() + "/test_dict.dict";
        QFile file(filePath);
        
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << content;
            file.close();
            return filePath;
        }
        
        return QString();
    }

    Dictionary* dict{};
    QTemporaryDir* tempDir{};
};

TEST_F(DictionaryTest, AddWord) {
    dict->addWord("test");
    EXPECT_EQ(dict->size(), 1);

    auto words = dict->getWordsAlphabetically();
    EXPECT_EQ(words.size(), 1);
    EXPECT_EQ(words[0].first, "test");
    EXPECT_EQ(words[0].second, 1);

    dict->addWord("test");
    words = dict->getWordsAlphabetically();
    EXPECT_EQ(words.size(), 1);
    EXPECT_EQ(words[0].second, 2);

    dict->addWord("another");
    EXPECT_EQ(dict->size(), 2);

    words = dict->getWordsAlphabetically();
    EXPECT_EQ(words.size(), 2);
    EXPECT_EQ(words[0].first, "another");
    EXPECT_EQ(words[1].first, "test");
}

TEST_F(DictionaryTest, AddWordsFromFile) {
    QString filePath = createTempTextFile("word1 word2 word3\nword1 word4");
    ASSERT_FALSE(filePath.isEmpty());

    bool result = dict->addWordsFromFile(filePath);
    EXPECT_TRUE(result);

    EXPECT_EQ(dict->size(), 4);
    
    auto words = dict->getWordsAlphabetically();
    EXPECT_EQ(words.size(), 4);

    for (const auto& pair : words) {
        if (pair.first == "word1") {
            EXPECT_EQ(pair.second, 2);
        } else {
            EXPECT_EQ(pair.second, 1);
        }
    }
}

TEST_F(DictionaryTest, SaveAndLoadDictionary) {
    dict->addWord("word1");
    dict->addWord("word1");
    dict->addWord("word2");

    QString filePath = tempDir->path() + "/test_save.dict";
    bool saveResult = dict->saveToFile(filePath);
    EXPECT_TRUE(saveResult);

    Dictionary loadedDict;
    bool loadResult = loadedDict.loadFromFile(filePath);
    EXPECT_TRUE(loadResult);

    EXPECT_EQ(loadedDict.size(), 2);
    
    auto words = loadedDict.getWordsAlphabetically();
    for (const auto& pair : words) {
        if (pair.first == "word1") {
            EXPECT_EQ(pair.second, 2);
        } else if (pair.first == "word2") {
            EXPECT_EQ(pair.second, 1);
        }
    }
}

TEST_F(DictionaryTest, LoadFromNonExistentFile) {
    bool result = dict->loadFromFile("/non/existent/file.dict");
    EXPECT_FALSE(result);
    EXPECT_EQ(dict->size(), 0);
}

TEST_F(DictionaryTest, ClearDictionary) {
    dict->addWord("word1");
    dict->addWord("word2");
    EXPECT_EQ(dict->size(), 2);

    dict->clear();
    EXPECT_EQ(dict->size(), 0);
}

TEST_F(DictionaryTest, SortByFrequency) {
    dict->addWord("rare");
    dict->addWord("common");
    dict->addWord("common");
    dict->addWord("very_common");
    dict->addWord("very_common");
    dict->addWord("very_common");
    
    auto words = dict->getWordsByFrequency();
    EXPECT_EQ(words.size(), 3);

    EXPECT_EQ(words[0].first, "very_common");
    EXPECT_EQ(words[0].second, 3);
    
    EXPECT_EQ(words[1].first, "common");
    EXPECT_EQ(words[1].second, 2);
    
    EXPECT_EQ(words[2].first, "rare");
    EXPECT_EQ(words[2].second, 1);
}

TEST_F(DictionaryTest, EmptyString) {
    dict->addWord("");
    EXPECT_EQ(dict->size(), 0);
}

TEST_F(DictionaryTest, OnlySymbols) {
    dict->addWord("!@#$%^&*()");
    EXPECT_EQ(dict->size(), 0);
} 