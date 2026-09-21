#pragma once
#include <input_parser/builder/ParserCommandBuilder.h>


namespace cpp_warships::input_parser {
    template <typename T>
    class ConfigCommandBuilder: public ParserCommandBuilder<T> {
    public:
        ~ConfigCommandBuilder() override = default;

        ConfigCommandBuilder& setDescription(std::string description) override {
            this->description = std::move(description);
            return *this;
        };
        ConfigCommandBuilder& addParameter(ParserParameter parameter) override {
            this->parameters.push_back(std::move(parameter));
            return *this;
        };
        ConfigCommandBuilder& setDisplayError(ParseCallback<void> displayError) override {
            this->displayError = std::move(displayError);
            return *this;
        };
        ConfigCommandBuilder& setCallback(ParseCallback<T> function) override {
            this->executable = std::move(function);
            return *this;
        };
        ConfigCommandBuilder& setResolveAllFlags(bool resolveAll) override {
            this->resolveAllFlags = resolveAll;
            return *this;
        };
        ConfigCommandBuilder& setPrintHelp(ParseCallback<void> help) override {
            this->printHelp = help;
            return *this;
        };
        ParserCommandInfoConfig<T> build() {
            return ParserCommandInfoConfig<T>({
                this->description,
                this->parameters,
                this->executable ? this->executable : nullptr,
                this->displayError ? this->displayError : nullptr,
                this->resolveAllFlags ? this->resolveAllFlags : false,
                this->printHelp ? this->printHelp : nullptr
            });
        };
        ParserCommandInfoConfig<T> buildAndReset() {
            ParserCommandInfoConfig config = this->build();
            this->reset();
            return config;
        };
    };
}
