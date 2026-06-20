#pragma once

#include <any>
#include <ranges>
#include <iostream>

#include "ISerializer.h"
#include "ISerializable.h"
#include "exceptions/DeserializationException.h"
#include "exceptions/InterpretationException.h"
#include "exceptions/SerializationException.h"

namespace cpp_warships::game_saves {

    template <typename TSerialized>
    class SerializerAggregator;

    /**
     * @brief A class that aggregates serializers for different types of serializable objects.
     * @tparam TSerialized The type of serialized data that this aggregator will handle (e.g., JSON, XML, binary, string).
     *
     * It is capable of dynamically serializing and deserializing objects of any type that is passed to its constructor.
     * This can be useful when passing serializer to other classes as it aggregates all configuration at one place.
     */
    template <typename TSerialized>
    class SerializerAggregator {
    protected:
        /**
         * @brief A map of available serializers indexed by their type name. Used for quick lookup.
         */
        std::unordered_map<std::string, ISerializerBase*> availableSerializers;
    public:
        SerializerAggregator() : availableSerializers({}) {}

        /**
         * @brief Serializes an item of type TPassed using the related serializer.
         * @tparam TPassed The type of the item to be serialized. Defaults to std::any.
         * @param item The item to be serialized.
         * @return The serialized data of type TSerialized.
         * @exception InterpretationException Thrown if the provided item is not a derivative of ISerializableBase.
         * @exception SerializationException Thrown if no related serializer is found for the provided item.
         *
         * This method finds the related serializer for the provided item and uses it to serialize the item.
         * If no related serializer is found, it throws an exception.
         */
        template <SerializableDerivative TPassed = std::any>
        TSerialized serialize(TPassed &item) {
            ISerializableBase *castedItem = castToSerializableBase<TPassed>(item);
            if (ISerializerBase *serializer = findRelatedSerializer(*castedItem)) {
                if (auto* castedSerializer = dynamic_cast<ISerializerCore<TSerialized, TPassed>*>(serializer)) {
                    return castedSerializer->serialize(item);
                }
            }

            throw exceptions::InterpretationException(
                "Even though related serializer found "
                "(" + std::string(typeid(TSerialized).name()) + "),"
                " conversion to ISerializerCore base failed. \n"
            );
        }

        /**
         * @brief Destructor that cleans up the available serializers.
         *
         * This destructor iterates through all available serializers and deletes them to prevent memory leaks.
         */
        ~SerializerAggregator() {
            for (const auto &value : availableSerializers | std::views::values) {
                delete value;
            }

            availableSerializers.clear();
        }

        /**
         * @brief Deserializes an item of type TSerialized using the related serializer.
         * @tparam TReturn The type of the deserialized item. Defaults to std::any.
         * @param item The serialized data to be deserialized.
         * @return The deserialized item of type TReturn.
         * @exception DeserializationException Thrown if no related serializer is found for the provided serialized data.
         *
         * This method finds the related serializer for the provided serialized data and uses it to deserialize the data.
         * Is uses the `isRelated` method of the serializer to determine if it can handle the provided serialized data.
         * If no related serializer is found, it throws an exception.
         */
        template <typename TReturn = std::any>
        TReturn deserialize(TSerialized item) {
            for (auto &serializer: availableSerializers | std::views::values) {
                auto* castedSerializer = dynamic_cast<ISerializerCore<TSerialized, TReturn>*>(serializer);
                if (castedSerializer != nullptr && castedSerializer->isRelated(item)) {
                    return castedSerializer->deserialize(item);
                }
            }

            throw exceptions::DeserializationException(
                std::string(typeid(TSerialized).name()),
                "Couldn't find a related serializer for the provided serialized data. \n"
                "Issue is likely with ISerializerCore::isRelated method."
            );
        }

        /**
         * @brief Sets the serializers that this aggregator will use for serialization and deserialization.
         * @tparam Serializers Types of serializers to be set. Must be derivatives of ISerializer.
         * @param serializers The serializers to be set.
         *
         * This method also sets child serializers for each serializer, allowing them to handle nested objects.
         * This is the main cause why setSerializers is not a constructor and called as a separate method.
         * Make sure you pass the serializers which are dynamically allocated (usually with `new`).
         */
        template<SerializerDerivative... Serializers>
        void setSerializers(Serializers*... serializers) {
            try {
                ([&](auto& serializer) {
                    availableSerializers[serializer->getType()] = serializer;
                    serializer->setChildrenSerializers(serializers...);
                }(serializers), ...);
            } catch (const std::exception &e) {
                availableSerializers.clear();
                std::cerr << e.what() << '\n';
            }
        }

    private:
        template <typename TPassed = std::any>
        static ISerializableBase* castToSerializableBase(TPassed &item) {
            if (auto* castedItem = dynamic_cast<ISerializableBase*>(&item)) {
                return castedItem;
            }

            throw exceptions::InterpretationException(
                "Provided item is not an ISerializableBase derivative. \n"
                "Type: " + std::string(typeid(TPassed).name()) + "\n"
            );
        }

        ISerializerBase* findRelatedSerializer(ISerializableBase &item) {
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
} // namespace cpp_warships::game_saves