#pragma once

#include <memory>
#include <vector>
#include <string>


namespace consoleMenu{
    using std::vector;
    using std::unique_ptr;
    using std::string;
}

namespace consoleMenu {
    class menuContents {
        private:
            short spacesToBullet = 0 ;
            short spacesAfterBulet = 0;
            string bullet{""};
            string brief{""};
            string details{""};

        public:
            //static string wrap_lines()
            string briefString() {
                string leading = string(spacesToBullet, ' ') + bullet + string( spacesAfterBulet, ' ' );
                return leading + brief + "\n\n" + string(leading.size(), ' ') + details;
            }

            /*detailedString() {

            }*/

    };

    class menuNode {
        private:
            menuContents 
            vector<unique_ptr<menuNode>> children;
        public:
            addChildren

    };
}