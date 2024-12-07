#pragma once
#include "integerString.h"
#include <iostream> 
#include <string_view> 
#include <type_traits>
#include <functional> 
#include <tuple> 

namespace ioUtils {
    using std::cin;
    using std::cout;
    using std::string_view;
    using std::is_default_constructible_v;
    using std::function;
    using std::apply;
    using std::tuple;
    using std::make_tuple;
    using std::tuple_cat;
}

namespace ioUtils {
    void ignoreInputLine();
    
    bool hasUnextractedInput();
    
    void resetInputStream();

    template <
        typename T,
        typename... PromptArgTypes,
        typename... ValidationArgTypes,
        typename... ConversionArgTypes
    >
        //requires std::is_default_constructible_v<T>
    auto getValidInput(
        function<void(PromptArgTypes...)> printPromptFunction,
        tuple<PromptArgTypes...> printPromptArguments,
        function<bool(string, ValidationArgTypes...)> isValidInputString,
        tuple<ValidationArgTypes...> validationArgs,
        function<T(string, ConversionArgTypes...)> convertStringToOutput,
        tuple<ConversionArgTypes...> conversionArgs
    ){
    
        auto printPrompt =
            [&printPromptFunction, &printPromptArguments]() -> void{
                apply(printPromptFunction, printPromptArguments);

        };

        auto printInvalidInputMessage = []()  {
            cout << "The input provided was invalid.\n";
        };

        T output{};
        string inputString{};
        while (true) // Loop until user enters a valid input
        {
            cin >> inputString;
            if (!cin) { // If the previous extraction failed
                resetInputStream();
                printInvalidInputMessage();
                printPrompt();
                continue;
            }

            try {
                auto validationAllArgs = tuple_cat(make_tuple(inputString), validationArgs);
                if (!apply(isValidInputString, validationAllArgs)) { // Invalid input
                    resetInputStream();
                    printInvalidInputMessage();
                    printPrompt();
                    continue;
                }

                auto conversionAllArgs = tuple_cat(make_tuple(inputString), conversionArgs);
                output = apply(convertStringToOutput, conversionAllArgs);
                // Wait to go outside the try block and ignore input line before returning 
            }catch (...) {
                resetInputStream();
                printInvalidInputMessage();
                printPrompt();
                continue;
            }

            ignoreInputLine();
            return output;
        }
    }
    
    template <typename T>
        //requires std::is_integral_v<T> || std::is_floating_point_v<T>
    T getNumberInRange(T lowerBound, T upperBound, string_view prompt);

    extern template int getNumberInRange(int, int, string_view);    
    extern template double getNumberInRange(double, double, string_view);
    extern template IntegerString getNumberInRange(IntegerString, IntegerString, string_view);
}
