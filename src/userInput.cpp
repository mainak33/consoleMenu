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

void ioUtils::ignoreInputLine(){
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool ioUtils::hasUnextractedInput(){
    return !cin.eof() && cin.peek() != '\n';
}

void ioUtils::resetInputStream()
{
    // Check for failed extraction
    if (cin.eof()){ // If the stream was closed
        exit(0); // Shut down the program now
    }

    // Let's handle the failure
    cin.clear(); // Put us back in 'normal' operation mode
    ignoreInputLine();     // And remove the bad input
}


template <typename T>
    requires requires(T a,T b){ 
        { a >= b };
        { a < b };
    }
static bool isInRange(T number, T lowerBound, T upperBound) {
    return (number >= lowerBound) and (number < upperBound);
}

static void printInvalidInputMessage(function<void()> callback) {
    cout << "The input provided was invalid.\n";
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
static T stringtoNumber(const string& numberString) {
    return T{stoi(numberString)};
}

template <>
static int stringtoNumber(const string& numberString){
    IntegerString intString{numberString};
    if (!intString.valid()){ 
        throw exception ("String is not a valid integer");
        return -999;
    };
    return stoi(numberString);
}

template <>
static double stringtoNumber(const string& numberString) {
    return stod(numberString);
}

template <>
static IntegerString stringtoNumber(const string& numberString) {
    IntegerString intString{ numberString };
    if (!intString.valid()) {
        throw exception("String is not a valid integer");
    };
    return intString;
}

template <typename T>
T ioUtils::getNumberInRange(T lowerBound, T upperBound, string_view prompt)
{
    string defaultPrompt{};
    string_view promptView{prompt};

    if(promptView.empty()) {
        defaultPrompt = move(getDefaultRangePromptMessage(lowerBound, upperBound));
        promptView = defaultPrompt;
    }

    auto printPrompt =
        [&promptView]() {
        cout << promptView << '\n';
    };

    printPrompt();

    T number{};
    string numberString{};
    while (true) // Loop until user enters a valid input
    {
        cin >> numberString;
        if(!cin){ // If the previous extraction failed
            resetInputStream();
            printInvalidInputMessage(printPrompt);
            continue;
        }

        try{
            number = stringtoNumber<T>(numberString);
        }catch(...){
            resetInputStream();
            printInvalidInputMessage(printPrompt);
            continue;
        }

        if (!isInRange(number, lowerBound, upperBound)) { // Invalid input
            resetInputStream();
            printInvalidInputMessage(printPrompt);
            continue;
        }

        ignoreInputLine(); 
        return number; 
    }
}
void fn(string_view prompt) {
    auto printPromptFunction = [](string_view prompt){cout << prompt;};
    auto isValidInputString = [](std::string x, size_t minlen, size_t maxlen) {
        return (x.length() >= minlen && x.length() <= maxlen);
    };
    auto convertStringToOutput = [](std::string str)->int{return std::stoi(str);};
    int x = getValidInput(
        function(printPromptFunction),
        std::make_tuple<>(prompt),
        function(isValidInputString),
        std::make_tuple<size_t,size_t>(0,5),
        function(convertStringToOutput),
        std::make_tuple<>()
    );
}
template int ioUtils::getNumberInRange(int, int, string_view);
template double ioUtils::getNumberInRange(double, double, string_view);
template IntegerString ioUtils::getNumberInRange(IntegerString, IntegerString, string_view);
