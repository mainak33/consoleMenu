#pragma once
#include <string>
#include <string_view>
#include <iostream>
#include <functional>

namespace svUtils {
    using std::string;
    using std::string_view;
    using std::ostream;
    using std::function;
}

namespace svUtils{
    
    static const size_t defaultMaxLineLength = 80;

    struct SVSplit{
        string_view before;
        string_view after;
    };

    SVSplit splitByPosition(string_view sv, size_t pos) {
        return {
            sv.substr(0, pos),
            sv.substr(pos + 1,  sv.length() - pos - 1)
        };
    }

    SVSplit splitByDelimiter(string_view sv, const string_view delimiter = " ") {
        auto delimiterPosition = sv.find(delimiter);
        if (delimiterPosition == string::npos) {
            return {sv,{}};
        }else {
            return splitByPosition(sv, delimiterPosition);
        }
    }
   

    void wrapToLength(
        ostream& os, 
        string_view lines, 
        const string_view indent, 
        const size_t maxLineLength = defaultMaxLineLength, 
        const string_view delimiter = " ",
        const size_t startingPositionInLine = 0
    ) {
       
        // Lambda to add a word till the end respecting the indent and newlines
        function<void (ostream& , string_view, size_t, string_view)> 
        addWord = 
            [&indent, &maxLineLength]
            (ostream& os, string_view word, const size_t startingPositionInLine, const string_view delimiter) {
                auto wordSplitPosition = maxLineLength - startingPositionInLine;
                while (word.length() > 0) {
                    // Split to end of line
                    auto [lettersToAdd, restOfWord] = splitByPosition(word, wordSplitPosition);
                    // Check For newlines
                    auto [lettersBeforeNewLine, lettersAfterNewLine] = splitByDelimiter(word, "\n");
                    if (lettersBeforeNewLine.length() < lettersToAdd.length()) {
                        os << lettersBeforeNewLine << '\n' << indent;
                        word = lettersAfterNewLine;
                    }else {
                        os << lettersToAdd << '\n' << indent;
                        word = restOfWord;
                    }
                    wordSplitPosition = maxLineLength - indent.length();
                }
            };

        size_t indentLength = indent.length();
        size_t delimiterLength = delimiter.length();
        size_t currentPositionInLine{ startingPositionInLine };
        
        // Add Indent at the beginning
        addWord(os, indent, currentPositionInLine, delimiter);

        while (lines.length() > 0) {
            auto [currentWord, restOfLines] = splitByDelimiter(lines, delimiter);
            lines = restOfLines;
            if (currentPositionInLine + currentWord.length() > defaultMaxLineLength) {
                os << '\n' << indent;
                currentPositionInLine = indentLength + 1;
            }

            os << currentWord << delimiter;

            currentPositionInLine += currentWord.length() + delimiterLength;
        }
    }
    
}