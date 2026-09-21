#pragma once
#include <functional>
#include <string>


class ViewHelper {
public:
    static void consoleOut(const std::string& output, int level = 0);
    static void errorOut(const std::string& output);
    static void errorOut(const std::string& output, const std::exception& e);
    static bool confirmAction(const std::function<std::string()>& readInput, const std::string& confirmMessage);
};
