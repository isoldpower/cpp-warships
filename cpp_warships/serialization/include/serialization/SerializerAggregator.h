#pragma once

#include <serialization/ISerializable.h>
#include <serialization/ISerializer.h>
#include <serialization/exceptions/DeserializationException.h>
#include <serialization/exceptions/InterpretationException.h>
#include <serialization/exceptions/SerializationException.h>

#include <any>
#include <iostream>
#include <ranges>
#include <unordered_map>

namespace cpp_warships::serialization {
    template <typename TSerialized>
    class SerializerAggregator;

    /** @brief A class that aggregates serializers for different types of
     * serializable objects. */
    template <typename TSerialized>
    class SerializerAggregator {
    protected:
        /** @brief A map of available serializers indexed by their type name. */
        std::unordered_map<std::string, ISerializerBase*> availableSerializers;

    public:
        SerializerAggregator()
            : availableSerializers({}) {}

        /** @brief Serializes an item of type TPassed using the related serializer.          *
         * @return The serialized data of type TSerialized. */
        template <SerializableDerivative TPassed = std::any>
        TSerialized serialize(TPassed& item) {
            ISerializableBase* castedItem = castToSerializableBase<TPassed>(item);
            if (ISerializerBase* serializer = findRelatedSerializer(*castedItem)) {
                if (auto* castedSerializer =
                        dynamic_cast<ISerializerCore<TSerialized, TPassed>*>(serializer)) {
                    return castedSerializer->serialize(item);
                }
            }

            throw exceptions::InterpretationException(
                "Even though related serializer found "
                "(" +
                std::string(typeid(TSerialized).name()) +
                "),"
                " conversion to ISerializerCore base failed. \n"
            );
        }

        /** @brief Destructor that cleans up the available serializers. */
        ~SerializerAggregator() {
            for (const auto& value : availableSerializers | std::views::values) {
                delete value;
            }

            availableSerializers.clear();
        }

        /** @brief Deserializes an item of type TSerialized using the related serializer.          *
         * @return The deserialized item of type TReturn. */
        template <typename TReturn = std::any>
        TReturn deserialize(TSerialized item) {
            for (auto& serializer : availableSerializers | std::views::values) {
                auto* castedSerializer =
                    dynamic_cast<ISerializerCore<TSerialized, TReturn>*>(serializer);
                if (castedSerializer != nullptr && castedSerializer->isRelated(item)) {
                    return castedSerializer->deserialize(item);
                }
            }

            throw exceptions::DeserializationException(
                std::string(typeid(TSerialized).name()),
                "Couldn't find a related serializer for the provided "
                "serialized "
                "data. "
                "\n"
                "Issue is likely with ISerializerCore::isRelated method."
            );
        }

        /** @brief Sets the serializers that this aggregator will use for
         * serialization and deserialization. */
        template <SerializerDerivative... Serializers>
        void setSerializers(Serializers*... serializers) {
            try {
                (
                    [&](auto& serializer) {
                        availableSerializers[serializer->getType()] = serializer;
                        serializer->setChildrenSerializers(serializers...);
                    }(serializers),
                    ...);
            } catch (const std::exception& error) {
                availableSerializers.clear();
                std::cerr << error.what() << '\n';
            }
        }

    private:
        template <typename TPassed = std::any>
        static ISerializableBase* castToSerializableBase(TPassed& item) {
            if (auto* castedItem = dynamic_cast<ISerializableBase*>(&item)) {
                return castedItem;
            }

            throw exceptions::InterpretationException(
                "Provided item is not an ISerializableBase derivative. \n"
                "Type: " +
                std::string(typeid(TPassed).name()) + "\n"
            );
        }

        ISerializerBase* findRelatedSerializer(ISerializableBase& item) {
            auto iterator = availableSerializers.find(item.getType());
            if (iterator != availableSerializers.end()) {
                return iterator->second;
            }

            throw exceptions::SerializationException(
                std::string(typeid(TSerialized).name()),
                "Couldn't find a related serializer for the provided item."
            );
        }
    };
}  // namespace cpp_warships::serialization