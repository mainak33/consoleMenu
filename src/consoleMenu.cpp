#include "osUtils.h"
#include "ioUtils.h"

using osUtils::clearScreen;
using ioUtils::getNumberInRange;
void consoleMenu()
{
    clearScreen();
    getNumberInRange<int>(1,6,"Choose an option from 1 to 5");
    ioUtils::IntegerString{"7728358638"};
}
