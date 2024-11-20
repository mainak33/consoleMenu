#pragma once
#include <string_view>
#include <iostream>
#include <tuple>

namespace svUtils {
    using std::string_view;
    using std::ostream;
    template< class... Types >
    using tuple = std::tuple<Types ...>;
}

namespace svUtils{
    
    static const size_t defaultMaxLineLength = 80;
    using SVSplit = std::tuple<string_view, string_view>;

    struct LineOptions {
        string_view indent{"    "};
        size_t maxLineLength{defaultMaxLineLength} ;
        string_view delimiter{" "};
        size_t startingPositionInLine{0};
    };

    SVSplit splitByPosition(string_view sv, size_t pos);
    SVSplit splitByDelimiter(string_view sv, const string_view delimiter = " ");
   

    void wrapToLength(
        ostream& os, 
        string_view lines, 
        const LineOptions& lineOptions = {}
    );
    
}