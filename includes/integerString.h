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

namespace ioUtils{
    using std::swap;
    using std::move;
    using std::string;
    using std::string_view;
    using std::istream;
    using std::ostream;
}

namespace ioUtils {
   
    class IntegerString {
        private :
            string numberString;
            bool negative = false;
        public :

            struct DigitInString{
                unsigned short digit;
                decltype(numberString.begin())::difference_type positionInString;
            };

            static constexpr bool charIsDigit(char a);
            static constexpr bool isInteger(string_view maybeInteger);
            static constexpr unsigned short charToDigit(char a);
            static DigitInString mostSignificantDigit(string_view integer);
            static string_view removeSignAndLeadingZeros(string_view integer);
            static string_view getAbsoluteValueString(string_view maybeInteger);

            static bool isNegative(const string_view maybeInteger);

            IntegerString(const string_view number);

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

            IntegerString():
                numberString{},
                negative{false}{
            }

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
}

inline void ioUtils::IntegerString::clear(){
    numberString.clear();
    negative = false;
}

inline void ioUtils::IntegerString::reset(const string_view number) {
    clear();
    numberString = getAbsoluteValueString(number);
    if (!numberString.empty()) negative = isNegative(number);
}

inline std::string ioUtils::IntegerString::toString() const{
    
    std::string representation;
    if (negative) representation = '-';
    representation += numberString;

    return representation;
};
