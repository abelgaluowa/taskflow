#pragma once

#include<type_traits>
#include<memory>
#include "absl/base/internal/invoke.h"
#include "absl/types/optional.h"

namespace neo {

template<typename T>
using decay_t = typename std::decay<T>::type;

template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<bool B, class T, class F>
using conditional_t = typename std::conditional<B, T, F>::type;

template< class T >
using underlying_type_t = typename std::underlying_type<T>::type;

/// helper
template<typename T, typename... Ts>
struct is_one_of: std::false_type {
};

template<typename T, typename F, typename... Ts>
struct is_one_of<T, F, Ts...>: std::integral_constant<bool,
    std::is_same<T, F>::value ||
    is_one_of<T, Ts...>::value> {
};

/// make_unique
namespace detail {

template<class>
struct is_unbounded_array: std::false_type {};

template<class T>
struct is_unbounded_array<T[]>: std::true_type {};

template<class>
struct is_bounded_array: std::false_type {};

template<class T, std::size_t N>
struct is_bounded_array<T[N]>: std::false_type {};

} // namespace detail

template<class T, class... Args>
enable_if_t<!std::is_array<T>::value, std::unique_ptr<T>>
make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template< class T >
using remove_extent_t = typename std::remove_extent<T>::type;

template<class T>
enable_if_t<detail::is_unbounded_array<T>::value, std::unique_ptr<T>>
make_unique(std::size_t n)
{
    return std::unique_ptr<T>(new remove_extent_t<T>[n]());
}

template<class T, class... Args>
enable_if_t<detail::is_bounded_array<T>::value> make_unique(Args&&...) = delete;


/// helper for Subflow 
template<typename F, typename... ArgsT>
using InvokeRet = absl::base_internal::invoke_result_t<F, ArgsT...>;

template<typename F, typename... ArgsT>
using FRet = neo::conditional_t<std::is_same<InvokeRet<F, ArgsT...>, void>::value, void, absl::optional<InvokeRet<F, ArgsT...>>>;

}  // end of namespace neo. ----------------------------------------------------



