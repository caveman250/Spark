#pragma once

#define ENUM_CLASS_ENABLE_BITWISE(Enum)\
inline Enum operator|(Enum lhs, Enum rhs)\
{\
    using Underlying = std::underlying_type_t<Enum>;\
    return static_cast<Enum>(static_cast<Underlying>(lhs) | static_cast<Underlying>(rhs));\
}\
inline Enum operator&(Enum lhs, Enum rhs)\
{\
    using Underlying = std::underlying_type_t<Enum>;\
    return static_cast<Enum>(static_cast<Underlying>(lhs) & static_cast<Underlying>(rhs));\
}\
inline Enum operator~(Enum rhs)\
{\
    using Underlying = std::underlying_type_t<Enum>;\
    return static_cast<Enum>(~static_cast<Underlying>(rhs));\
}\
inline Enum& operator|=(Enum& lhs, Enum rhs)\
{\
    lhs = lhs | rhs;\
    return lhs;\
}\
inline Enum& operator&=(Enum& lhs, Enum rhs)\
{\
    lhs = lhs & rhs;\
    return lhs;\
}\