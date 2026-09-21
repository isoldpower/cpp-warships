#pragma once

#include <string>
#include <iostream>

#include <serialization/ISerializer.h>
#include <serialization/ISerializable.h>
#include <serialization/helpers/serializers/JsonStringSerializer.h>

namespace cpp_warships::game_saves::examples {
    using namespace helpers::serializers;

    // Test class serializer TName definition.
    inline char ImplicitTestClassTypeName[] = "ImplicitTestClass";

    // Forward declaration of ImplicitTestClass and ImplicitTestClassStringSerializer to avoid circular dependency.
    class ImplicitTestClassStringSerializer;

    // Test class that implements ISerializable and contains both public and private fields.
    class ImplicitTestClass
        : public ISerializable<ImplicitTestClassTypeName>
    {
    private:
        std::string stringPrivateField = "default-private";
    public:
        friend ImplicitTestClassStringSerializer;
        friend std::ostream& operator<<(std::ostream& os, const ImplicitTestClass& obj);

        ImplicitTestClass() = default;
        std::string stringPublicField = "default-public";
    };

    // Serializer for ImplicitTestClass with std::string serialization implementation.
    class ImplicitTestClassStringSerializer
        : public ISerializer<std::string, ImplicitTestClass, ImplicitTestClassTypeName>
    {
    public:
        bool isRelated(std::string item) override {
            return JsonStringSerializer::isIncludeFields(item, "stringPublicField", "stringPrivateField");
        }

        std::string serialize(ImplicitTestClass& item) override {
            return JsonStringSerializer::serializeFields({
                { "stringPublicField", item.stringPublicField },
                { "stringPrivateField", item.stringPrivateField }
            });
        }

        ImplicitTestClass deserialize(std::string data) override {
            auto* testClass = new ImplicitTestClass();

            try {
                JsonStringSerializer::setFieldValue(&testClass->stringPublicField, data, "stringPublicField");
                JsonStringSerializer::setFieldValue(&testClass->stringPrivateField, data, "stringPrivateField");
            } catch (std::exception& e) {
                delete testClass;
                throw;
            }

            return *testClass;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const ImplicitTestClass& obj) {
        os << "ImplicitTestClass stringPublicField: " << obj.stringPublicField << std::endl;
        os << "ImplicitTestClass stringPrivateField: " << obj.stringPrivateField << std::endl;

        return os;
    }
} // namespace cpp_warships::game_saves::examples

