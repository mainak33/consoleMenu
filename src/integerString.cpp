#include "integerString.h"

#include <iterator>
#include <unordered_map>
#include <stdexcept>


namespace ioUtils {
    using std::distance;
    using std::next;
    using std::prev;
    using std::find_if;
    using std::runtime_error;
    using std::abort;
    using std::ios;
    using std::lexicographical_compare;
}

using namespace ioUtils;

IntegerString::DigitInString IntegerString::mostSignificantDigit(string_view integer) {

    auto beg = integer.begin();

    // Check if starting character is '+' or '-' and move the beginning iteartor if that is the case
    if (integer.at(0) == '+' || integer.at(0) == '-') {
        beg = next(beg);
    }

    auto nonZeroDigitIterator =
        find_if(
            beg,
            integer.end(),
            [](char a) -> bool {
                if (a != '0') return true;
                return false;
            }
        );

    if (nonZeroDigitIterator == integer.end()) {
        // String is all 0's after ignoring sign 
        // This function assumes input is a valid integer
        // So, Just point to the last character which is 0 
        nonZeroDigitIterator = prev(nonZeroDigitIterator);
    }

    return {
        charToDigit(*nonZeroDigitIterator),
        distance(integer.begin(), nonZeroDigitIterator)
    };
}

string_view IntegerString::removeSignAndLeadingZeros(const string_view integer){
    auto digit = mostSignificantDigit(integer);
    return integer.substr(digit.positionInString);
};

string_view IntegerString::getAbsoluteValueString(string_view maybeInteger) {
    if (!isInteger(maybeInteger)) return {};
    return removeSignAndLeadingZeros(maybeInteger);
}

bool IntegerString::operator < (const IntegerString& other) const {
    if (&other == this) return false;
    if (this->negative && !other.negative) return true;
    if (!this->negative && other.negative) return false;
    // Assumes that aString and bString are have only digits and no leading zeros
    auto isnumberStringSmaller =
        [](const string_view aString, const string_view bString) -> bool {
        if (aString.empty() && bString.empty()) return false;
        if (aString.length() < bString.length()) return true;
        if (aString.length() > bString.length()) return false;
        // Equal Lengths
        return lexicographical_compare(
            aString.begin(),
            aString.end(),
            bString.begin(),
            bString.end(),
            [](char a, char b) {
            return charToDigit(a) < charToDigit(b);
        }
        );
    };

    if (this->negative && other.negative) return !isnumberStringSmaller(this->numberString, other.numberString);
    //if (!this->negative && !other.negative) 
    return isnumberStringSmaller(this->numberString, other.numberString);
}

bool IntegerString::operator == (const IntegerString& other) const {
    if(&other == this) return true;
    if (this->negative != other.negative) return false;
    // Assumes that aString and bString are have only digits and no leading zeros
    auto isnumberStringEqual =
        [](const string_view aString, const string_view bString) -> bool {
        if (aString.empty() && bString.empty()) return false;
        if (aString.length() != bString.length()) return false;
        // Equal Lengths
        return equal(
            aString.begin(),
            aString.end(),
            bString.begin(),
            bString.end(),
            [](char a, char b) {
            return charToDigit(a) == charToDigit(b);
        }
        );
    };

    return isnumberStringEqual(this->numberString, other.numberString);
}

istream& ioUtils::operator >> (istream& is, IntegerString& integerString) {
    // read obj from stream
    string input;
    is >> input;
    if (IntegerString::isInteger(input)) {
        integerString.reset(input);
    }
    else {
        is.setstate(ios::failbit);
    }
    return is;
}

ostream& ioUtils::operator << (ostream& os, const IntegerString& integerString) {
    os << integerString.toString();
    return os;
}

IntegerString::IntegerString(const string_view number) : 
numberString{ getAbsoluteValueString(number)},
negative{isNegative(number)}{
    if(numberString.empty()) negative = false;
};