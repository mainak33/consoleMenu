#include "integerString.h"
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <stdexcept>


namespace ioUtils {
    using std::distance;
    using std::next;
    using std::prev;
    using std::find_if;
    using std::find_if_not;
    using std::runtime_error;
    using std::abort;
    using std::ios;
    using std::lexicographical_compare;
}

using namespace ioUtils;

constexpr bool IntegerString::charIsDigit(char a) {
    if (a >= '0' && a <= '9') return true;
    return false;
};

constexpr bool IntegerString::isInteger(string_view maybeInteger) {

    auto beg = maybeInteger.begin();

    // Check if starting character is '+' or '-' and move the beginning iteartor if that is the case
    if (maybeInteger.at(0) == '+' || maybeInteger.at(0) == '-') {
        beg = next(beg);
    }

    // If String is now empty return false
    if(std::distance(beg, maybeInteger.end()) == size_t{0}) return false;

    // Iterate over all characters and make sure they are digits
    return
        find_if_not(
            beg,
            maybeInteger.end(),
            &charIsDigit
        ) == maybeInteger.end();
};


constexpr unsigned short IntegerString::charToDigit(char a) {
    if (!charIsDigit(a)) return {};
    switch(a){
        case('0') : return 0;
        case('1') : return 1;
        case('2') : return 2;
        case('3') : return 3;
        case('4') : return 4;
        case('5') : return 5;
        case('6') : return 6;
        case('7') : return 7;
        case('8') : return 8;
        case('9') : return 9;
        default : {
           throw(runtime_error("Conversion to a digit requested from an invalid character " + a));
           abort();
        }
    };
};

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

bool IntegerString::isNegative(const string_view maybeInteger){
    if(!maybeInteger.empty() && maybeInteger.at(0) == '-') return true;
    return false;
};


IntegerString::IntegerString(const string_view number) : 
numberString{ getAbsoluteValueString(number)},
negative{isNegative(number)}{
    if(numberString.empty()) negative = false;
};

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
