# TODO
- [x] recursive object serialization
- [x] exclude necessary std::any casts for final users
- [x] implement custom exceptions
- [x] create mixin classes for easier predefined serialization-deserialization behavior:
  - SomeSerializationMixin::serializeVector() and deserializeVector();
  - SomeSerializationMixin::serialize() and deserialize();
  - SomeSerializationMixin::serializeMap() and deserializeMap();
- [x] write docs
- [ ] write tests

# Possible improvements
- [ ] try to exclude necessity of defining required serializers when inheriting from base class  
