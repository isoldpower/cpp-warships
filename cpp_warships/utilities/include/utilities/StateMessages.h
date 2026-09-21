#pragma once
#include <string>


class StateMessages {
public:
    static void displayGreetingMessage(const std::string &title);
    static void awaitCommandMessage();
    static void displayCloseMessage(const std::string &title);
};
