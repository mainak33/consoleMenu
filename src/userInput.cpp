#include "userInput.h"
#include <limits> 
#include <string> 
#include <stdexcept> 

namespace ioUtils {
    using std::numeric_limits;
    using std::streamsize;
    using std::exit;
    using std::string;
    using std::to_string;
    using std::is_integral_v;
    using std::is_floating_point_v;
    using std::exception;
}

using namespace ioUtils;

void ioUtils::ignoreInputLine(istream& is){
    if (!isReadable(is)) return;
    if (is.peek() == std::ios::traits_type::eof()) return;
    is.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool ioUtils::hasUnextractedInput(istream& is){
    return isReadable(is) && is.peek() != '\n';
}

void ioUtils::resetInputStream(istream& is)
{
    // If the stream was closed or unrecoverrable error
    if (!isReadable(is)) return;

    is.clear(); // Clear failbit
    ignoreInputLine(is); // Ignore rest of line
}


template <typename T>
    requires requires(T a,T b){ 
        { a >= b };
        { a <= b };
    }
static bool isInRange(const T& number, T lowerBound, T upperBound) {
    return (number >= lowerBound) and (number <= upperBound);
}

static void printInvalidInputMessage(
    function<void()> callback,
    ostream & os
) {
    os << "The input provided was invalid.\n";
    callback();
}

template <typename T>
    requires is_integral_v<T> || is_floating_point_v<T>
static string getDefaultRangePromptMessage(T lowerBound, T upperBound) {
    return 
        "Please enter a number in the range [" +
        to_string(lowerBound) + "," + to_string(upperBound) +
        ")";
}

static string getDefaultRangePromptMessage(IntegerString lowerBound, IntegerString upperBound) {
    return
        "Please enter a number in the range [" +
        lowerBound.toString() + "," + upperBound.toString() +
        ")";
}

template <typename T>
static T stringtoNumber(string_view numberString) {
    if(!IntegerString::isInteger(numberString)){
        throw "String is not a valid integral type";
    };

    if(std::is_unsigned_v<T> && IntegerString::isNegative(numberString)){
        throw "String is not a valid unsigned type";
    }

    string maybeNumber{numberString};

    // Unsigned number type
    if (std::is_unsigned_v<T>) {
        auto ullRepresentation = stoull(maybeNumber);
        if (ullRepresentation > static_cast<unsigned long long>(numeric_limits<T>::max())) {
            throw "String represents a number greater than the maximum allowed value for the type";
        }
        return static_cast<T>(ullRepresentation);
    }
        
    auto llRepresentation = stoll(maybeNumber);
    if (llRepresentation > static_cast<long long>(numeric_limits<T>::max())) {
        throw "String represents a number greater than the maximum allowed value for the type";
    }

    if (llRepresentation < static_cast<long long>(numeric_limits<T>::min())) {
        throw "String represents a number smaller than the minimum allowed value for the type";
    }
    
    return static_cast<T>(llRepresentation);
}

template <>
static int stringtoNumber(string_view numberString){
    IntegerString intString{numberString};
    if (!intString.valid()){ 
        throw "String is not a valid integer";
        return -999;
    };
    return stoi(move(string(numberString)));
}

template <>
static double stringtoNumber(string_view numberString) {
    return stod(move(string(numberString)));
}

template <>
static IntegerString stringtoNumber(string_view numberString) {
    IntegerString intString{ numberString };
    if (!intString.valid()) {
        throw "String is not a valid integer";
    };
    return intString;
}

//template <typename T>
//optional<T> ioUtils::getNumberInRange(
//    T lowerBound, 
//    T upperBound, 
//    string_view prompt, 
//    istream& is, 
//    ostream& os
//){
//    string defaultPrompt{};
//    string_view promptView{prompt};
//
//    if(promptView.empty()) {
//        defaultPrompt = move(getDefaultRangePromptMessage(lowerBound, upperBound));
//        promptView = defaultPrompt;
//    }
//
//    auto printPrompt =
//        [&promptView, &os]() {
//        os << promptView << '\n';
//    };
//
//    printPrompt();
//
//    T number{};
//    string numberString{};
//    while (hasUnextractedInput(is)) // Loop until user enters a valid input
//    {
//        is >> numberString;
//        if(!is){ // If the previous extraction failed
//            resetInputStream(is);
//            printInvalidInputMessage(printPrompt, os);
//            continue;
//        }
//
//        try{
//            number = stringtoNumber<T>(numberString);
//            if (!isInRange(number, lowerBound, upperBound)) { // Invalid input
//                resetInputStream(is);
//                printInvalidInputMessage(printPrompt, os);
//                continue;
//            }
//        }catch(...){
//            resetInputStream(is);
//            printInvalidInputMessage(printPrompt, os);
//            continue;
//        }
//
//        ignoreInputLine(is); 
//        
//        return {number}; 
//    }
//
//    return {};
//}

template <typename T>
optional<T> 
ioUtils::getNumberInRange(
    T lowerBound,
    T upperBound,
    string_view prompt,
    istream& is,
    ostream& os
){
    
    auto printPromptFunction = 
        [&prompt, &lowerBound, &upperBound]
        (ostream& os){
            string defaultPrompt{};
            string_view promptView{ prompt };

            if (promptView.empty()) {
                defaultPrompt = move(getDefaultRangePromptMessage(lowerBound, upperBound));
                promptView = defaultPrompt;
            }

            os << promptView << '\n';
    };

    auto printInvalidInputMessage = [](ostream& os){
        os << "The number provided was invalid.\n";
    };

    auto printErrorMessage = [](ostream& os){
        os << "No valid number was provided.\n";
    };
   
    return 
        getValidInput
        (
            function<void(ostream&)>(printPromptFunction),
            function<void(ostream&)>(printInvalidInputMessage),
            function<void(ostream&)>(printErrorMessage),
            isAlwaysValidInput(),
            tuple<>{},
            function<T(string_view)>(stringtoNumber<T>),
            tuple<>{},
            function<bool(const T&, T, T)>(isInRange<T>),
            tuple<T, T>{lowerBound, upperBound},
            is,
            os
        );
}

void fn(
    string_view prompt, 
    istream& is, 
    ostream& os
){
    auto printPromptFunction = [&prompt](ostream& os){os << prompt << '\n'; };
    auto printInvalidInputMessage = [](ostream& os){os << "The input provided was invalid.\n";};
    auto printErrorMessage = [](ostream& os){os << "NoValid input was provided.\n";};
    auto isValidInputString = 
        [](string_view sv, size_t minlen, size_t maxlen) {
            return (sv.length() >= minlen && sv.length() <= maxlen);
        };

    auto convertStringToOutput = 
        [](string_view sv)-> int{
            return stoi(move(string{sv}));
        };
    
    //auto dontCheckOutput = [](const int& x)->bool{return true;};

    int x = getValidInput(
        printPromptFunction,
        printInvalidInputMessage,
        printErrorMessage,
        isAlwaysValidInput(),
        make_tuple<>(),
        function(convertStringToOutput),
        make_tuple<>(),
        isAlwaysValidOutput<int>(),
        make_tuple<>()
    ).value_or(-999);
}

std::function<bool(string_view)> ioUtils::isAlwaysValidInput() {
    return function([](string_view sv) { return true; });
};


template optional<int> ioUtils::getNumberInRange(int, int, string_view, istream&, ostream&);
template optional<unsigned short> ioUtils::getNumberInRange(unsigned short, unsigned short, string_view, istream&, ostream&);
template optional<double> ioUtils::getNumberInRange(double, double, string_view, istream&, ostream&);
template optional<IntegerString> ioUtils::getNumberInRange(IntegerString, IntegerString, string_view, istream&, ostream&);
