#include "osName.h"
#include "osConsole.h"
#include <cstdlib>
#include <stdexcept>

namespace osUtils {
    using std::runtime_error;
    using std::abort;
}

void osUtils::clearScreen() {

    try {
        int exitCode;

        if (OS::is(OS::NAME::WINDOWS)) {
            exitCode = system("cls");
        }
        else {
            exitCode = system("clear");
        }
        if (0 != exitCode) throw std::runtime_error("Error while attemppting to clear screen via a OS command");
    }catch (...) {
        std::abort();
    }
}

