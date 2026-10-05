#pragma once
#include <input_parser/builder/ParserParameterBuilder.h>
#include <input_parser/model/ParserParameter.h>

namespace cpp_warships::input_parser::builder {
    class DefaultParameterBuilder : public ParserParameterBuilder {
    public:
        ~DefaultParameterBuilder() override = default;

        DefaultParameterBuilder& addFlag(std::string flag) override {
            this->flags.push_back(flag);
            return *this;
        };

        DefaultParameterBuilder& setValidator(std::regex chosenValidator) override {
            this->validator = chosenValidator;
            return *this;
        }

        DefaultParameterBuilder& setDescription(std::string chosenDescription) override {
            this->description = chosenDescription;
            return *this;
        }

        DefaultParameterBuilder& setNecessary(bool isNecessary) override {
            this->necessary = isNecessary;
            return *this;
        }

        model::ParserParameter build() {
            return model::ParserParameter({flags, validator, description, necessary});
        }

        model::ParserParameter buildAndReset() {
            model::ParserParameter parameter = this->build();
            this->reset();

            return parameter;
        }
    };
}  // namespace cpp_warships::input_parser::builder