#pragma once
#include <functional>
#include <regex>


class TypesHelper {
public:
    template <class TClass>
    static std::function<void()> methodToFunction(void (TClass::*method)(), TClass *instance) {
        return [method, instance]() { (instance->*method)(); };
    }

    template <class TClass, class TOptions>
    static std::function<void(TOptions)> methodToFunction(void (TClass::*method)(TOptions),
                                                          TClass *instance) {
        return [method, instance](TOptions options) { (instance->*method)(options); };
    }

    template <class TClass, class TOptions, class TReturn>
    static std::function<TReturn(TOptions)> methodToFunction(TReturn (TClass::*method)(TOptions),
                                                             TClass *instance) {
        return [method, instance](TOptions options) { return (instance->*method)(options); };
    }

    template <typename TClass, typename TReturn, typename... TArgs>
    static std::function<TReturn(TArgs...)> methodToFunction(TReturn (TClass::*method)(TArgs...),
                                                             TClass *instance) {
        return [method, instance](TArgs... args) -> TReturn {
            return (instance->*method)(args...);
        };
    }

    static std::pair<int, int> cell(const std::string &coord);
    static std::pair<int, int> convertToPair(const std::string &input);
};
