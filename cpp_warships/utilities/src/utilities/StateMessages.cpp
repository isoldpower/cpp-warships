#include <utilities/StateMessages.h>

#include <utilities/ViewHelper.h>


void StateMessages::displayGreetingMessage(const std::string &title) {
    ViewHelper::consoleOut("You have entered the " + title + " state.");
    ViewHelper::consoleOut("Enter 'help' to see full list of available commands.\n", 1);
}

void StateMessages::awaitCommandMessage() {
    ViewHelper::consoleOut("Waiting for future commands...");
}

void StateMessages::displayCloseMessage(const std::string &title) {
    ViewHelper::consoleOut("You have left the " + title + " state.");
}
