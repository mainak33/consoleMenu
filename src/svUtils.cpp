#pragma once
#include "svUtils.h"
#include <string>
#include <iostream>
#include <tuple>

namespace svUtils {
    using std::string;
    using std::make_tuple;
}

using namespace svUtils;

SVSplit svUtils::splitByPosition(
    string_view sv, 
    size_t pos
) {
    auto len = sv.length();
    if (0 == len) return make_tuple(string_view{}, string_view{});
    if (pos < 0) return make_tuple(string_view{}, sv);
    if (0 == pos) return make_tuple(string_view{}, sv.substr(1));
    if (pos >= len) return make_tuple(sv, string_view{});
    return make_tuple(
        sv.substr(0, pos),
        sv.substr(pos + 1, sv.length() - pos - 1)
    );
}

SVSplit svUtils::splitByDelimiter(
    string_view sv, 
    const string_view delimiter
) {
    auto delimiterPosition = sv.find(delimiter);
    if (delimiterPosition == string::npos) {
        return make_tuple(sv, string_view{});
    }
    else {
        return splitByPosition(sv, delimiterPosition);
    }
}


void svUtils::wrapToLength(
    ostream& os,
    string_view lines,
    const LineOptions& lineOptions
) {
    auto indent{lineOptions.indent};
    auto maxLineLength{lineOptions.maxLineLength};
    auto delimiter{lineOptions.delimiter};
    auto startingPositionInLine{lineOptions.startingPositionInLine };

    //Adjust indent side if longer than line length
    if(indent.length() >= maxLineLength) indent = indent.substr(0, maxLineLength-1);
    // Other Initializations
    size_t indentLength = indent.length();
    size_t delimiterLength = delimiter.length();

    // Lambda to add a word till the end respecting the indent and newlines
    auto addWord =
        [&indent, &maxLineLength, &indentLength]
        (ostream& os, string_view word, size_t& currentPositionInLine) {
        string_view lettersToAdd, restOfWord;
        auto remainingWordLength = word.length();
        while (remainingWordLength > 0) {
            // Check if Word is longer than remaining line
            auto relativeLineEndPosition = maxLineLength - currentPositionInLine; // remaining characters in Line
            if (relativeLineEndPosition == 0) {
                os << '\n' << indent;
                currentPositionInLine = indentLength;
                continue;
            }else if (remainingWordLength <= relativeLineEndPosition) {
                // remaining word is shorter than line
                lettersToAdd = word;
                restOfWord = {};
            }
            else {
                // remaining word is longer than line. Keep letters till end of line
                lettersToAdd = word.substr(0, relativeLineEndPosition);
                restOfWord = word.substr(relativeLineEndPosition);
            }
            // Check for newlines in lettersToAdd
            auto [lettersBeforeNewLine, lettersAfterNewLine] = splitByDelimiter(word, "\n");
            if (lettersBeforeNewLine.length() < lettersToAdd.length()) {
                // If new line is within lettersToAdd. Split at the newline character
                os << lettersBeforeNewLine << '\n' << indent;
                currentPositionInLine = indentLength;
                word = lettersAfterNewLine;
            }
            else {
                // If no new line within letters to add, just add the letters
                os << lettersToAdd;
                currentPositionInLine += lettersToAdd.length();
                word = restOfWord;
            }
            remainingWordLength = word.length();
        }
    };


    // Add Indent at the beginning
    size_t currentPositionInLine{ startingPositionInLine % maxLineLength };
    string_view currentWord{indent}, restOfLines{lines};
 
    addWord(os, currentWord, currentPositionInLine);

    while (restOfLines.length() > 0) {
        std::tie(currentWord, restOfLines) = splitByDelimiter(restOfLines, delimiter);

        // Add Word
        addWord(os, currentWord, currentPositionInLine);
        
        // If delimiter is at start of line skip it
        if (currentPositionInLine == indentLength) continue;
        // If delimiter is at end of line skip it
        if (currentPositionInLine == maxLineLength) continue;
        // If this is the last word don't add delimiter
        if (restOfLines.length() == 0) continue;
        // Add Delimiter
        addWord(os, delimiter, currentPositionInLine);
    }
}
