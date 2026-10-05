#pragma once

#include <serialization/exceptions/InterpretationException.h>
#include <serialization/helpers/TupleBuilder.h>

#include <any>
#include <string>
#include <utility>

namespace cpp_warships::serialization {
    class ISerializerBase;

    template <char* TName = nullptr>
    class ISerializerTyped;

    template <typename TSerialized, typename TItem, char* TName = nullptr, typename... TArguments>
    class ISerializer;

    template <typename T>
    struct is_serializer_derivative;

    template <typename TSerialized, typename TItem>
    class ISerializerCore;

    /** @brief Pure virtual base class that doesn't use any template parameters. */
    class ISerializerBase {
    public:
        virtual ~ISerializerBase() = default;
        [[nodiscard]] virtual std::string getType() = 0;
    };

    /** @brief Base class for typed serializers implementing the getType method. */
    template <char* TName>
    class ISerializerTyped : public ISerializerBase {
    public:
        /** @brief getType method that returns the type name or throws an error when called on
         * base class. */
        [[nodiscard]] std::string getType() override {
            if constexpr (TName != nullptr) {
                return TName;
            } else {
                throw exceptions::InterpretationException(
                    "Trying to get type from ISerializableTyped with nullptr "
                    "TName."
                );
            }
        }
    };

    /** @brief Trait to check if a type is a derivative of ISerializer. */
    template <typename T>
    struct is_serializer_derivative {
    private:
        template <typename U, typename V, char* Z, typename... TArguments>
        static std::true_type test(ISerializer<U, V, Z, TArguments...>*);
        static std::false_type test(...);

    public:
        static constexpr bool value = decltype(test(std::declval<T*>()))::value;
    };

    /** @brief Concept to check if a type is a ISerializer derivative. */
    template <typename T>
    concept SerializerDerivative = is_serializer_derivative<T>::value;

    /** @brief Core interface for local serializers that handle serialization
     * and deserialization. */
    template <typename TSerialized, typename TItem>
    class ISerializerCore {
    public:
        virtual ~ISerializerCore() = default;

        /** @brief Serializes an item into a serialized format. @return The
         * serialized representation of the item. */
        virtual TSerialized serialize(TItem& item) = 0;

        /** @brief Deserializes data into an item. @return The deserialized item
         * instance with filled-in fields. */
        virtual TItem deserialize(TSerialized data) = 0;

        /** @brief Checks if the serializer can handle the given serialized item.          * @return
         * True if the serializer can handle the item, false otherwise. */
        virtual bool isRelated(TSerialized item) = 0;
    };

    /** @brief Local serializer interface that combines type information and
     * serialization logic. */
    template <typename TSerialized, typename TItem, char* TName, typename... TChildren>
    class ISerializer : public ISerializerTyped<TName>, public ISerializerCore<TSerialized, TItem> {
    public:
        ~ISerializer() override = default;
        ISerializer() = default;

        /** @brief Constructor that initializes the serializer with child serializers. */
        template <
            typename... Arguments,
            typename = std::enable_if_t<
                (sizeof...(Arguments) == sizeof...(TChildren)) && (sizeof...(TChildren) > 0) &&
                std::is_same_v<std::tuple<Arguments...>, std::tuple<TChildren...>>>>
        explicit ISerializer(Arguments&&... children)
            : childrenSerializers(std::forward<Arguments>(children)...) {}

        /** @brief Method to set child serializers at runtime that will be used
         * by this serializer. */
        template <typename... Arguments>
        void setChildrenSerializers(Arguments*... children) {
            childrenSerializers =
                helpers::TupleBuilder<TChildren...>::build(std::forward<Arguments>(*children)...);
        }

    protected:
        /** @brief Tuple of child serializers that this serializer can use. */
        std::tuple<TChildren...> childrenSerializers;
    };
}  // namespace cpp_warships::serialization