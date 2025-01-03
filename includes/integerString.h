#pragma once
/*********************************************************************
 * @file  integerString.h
 *
 * @brief Class integerString for a string representation of an integer
 *
 *********************************************************************/

#include <utility>
#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>

namespace ioUtils{
    using std::swap;
    using std::move;
    using std::string;
    using std::string_view;
    using std::istream;
    using std::ostream;
    using std::find_if_not;
}

namespace ioUtils {
   
    class IntegerString {
        private :
            string numberString;
            bool negative = false;
            static constexpr unsigned short charToDigit(char a);
            static constexpr bool charIsDigit(char a);
        public :

            struct DigitInString{
                unsigned short digit;
                decltype(numberString.begin())::difference_type positionInString;
            };

            static constexpr bool isInteger(string_view maybeInteger);
            static constexpr bool isNegative(const string_view maybeInteger);
            static DigitInString mostSignificantDigit(string_view integer);
            static string_view removeSignAndLeadingZeros(string_view integer);
            static string_view getAbsoluteValueString(string_view maybeInteger);


            void clear();

            void reset(const string_view number);

            inline string toString() const;
            inline bool valid(){return !numberString.empty();};

            bool operator < (const IntegerString& other) const;
            inline bool operator > (const IntegerString& other) const { return other < *this; }
            inline bool operator <= (const IntegerString& other) const { return !(*this > other); }
            inline bool operator >= (const IntegerString& other) const { return !(*this < other); }
            
            bool operator == (const IntegerString& other) const;
            inline bool operator != (const IntegerString& other) const { return !(*this == other); };
            friend istream& operator >> (istream& is, IntegerString& integerString);
            friend ostream& operator << (ostream& os, const IntegerString& integerString);

            // Constructors, Copy/Move Operators and Destructor 
            IntegerString():
                numberString{},
                negative{false}{
            }

            explicit IntegerString(const string_view number);
            
            IntegerString(const IntegerString& other) :
                numberString{other.numberString},
                negative{ other.negative } {
            }

            IntegerString& operator = (const IntegerString& other){
                if (this == &other)
                    return *this;

                IntegerString temp{other}; 
                swap(numberString, temp.numberString); 
                swap(negative, temp.negative); 
                return *this;
            }

            IntegerString(IntegerString&& other) noexcept :
                numberString{ move(other.numberString) },
                negative{ other.negative } {
            }

            IntegerString& operator=(IntegerString&& other) noexcept 
            {
                IntegerString temp{move(other)};
                swap(numberString, temp.numberString);
                swap(negative, temp.negative);
                return *this;
            }

            ~IntegerString() {
            }
    };

    // Definitions of inline and constexpr functions
    inline void IntegerString::clear(){
        numberString.clear();
        negative = false;
    }

    inline void IntegerString::reset(const string_view number) {
        clear();
        numberString = getAbsoluteValueString(number);
        if (!numberString.empty()) negative = isNegative(number);
    }

    inline std::string IntegerString::toString() const{
    
        std::string representation;
        if (negative) representation = '-';
        representation += numberString;

        return representation;
    };

    constexpr bool IntegerString::charIsDigit(char a) {
        if (a >= '0' && a <= '9') return true;
        return false;
    };

    constexpr unsigned short IntegerString::charToDigit(char a) {
        if (!charIsDigit(a)) return {};
        switch (a) {
            case('0'): return 0;
            case('1'): return 1;
            case('2'): return 2;
            case('3'): return 3;
            case('4'): return 4;
            case('5'): return 5;
            case('6'): return 6;
            case('7'): return 7;
            case('8'): return 8;
            case('9'): return 9;
            default: {
                throw "Conversion to a digit requested from an invalid character " + a;
            }
        };
    };

    constexpr bool IntegerString::isNegative(const string_view maybeInteger) {
        if (!maybeInteger.empty() && maybeInteger.at(0) == '-') return true;
        return false;
    };

    constexpr bool IntegerString::isInteger(string_view maybeInteger) {

        auto beg = maybeInteger.begin();

        // Check if starting character is '+' or '-' and move the beginning iterator if that is the case
        if (maybeInteger.at(0) == '+' || maybeInteger.at(0) == '-') {
            beg = next(beg);
        }

        // If String is now empty return false
        if (std::distance(beg, maybeInteger.end()) == size_t{ 0 }) return false;

        // Iterate over all characters and make sure they are digits
        return
            find_if_not(
                beg,
                maybeInteger.end(),
                &charIsDigit
            ) == maybeInteger.end();
    };
}
