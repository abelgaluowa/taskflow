#pragma once

#include<type_traits>

namespace neo {

template<typename T>
using decay_t = typename std::decay<T>::type;

template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<bool B, class T, class F>
using conditional_t = typename std::conditional<B, T, F>::type;

template< class T >
using underlying_type_t = typename std::underlying_type<T>::type;

}  // end of namespace neo. ----------------------------------------------------



