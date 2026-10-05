#pragma once
#include <functional>
#include <regex>
#include <string>
#include <utility>

class TypesHelper {
public:
    template <class TClass>
    static std::function<void()> methodToFunction(void (TClass::*method)(), TClass* instance) {
        return [method, instance]() { (instance->*method)(); };
    }

    template <class TClass, class TOptions>
    static std::function<void(TOptions)> methodToFunction(
        void (TClass::*method)(TOptions),
        TClass* instance
    ) {
        return [method, instance](TOptions options) { (instance->*method)(options); };
    }

    template <class TClass, class TOptions, class TReturn>
    static std::function<TReturn(TOptions)> methodToFunction(
        TReturn (TClass::*method)(TOptions),
        TClass* instance
    ) {
        return [method, instance](TOptions options) { return (instance->*method)(options); };
    }

    template <typename TClass, typename TReturn, typename... TArguments>
    static std::function<TReturn(TArguments...)> methodToFunction(
        TReturn (TClass::*method)(TArguments...),
        TClass* instance
    ) {
        return [method, instance](TArguments... arguments) -> TReturn {
            return (instance->*method)(arguments...);
        };
    }

    static std::pair<int, int> cell(const std::string& coordinate);
    static std::pair<int, int> convertToPair(const std::string& input);
};
