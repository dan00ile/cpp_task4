#include "gtest/gtest.h"
#include "../dictionary.h"
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

using namespace std;

class MockMainWindow {
public:
    MockMainWindow() {
        dictionary = new Dictionary();
    }

    ~MockMainWindow() {
        delete dictionary;
    }

    bool loadWordsFromFile(const QString& filePath) {
        bool result = dictionary->addWordsFromFile(filePath);
        if (result) {
            updateState();
        }
        return result;
    }

    bool saveDictionaryToFile(const QString& filePath) {
        if (dictionary->size() == 0) {
            return false;
        }
        return dictionary->saveToFile(filePath);
    }

    bool loadDictionaryFromFile(const QString& filePath, bool replace) {
        if (replace) {
            return dictionary->loadFromFile(filePath);
        } else {
            Dictionary tempDict;
            if (!tempDict.loadFromFile(filePath)) {
                return false;
            }

            auto words = tempDict.getWordsAlphabetically();
            for (const auto& [word, count] : words) {
                for (int i = 0; i < count; i++) {
                    dictionary->addWord(word);
                }
            }

            return true;
        }
    }

    void clearDictionary() {
        dictionary->clear();
        updateState();
    }

    vector<pair<string, int>> sortAlphabetically() {
        currentOrder = dictionary->getWordsAlphabetically();
        return currentOrder;
    }

    vector<pair<string, int>> sortByFrequency() {
        currentOrder = dictionary->getWordsByFrequency();
        return currentOrder;
    }

    Dictionary* getDictionary() {
        return dictionary;
    }

    const vector<pair<string, int>>& getCurrentOrder() const {
        return currentOrder;
    }

private:
    Dictionary* dictionary;
    vector<pair<string, int>> currentOrder;

    void updateState() {
        currentOrder = dictionary->getWordsAlphabetically();
    }
};

class MockMainWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = new QTemporaryDir();
        ASSERT_TRUE(tempDir->isValid());
        mockWindow = new MockMainWindow();
    }

    void TearDown() override {
        delete mockWindow;
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

    MockMainWindow* mockWindow;
    QTemporaryDir* tempDir;
};

TEST_F(MockMainWindowTest, LoadWordsFromFile) {
    QString filePath = createTempTextFile("test1 test2\ntest3 test1");
    ASSERT_FALSE(filePath.isEmpty());

    bool result = mockWindow->loadWordsFromFile(filePath);
    EXPECT_TRUE(result);

    EXPECT_EQ(mockWindow->getDictionary()->size(), 3);

    const auto& currentWords = mockWindow->getCurrentOrder();
    EXPECT_EQ(currentWords.size(), 3);

    EXPECT_EQ(currentWords[0].first, "test1");
    EXPECT_EQ(currentWords[1].first, "test2");
    EXPECT_EQ(currentWords[2].first, "test3");

    for (const auto& pair : currentWords) {
        if (pair.first == "test1") {
            EXPECT_EQ(pair.second, 2);
        } else {
            EXPECT_EQ(pair.second, 1);
        }
    }
}

TEST_F(MockMainWindowTest, SaveAndLoadDictionary) {
    QString textFilePath = createTempTextFile("test1 test2\ntest3 test1");
    ASSERT_FALSE(textFilePath.isEmpty());

    ASSERT_TRUE(mockWindow->loadWordsFromFile(textFilePath));

    QString dictFilePath = tempDir->path() + "/save_test.dict";
    bool saveResult = mockWindow->saveDictionaryToFile(dictFilePath);
    EXPECT_TRUE(saveResult);

    mockWindow->clearDictionary();
    EXPECT_EQ(mockWindow->getDictionary()->size(), 0);

    bool loadResult = mockWindow->loadDictionaryFromFile(dictFilePath, true);
    EXPECT_TRUE(loadResult);

    EXPECT_EQ(mockWindow->getDictionary()->size(), 3);
}

TEST_F(MockMainWindowTest, ClearDictionary) {
    QString filePath = createTempTextFile("test1 test2\ntest3 test1");
    ASSERT_TRUE(mockWindow->loadWordsFromFile(filePath));
    EXPECT_EQ(mockWindow->getDictionary()->size(), 3);

    mockWindow->clearDictionary();
    EXPECT_EQ(mockWindow->getDictionary()->size(), 0);
    EXPECT_TRUE(mockWindow->getCurrentOrder().empty());
}

TEST_F(MockMainWindowTest, SortByFrequency) {
    QString filePath = createTempTextFile("rare common common very_common very_common very_common");
    ASSERT_TRUE(mockWindow->loadWordsFromFile(filePath));

    auto words = mockWindow->sortByFrequency();

    EXPECT_EQ(words.size(), 3);
    EXPECT_EQ(words[0].first, "very_common");
    EXPECT_EQ(words[0].second, 3);

    EXPECT_EQ(words[1].first, "common");
    EXPECT_EQ(words[1].second, 2);

    EXPECT_EQ(words[2].first, "rare");
    EXPECT_EQ(words[2].second, 1);
}

TEST_F(MockMainWindowTest, AddWordsFromAnotherDictionary) {
    QString dictContent = "word1 2\nword2 1\n";
    QString dictFilePath = createTempDictFile(dictContent);
    ASSERT_FALSE(dictFilePath.isEmpty());

    ASSERT_TRUE(mockWindow->loadDictionaryFromFile(dictFilePath, true));
    EXPECT_EQ(mockWindow->getDictionary()->size(), 2);

    QString dictContent2 = "word3 1\nword1 3\n";
    QString dictFilePath2 = tempDir->path() + "/another_dict.dict";
    QFile file(dictFilePath2);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&file);
    out << dictContent2;
    file.close();

    ASSERT_TRUE(mockWindow->loadDictionaryFromFile(dictFilePath2, false));
    EXPECT_EQ(mockWindow->getDictionary()->size(), 3);

    auto words = mockWindow->sortAlphabetically();
    for (const auto& pair : words) {
        if (pair.first == "word1") {
            EXPECT_EQ(pair.second, 5);
        } else if (pair.first == "word2") {
            EXPECT_EQ(pair.second, 1);
        } else if (pair.first == "word3") {
            EXPECT_EQ(pair.second, 1);
        }
    }
}
