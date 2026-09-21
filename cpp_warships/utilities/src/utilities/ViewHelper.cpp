#include <utilities/ViewHelper.h>
#include <utilities/StringHelper.h>

#include <functional>
#include <iostream>
#include <ostream>


void ViewHelper::consoleOut(const std::string &output, int level) {
    std::string tabs = std::string(level, '\t');
    std::cout << tabs << output << std::endl;
}

void ViewHelper::errorOut(const std::string &output) { std::cerr << output << std::endl; }

void ViewHelper::errorOut(const std::string &output, const std::exception &e) {
    errorOut(output);
    errorOut(e.what());
}

bool ViewHelper::confirmAction(const std::function<std::string()>& readInput, const std::string &confirmMessage) {
    consoleOut("Do you want to confirm the action? (" + confirmMessage + " to confirm)");
    const std::string input = readInput();
    const std::string inputLower = StringHelper::toLower(input);
    const std::string confirmMessageLower = StringHelper::toLower(confirmMessage);
    return inputLower == confirmMessageLower;
}
