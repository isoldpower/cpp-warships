#pragma once
#include <input_parser/builder/ParserCommandBuilder.h>

#include <utility>

namespace cpp_warships::input_parser::builder {
    template <typename T>
    class ConfigCommandBuilder : public ParserCommandBuilder<T> {
    public:
        ~ConfigCommandBuilder() override = default;

        ConfigCommandBuilder& setDescription(std::string chosenDescription) override {
            this->description = std::move(chosenDescription);
            return *this;
        };
        ConfigCommandBuilder& addParameter(model::ParserParameter parameter) override {
            this->parameters.push_back(std::move(parameter));
            return *this;
        };
        ConfigCommandBuilder& setDisplayError(
            model::ParseCallback<void> chosenDisplayError
        ) override {
            this->displayError = std::move(chosenDisplayError);
            return *this;
        };
        ConfigCommandBuilder& setCallback(model::ParseCallback<T> function) override {
            this->executable = std::move(function);
            return *this;
        };
        ConfigCommandBuilder& setResolveAllFlags(bool resolveAll) override {
            this->resolveAllFlags = resolveAll;
            return *this;
        };
        ConfigCommandBuilder& setPrintHelp(model::ParseCallback<void> help) override {
            this->printHelp = help;
            return *this;
        };
        model::ParserCommandInfoConfig<T> build() {
            return model::ParserCommandInfoConfig<T>(
                {this->description,
                 this->parameters,
                 this->executable ? this->executable : nullptr,
                 this->displayError ? this->displayError : nullptr,
                 this->resolveAllFlags ? this->resolveAllFlags : false,
                 this->printHelp ? this->printHelp : nullptr}
            );
        };
        model::ParserCommandInfoConfig<T> buildAndReset() {
            model::ParserCommandInfoConfig config = this->build();
            this->reset();
            return config;
        };
    };
}  // namespace cpp_warships::input_parser::builder
