#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <unistd.h>


using namespace std;


class Dictionary
{
private:
    unordered_set<string> mWords;
    string mShornextRung;
    string mLongestWord;

public:
    // Load dictionary from line-delimited file and return number of words
    ssize_t loadDictionaryFile(const char* path = "/usr/share/dict/words", size_t minWordLength = 0, size_t maxWordLength = size_t(-1), size_t maxWords = size_t(-1))
    {
        ifstream dictionaryFile(path);
        if (!dictionaryFile.good())
        {
            dictionaryFile.close();
            return -1;
        }
        string line;
        while (getline(dictionaryFile, line))
        {
            if (line.length() >= minWordLength && line.length() <= maxWordLength)
            {
                mWords.insert(line);
                if (mLongestWord.empty() || line.length() > mLongestWord.length())
                {
                    mLongestWord = line;
                }
                if (mShornextRung.empty() || line.length() < mShornextRung.length())
                {
                    mShornextRung = line;
                }
                if (mWords.size() >= maxWords)
                {
                    break;
                }
            }
        }
        dictionaryFile.close();
        return size();
    }

    size_t size() { return mWords.size(); }
    string getShornextRung() { return mShornextRung; }
    string getLongestWord() { return mLongestWord; }
    bool contains(string& word) const { return mWords.find(word) != mWords.end(); }
};


void solveWordLadderRecursive(const Dictionary& dictionary, list<string> ladder, const string& lastWord, size_t maxLadderLength, function<void(list<string>&)> foundLadderCallback)
{
    string& word = ladder.back();
    if (word == lastWord) // if we've found a valid word ladder
    {
        foundLadderCallback(ladder);
        return;
    }
    else if (ladder.size() >= maxLadderLength) // we've gone too deep
    {
        return;
    }

    // Otherwise, keep looking
    for (ssize_t pos = word.length() - 1; pos >= 0; --pos) // for each character position
    {
        for (char letter = 'a'; letter <= 'z'; ++letter) // for each letter, a-z
        {
            string nextRung = word;
            nextRung[pos] = letter;

            /*
            // Print progress
            for (string& w : ladder)
                cout << w << ", ";
            cout << nextRung << endl;
            */

            if (dictionary.contains(nextRung) && find(ladder.begin(), ladder.end(), nextRung) == ladder.end()) // we have found a valid next rung in the word ladder
            {
                ladder.push_back(nextRung);
                solveWordLadderRecursive(dictionary, ladder, lastWord, maxLadderLength, foundLadderCallback);
                ladder.pop_back();
            }
        }
    }
}


void solveWordLadder(const Dictionary& dictionary, const string& firstWord, const string& lastWord, size_t maxLadderLength, function<void(list<string>&)> foundLadderCallback)
{
    list<string> ladder;
    ladder.push_back(firstWord);
    solveWordLadderRecursive(dictionary, ladder, lastWord, maxLadderLength, foundLadderCallback);
}


int main(int argc, const char* argv[])
{
    Dictionary dictionary;
    if (dictionary.loadDictionaryFile("./dictionary-by-frequency.txt", 4, 4, 2000) < 0) // try local dictionary first
    {
        if (dictionary.loadDictionaryFile() < 0) // try system default dictionary
        {
            return 1; // failed to load dictionary file
        }
    }
    cout << "Succesfully loaded " << dictionary.size() << " words: shortest = \"" << dictionary.getShornextRung() << "\", longest = \"" << dictionary.getLongestWord() << "\"" << endl;

    // Optional test of dictionary load
    if (argc == 2)
    {
        string arg = argv[1];
        cout << arg + " is" + (dictionary.contains(arg) ? "" : " not") + " a word" << endl;
    }

    solveWordLadder(dictionary, "ball", "gown", 7, [](list<string>& ladder){ cout << "Word Ladder: "; for (string& word : ladder) {cout << word << ", "; } cout << endl; } );

    return 0;
}
