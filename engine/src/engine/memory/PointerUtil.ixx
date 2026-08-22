module;

#include <type_traits>
#include "spark.h"

export module PointerUtil;

export template <typename T> struct is_shared_ptr : std::false_type {};
export template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
export template <typename T> inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;
export template<typename T> struct remove_shared_ptr { using type = T; };
export template<typename U> struct remove_shared_ptr<std::shared_ptr<U>> { using type = U; };
export template<typename T> using remove_shared_ptr_t = remove_shared_ptr<std::decay_t<T>>::type;
export template <typename T> concept SharedPtr = is_shared_ptr<T>::value;
export template <typename T> concept RawPtr = std::is_pointer_v<T>;
