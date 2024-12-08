#pragma once
#include "integerString.h"
#include <iostream> 
#include <string_view> 
#include <type_traits>
#include <functional> 
#include <tuple> 
#include <optional> 

namespace ioUtils {
    using std::cin;
    using std::cout;
    using std::istream;
    using std::ostream;
    using std::string_view;
    using std::is_default_constructible_v;
    using std::function;
    using std::apply;
    using std::tuple;
    using std::make_tuple;
    using std::tuple_cat;
    using std::optional;
    using std::make_optional;
}

namespace ioUtils {
    void ignoreInputLine(istream& is);
    
    inline bool isReadable(istream& is) {
        return !(is.eof() or is.bad());
    }
    bool hasUnextractedInput(istream& is);
    
    void resetInputStream(istream& is);
    
    function<bool(string_view)> isAlwaysValidInput();
    template <class T>
    function<bool(const T&)> isAlwaysValidOutput(){ 
        return [](const T& t) {return true;}; 
    };
    
    template <
        typename T,
        typename... InputValidationArgTypes,
        typename... ConversionArgTypes,
        typename... OutputValidationArgTypes
    >
        requires std::is_default_constructible_v<T>
    optional<T> getValidInput(
        function<void(ostream&)> printPrompt,
        function<void(ostream&)> printInvalidInputMessage,
        function<void(ostream&)> printErrorMessage,
        function<bool(string_view, InputValidationArgTypes...)> isValidInput,
        tuple<InputValidationArgTypes...> inputValidationArgs,
        function<T(string_view, ConversionArgTypes...)> convertStringToOutput,
        tuple<ConversionArgTypes...> conversionArgs,
        function<bool(const T&, OutputValidationArgTypes...)> isValidOutput,
        tuple<OutputValidationArgTypes...> outputValidationArgs,
        istream& is = cin,
        ostream& os = cout
    ){
    
        auto printInvalidInputAndRepeatPrompt =
            [
                &printInvalidInputMessage, 
                &printPrompt
            ](ostream& os) -> void{
                printInvalidInputMessage(os);
                printPrompt(os);
        };

        // Prompt the User for Input
        printPrompt(os);

        T output{};
        string inputString{};
        while (hasUnextractedInput(is)) // Loop until user enters a valid input
        {
            is >> inputString;

            if (!is) { // If the previous extraction failed
                resetInputStream(is);
                printInvalidInputAndRepeatPrompt(os);
                continue;
            }

            try {
                // Validate Input String
                auto inputValidationAllArgs = tuple_cat(make_tuple(inputString), inputValidationArgs);
                if (!apply(isValidInput, inputValidationAllArgs)) { // Invalid input
                    resetInputStream(is);
                    printInvalidInputAndRepeatPrompt(os);
                    continue;
                }

                // Convert to Output
                auto conversionAllArgs = tuple_cat(make_tuple(inputString), conversionArgs);
                output = apply(convertStringToOutput, conversionAllArgs);

                // Validate Output
                auto outputValidationAllArgs = tuple_cat(make_tuple(output), outputValidationArgs);
                if (!apply(isValidOutput, outputValidationAllArgs)) { // Invalid input
                    resetInputStream(is);
                    printInvalidInputAndRepeatPrompt(os);
                    continue;
                }


            }catch (...) {
                resetInputStream(is);
                printInvalidInputAndRepeatPrompt(os);
                continue;
            }

            // Ignore input line before returning 
            ignoreInputLine(is);
            return make_optional(output);
        }


        printErrorMessage(os);
        return {};
    }
    
    template <typename T>
        //requires std::is_integral_v<T> || std::is_floating_point_v<T>
    optional<T> getNumberInRange(T lowerBound, T upperBound, string_view prompt, istream & is = cin, ostream& os = cout);

    extern template optional<int> getNumberInRange(int, int, string_view, istream&, ostream&);
    extern template optional<double> getNumberInRange(double, double, string_view, istream&, ostream&);
    extern template optional<IntegerString> getNumberInRange(IntegerString, IntegerString, string_view, istream&, ostream&);
}
