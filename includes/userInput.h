#pragma once
#include "integerString.h"
#include <string_view> 

namespace ioUtils {
    using std::string_view;
}

namespace ioUtils {
    void ignoreInputLine();
    
    bool hasUnextractedInput();
    
    void resetInputStream();
    
    template <typename T>
        //requires std::is_integral_v<T> || std::is_floating_point_v<T>
    T getNumberInRange(T lowerBound, T upperBound, string_view prompt);

    extern template int getNumberInRange(int, int, string_view);    
    extern template double getNumberInRange(double, double, string_view);
    extern template IntegerString getNumberInRange(IntegerString, IntegerString, string_view);
}
