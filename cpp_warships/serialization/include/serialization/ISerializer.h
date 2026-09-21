#pragma once

#include <string>
#include <any>

#include <serialization/exceptions/InterpretationException.h>
#include <serialization/helpers/TupleBuilder.h>


namespace cpp_warships::game_saves {
    // Forward declarations to avoid circular dependencies.
    class ISerializerBase;

    template <char* TName = nullptr>
    class ISerializerTyped;

    template <typename TSerialized, typename TItem, char* TName = nullptr, typename... TArgs>
    class ISerializer;

    template <typename T>
    struct is_serializer_derivative;

    template <typename TSerialized, typename TItem>
    class ISerializerCore;


    /**
     * @brief Pure virtual base class that doesn't use any template parameters. Used for upcasting
     */
    class ISerializerBase {
    public:
        virtual ~ISerializerBase() = default;
        [[nodiscard]] virtual std::string getType()  = 0;
    };

    /**
     * @brief Base class for typed serializers implementing the getType method.
     * @tparam TName A compile-time constant character pointer representing the type name. Used for type identification.
     */
    template <char* TName>
    class ISerializerTyped : public ISerializerBase {
    public:
        /**
         * @brief getType method that returns the type name or throws an error when called on base class.
         * @return A string representation of the type name.
         */
        [[nodiscard]] std::string getType()  override  {
            if constexpr (TName != nullptr) {
                return TName;
            } else {
                throw exceptions::InterpretationException(
                    "Trying to get type from ISerializableTyped with nullptr TName."
                );
            }
        }
    };

    /**
     * @brief Trait to check if a type is a derivative of ISerializer.
     * @tparam T The type to check which is usually identified at a compile time.
     */
    template <typename T>
    struct is_serializer_derivative {
    private:
        template <typename U, typename V, char* Z, typename... TArgs>
        static std::true_type test( ISerializer<U, V, Z, TArgs...> *);
        static std::false_type test(...);
    public:
        static constexpr bool value = decltype(test(std::declval<T*>()))::value;
    };

    /**
     * @brief Concept to check if a type is a ISerializer derivative.
     * @tparam T The type to check which is usually identified at a compile time.
     */
    template <typename T>
    concept SerializerDerivative = is_serializer_derivative<T>::value;

    /**
     * @brief Core interface for local serializers that handle serialization and deserialization.
     * @tparam TSerialized The type of the serialized data.
     * @tparam TItem The type of the item being serialized or deserialized (raw type).
     */
    template <typename TSerialized, typename TItem>
    class ISerializerCore {
    public:
        virtual ~ISerializerCore() = default;

        /**
         * @brief Serializes an item into a serialized format.
         * Throws an error if the item is not related to the serializer or serialization failed.
         * @param item The reference to an item to serialize.
         * @return The serialized representation of the item.
         */
        virtual TSerialized serialize(TItem& item) = 0;

        /**
         * @brief Deserializes data into an item.
         * Throws an error if the data is not related to the serializer or deserialization failed.
         * @param data The serialized data schema to deserialize.
         * @return The deserialized item instance with filled-in fields.
         */
        virtual TItem deserialize(TSerialized data) = 0;

        /**
         * @brief Checks if the serializer can handle the given serialized item.
         * @param item The serialized item to check.
         * @return True if the serializer can handle the item, false otherwise.
         */
        virtual bool isRelated(TSerialized item) = 0;
    };

    /**
     * @brief Local serializer interface that combines type information and serialization logic.
     * @tparam TSerialized The type of the serialized data. Passed to ISerializerCore.
     * @tparam TItem The type of the item being serialized or deserialized (raw type). Passed to ISerializerCore.
     * @tparam TChildren Types of child serializers that this serializer can use.
     */
    template <typename TSerialized, typename TItem, char* TName, typename ...TChildren>
    class ISerializer
        : public ISerializerTyped<TName>
        , public ISerializerCore<TSerialized, TItem>
    {
    public:
        ~ISerializer() override = default;
        ISerializer() = default;

        /**
         * @brief Constructor that initializes the serializer with child serializers.
         * @tparam Args Types of child serializers. Must match TChildren types (e.g. ones specified when inheriting).
         * @param children Child serializers to be used by this serializer.
         *
         * This constructor is enabled only if the number of passed arguments matches
         * the number of TChildren and all types are the same as TChildren. Otherwise, it will be
         * excluded at compile time.
         */
        template <
            typename... Args,
            typename = std::enable_if_t<
                (sizeof...(Args) == sizeof...(TChildren)) &&
                (sizeof...(TChildren) > 0) &&
                std::is_same_v<std::tuple<Args...>, std::tuple<TChildren...>
            >
        >>
        // We want all passed template parameters to be of the same type as TChildren.
        // It also should be greater than 0 so it doesn't conflict with the default constructor.
        explicit ISerializer(Args&&... children)
            : childrenSerializers(std::forward<Args>(children)...)
        {}

        /**
         * @brief Method to set child serializers at runtime that will be used by this serializer.
         * @tparam Args Types of child serializers. Computed at runtime automatically.
         * @param children Pack of child serializers to be used by this serializer.
         *
         * Serializers set by this method will be reconstructed and used to serialize/deserialize
         * children items.
         */
        template <typename... Args>
        void setChildrenSerializers(Args*... children) {
            // We are using this trick to dynamically exclude serializers which are not listed in TChildren.
            // It is required for correct order of arguments in the tuple to avoid confusion
            childrenSerializers = helpers::TupleBuilder<TChildren...>::build(std::forward<Args>(*children)...);
        }
    protected:
        /**
         * @brief Tuple of child serializers that this serializer can use.
         *
         * This tuple is used to store child serializers that can be used by this serializer
         * to serialize/deserialize children items. It is initialized in the constructor and can
         * be set at runtime using setChildrenSerializers method.
         */
        std::tuple<TChildren...> childrenSerializers;
    };
} // namespace cpp_warships::game_saves