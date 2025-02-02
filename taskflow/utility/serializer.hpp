#pragma once

#include <type_traits>
#include <iterator>
#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <list>
#include <forward_list>
#include <numeric>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <array>
#include <string>
#include "absl/types/optional.h"
#include "absl/types/variant.h"

namespace tf {

// ----------------------------------------------------------------------------
// Supported C++ STL type
// ----------------------------------------------------------------------------

// std::basic_string
template <typename T>
struct is_std_basic_string : std::false_type {};

template <typename... ArgsT>
struct is_std_basic_string <std::basic_string<ArgsT...>> : std::true_type {};

// std::array
template <typename T>
struct is_std_array : std::false_type {};

template <typename T, size_t N>
struct is_std_array <std::array<T, N>> : std::true_type {};

// std::vector
template <typename T>
struct is_std_vector : std::false_type {};

template <typename... ArgsT>
struct is_std_vector <std::vector<ArgsT...>> : std::true_type {};

// std::deque
template <typename T>
struct is_std_deque : std::false_type {};

template <typename... ArgsT>
struct is_std_deque <std::deque<ArgsT...>> : std::true_type {};

// std::list
template <typename T>
struct is_std_list : std::false_type {};

template <typename... ArgsT>
struct is_std_list <std::list<ArgsT...>> : std::true_type {};

// std::forward_list
template <typename T>
struct is_std_forward_list : std::false_type {};

template <typename... ArgsT>
struct is_std_forward_list <std::forward_list<ArgsT...>> : std::true_type {};

// std::map
template <typename T>
struct is_std_map : std::false_type {};

template <typename... ArgsT>
struct is_std_map <std::map<ArgsT...>> : std::true_type {};

// std::unordered_map
template <typename T>
struct is_std_unordered_map : std::false_type {};

template <typename... ArgsT>
struct is_std_unordered_map <std::unordered_map<ArgsT...>> : std::true_type {};

// std::set
template <typename T>
struct is_std_set : std::false_type {};

template <typename... ArgsT>
struct is_std_set <std::set<ArgsT...>> : std::true_type {};

// std::unordered_set
template <typename T>
struct is_std_unordered_set : std::false_type {};

template <typename... ArgsT>
struct is_std_unordered_set <std::unordered_set<ArgsT...>> : std::true_type {};

// absl::variant
template <typename T>
struct is_std_variant : std::false_type {};

template <typename... ArgsT>
struct is_std_variant <absl::variant<ArgsT...>> : std::true_type {};

// absl::optional
template <typename T>
struct is_std_optional : std::false_type {};

template <typename... ArgsT>
struct is_std_optional <absl::optional<ArgsT...>> : std::true_type {};

// std::unique_ptr
template <typename T>
struct is_std_unique_ptr : std::false_type {};

template <typename... ArgsT>
struct is_std_unique_ptr <std::unique_ptr<ArgsT...>> : std::true_type {};

// template <typename T>
// constexpr bool is_std_unique_ptr_v = is_std_unique_ptr<T>::value;

// std::shared_ptr
template <typename T>
struct is_std_shared_ptr : std::false_type {};

template <typename... ArgsT>
struct is_std_shared_ptr <std::shared_ptr<ArgsT...>> : std::true_type {};

// template <typename T>
// constexpr bool is_std_shared_ptr_v = is_std_shared_ptr<T>::value;

// std::duration
template <typename T> struct is_std_duration : std::false_type {};

template <typename... ArgsT>
struct is_std_duration<std::chrono::duration<ArgsT...>> : std::true_type {};

// std::time_point
template <typename T>
struct is_std_time_point : std::false_type {};

template <typename... ArgsT>
struct is_std_time_point<std::chrono::time_point<ArgsT...>> : std::true_type {};

// std::tuple
template <typename T>
struct is_std_tuple : std::false_type {};

template <typename... ArgsT>
struct is_std_tuple<std::tuple<ArgsT...>> : std::true_type {};

//-----------------------------------------------------------------------------
// Type extraction.
//-----------------------------------------------------------------------------

// ExtractType: forward declaration
template <size_t, typename>
struct ExtractType;

// ExtractType_t: alias interface
template <size_t idx, typename C>
using ExtractType_t = typename ExtractType<idx, C>::type;

// ExtractType: base
template <template <typename...> typename C, typename T, typename... RestT>
struct ExtractType <0, C<T, RestT...>> {
  using type = T;
};

// ExtractType: base
template <typename T>
struct ExtractType <0, T> {
  using type = T;
};

// ExtractType: recursive definition.
template <size_t idx, template <typename...> typename C, typename T, typename... RestT>
struct ExtractType <idx, C<T, RestT...>> : ExtractType<idx-1, C<RestT...>> {
};

// ----------------------------------------------------------------------------
// Size Wrapper
// ----------------------------------------------------------------------------

// Struct: SizeTag
// Class that wraps a given size item which can be customized.
template <typename T>
class SizeTag {

  public:

    using type = neo::conditional_t<std::is_lvalue_reference<T>::value, T, neo::decay_t<T>>;

    SizeTag(T&& item) : _item(std::forward<T>(item)) {}

    SizeTag& operator = (const SizeTag&) = delete;

    inline const T& get() const {return _item;}

    template <typename ArchiverT>
    auto save(ArchiverT & ar) const -> decltype(ar(std::declval<type>())) { return ar(_item); }

    template <typename ArchiverT>
    auto load(ArchiverT & ar) -> decltype(ar(std::declval<type>())) { return ar(_item); }

  private:

    type _item;
};

// Function: make_size_tag
template <typename T>
SizeTag<T> make_size_tag(T&& t) {
  return { std::forward<T>(t) };
}

// ----------------------------------------------------------------------------
// Size Wrapper
// ----------------------------------------------------------------------------

// Class: MapItem
template <typename KeyT, typename ValueT>
class MapItem {

  public:

    using KeyType = neo::conditional_t <std::is_lvalue_reference<KeyT>::value, KeyT, neo::decay_t<KeyT>>;
    using ValueType = neo::conditional_t <std::is_lvalue_reference<ValueT>::value, ValueT, neo::decay_t<ValueT>>;

    MapItem(KeyT&& k, ValueT&& v) : _key(std::forward<KeyT>(k)), _value(std::forward<ValueT>(v)) {}
    MapItem& operator = (const MapItem&) = delete;

    inline const KeyT& key() const { return _key; }
    inline const ValueT& value() const { return _value; }

    template <typename ArchiverT>
    auto save(ArchiverT & ar) const -> decltype(ar(std::declval<KeyType>(), std::declval<ValueType>())) { return ar(_key, _value); }

    template <typename ArchiverT>
    auto load(ArchiverT & ar) -> decltype(ar(std::declval<KeyType>(), std::declval<ValueType>())) { return ar(_key, _value); }

  private:

    KeyType _key;
    ValueType _value;
};

// Function: make_kv_pair
template <typename KeyT, typename ValueT>
MapItem<KeyT, ValueT> make_kv_pair(KeyT&& k, ValueT&& v) {
  return { std::forward<KeyT>(k), std::forward<ValueT>(v) };
}

// ----------------------------------------------------------------------------
// Serializer Definition
// ----------------------------------------------------------------------------

template <typename T>
struct is_default_serializable : std::integral_constant<bool,
    std::is_arithmetic<T>::value    ||
    std::is_enum<T>::value          ||
    is_std_basic_string<T>::value   ||
    is_std_vector<T>::value         ||
    is_std_deque<T>::value          ||
    is_std_list<T>::value           ||
    is_std_forward_list<T>::value   ||
    is_std_map<T>::value            ||
    is_std_unordered_map<T>::value  ||
    is_std_set<T>::value            ||
    is_std_unordered_set<T>::value  ||
    is_std_duration<T>::value       ||
    is_std_time_point<T>::value     ||
    is_std_variant<T>::value        ||
    is_std_optional<T>::value       ||
    is_std_tuple<T>::value          ||
    is_std_array<T>::value> {};


    // Class: Serializer
template <typename Stream, typename SizeType = std::streamsize>
class Serializer {

  public:

    Serializer(Stream& stream);

    template <typename... T>
    SizeType operator()(T&&... items);

  private:

    Stream& _stream;

    /// helper
    template <typename T>
    SizeType accumulate(T&& t);

    template <typename T, typename... Args>
    SizeType accumulate(T&& t, Args&&... items);

    struct Visitor{
        template<typename T>
        SizeType operator()(T&& t){
            return serializer._save(std::forward<T>(t));
        }

        Visitor(Serializer<Stream, SizeType>& s)
            : serializer(s){}
        Serializer<Stream, SizeType>& serializer;
    };

    /// tuple helper
    struct Traversal{
        template<typename... Args>
        SizeType operator()(Args&&... args){
            return serializer.accumulate(std::forward<Args>(args)...);
        }

        Traversal(Serializer<Stream, SizeType>& s)
            : serializer(s){}
        Serializer<Stream, SizeType>& serializer;
    };




    template <typename T,
      neo::enable_if_t<!is_default_serializable<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<std::is_arithmetic<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_basic_string<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_vector<neo::decay_t<T>>::value, void>* = nullptr,
      neo::enable_if_t<std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_vector<neo::decay_t<T>>::value, void>* = nullptr,
      neo::enable_if_t<!std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<
        is_std_deque<neo::decay_t<T>>::value ||
        is_std_list<neo::decay_t<T>>::value,
        void
      >* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<
        is_std_forward_list<neo::decay_t<T>>::value,
        void
      >* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<
        is_std_map<neo::decay_t<T>>::value ||
        is_std_unordered_map<neo::decay_t<T>>::value,
        void
      >* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<
        is_std_set<neo::decay_t<T>>::value ||
        is_std_unordered_set<neo::decay_t<T>>::value,
        void
      >* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<std::is_enum<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_duration<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_time_point<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_optional<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_variant<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_tuple<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_array<neo::decay_t<T>>::value, void>* = nullptr,
      neo::enable_if_t<std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>* = nullptr
    >
    SizeType _save(T&&);

    template <typename T,
      neo::enable_if_t<is_std_array<neo::decay_t<T>>::value, void>* = nullptr,
      neo::enable_if_t<!std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>* = nullptr
    >
    SizeType _save(T&&);


};

// Constructor
template <typename Stream, typename SizeType>
Serializer<Stream, SizeType>::Serializer(Stream& stream) : _stream(stream) {
}

template <typename Stream, typename SizeType>
template <typename T>
SizeType Serializer<Stream, SizeType>::accumulate(T&& t) {
    return _save(std::forward<T>(t));
}

template <typename Stream, typename SizeType>
template <typename T, typename... Args>
SizeType Serializer<Stream, SizeType>::accumulate(T&& t, Args&&... items) {
    return _save(std::forward<T>(t)) + accumulate(std::forward<Args>(items)...);
}

// Operator ()
template <typename Stream, typename SizeType>
template <typename... T>
SizeType Serializer<Stream, SizeType>::operator() (T&&... items) {
  // return (_save(std::forward<T>(items)) + ...);
  return accumulate(std::forward<T>(items)...);
}

// arithmetic data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<std::is_arithmetic<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  _stream.write(reinterpret_cast<const char*>(std::addressof(t)), sizeof(t));
  return sizeof(t);
}

// std::basic_string
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_basic_string<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  using U = neo::decay_t<T>;
  auto sz = _save(make_size_tag(t.size()));
  _stream.write(
    reinterpret_cast<const char*>(t.data()),
    t.size()*sizeof(typename U::value_type)
  );
  return sz + t.size()*sizeof(typename U::value_type);
}

// std::vector
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_vector<neo::decay_t<T>>::value, void>*,
  neo::enable_if_t<std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {

  using U = neo::decay_t<T>;

  auto sz = _save(make_size_tag(t.size()));
  _stream.write(
          reinterpret_cast<const char*>(t.data()),
          t.size() * sizeof(typename U::value_type)
          );
  sz += t.size() * sizeof(typename U::value_type);

  return sz;
}

template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_vector<neo::decay_t<T>>::value, void>*,
  neo::enable_if_t<!std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {

  using U = neo::decay_t<T>;

  auto sz = _save(make_size_tag(t.size()));
  for(auto&& item : t) {
      sz += _save(item);
  }

  return sz;
}


// std::list and std::deque
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_deque<neo::decay_t<T>>::value ||
                   is_std_list<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  auto sz = _save(make_size_tag(t.size()));
  for(auto&& item : t) {
    sz += _save(item);
  }
  return sz;
}

// std::forward_list
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_forward_list<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  auto sz = _save(make_size_tag(std::distance(t.begin(), t.end())));
  for(auto&& item : t) {
    sz += _save(item);
  }
  return sz;
}

// std::map and std::unordered_map
template <typename Stream, typename SizeType>
template <typename T, neo::enable_if_t<
  is_std_map<neo::decay_t<T>>::value ||
  is_std_unordered_map<neo::decay_t<T>>::value,
  void
>*>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  auto sz = _save(make_size_tag(t.size()));
  for(auto&& pair : t) {
    sz += _save(make_kv_pair(t.first, t.second));
  }
  return sz;
}

// std::set and std::unordered_set
template <typename Stream, typename SizeType>
template <typename T, neo::enable_if_t<
  is_std_set<neo::decay_t<T>>::value ||
  is_std_unordered_set<neo::decay_t<T>>::value,
  void
>*>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  auto sz = _save(make_size_tag(t.size()));
  for(auto&& item : t) {
    sz += _save(item);
  }
  return sz;
}

// enum data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<std::is_enum<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  using U = neo::decay_t<T>;
  return _save(static_cast<neo::underlying_type_t<U>>(t));
}

// duration data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_duration<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  return _save(t.count());
}

// time point data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_time_point<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  return _save(t.time_since_epoch());
}

// optional data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_optional<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  if(bool flag = t.has_value()) {
    return _save(flag) + _save(*t);
  }
  else {
    return _save(flag);
  }
}

// variant type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_variant<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  return _save(t.index()) +
         // absl::visit([&] (auto&& arg){ return _save(arg);}, t);
         absl::visit(Visitor{*this}, t);
}

// tuple type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_tuple<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  return absl::apply( Traversal(*this), std::forward<T>(t));
}

// array
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_array<neo::decay_t<T>>::value, void>*,
  neo::enable_if_t<std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {

  using U = neo::decay_t<T>;

  static_assert(std::tuple_size<U>::value > 0, "Array size can't be zero");

  _stream.write(reinterpret_cast<const char*>(t.data()), sizeof(t));
  SizeType sz = sizeof(t);

  return sz;
}

template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_array<neo::decay_t<T>>::value, void>*,
  neo::enable_if_t<!std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {

  using U = neo::decay_t<T>;

  static_assert(std::tuple_size<U>::value > 0, "Array size can't be zero");

  SizeType sz = 0;
  for(auto&& item : t) {
      sz += _save(item);
  }

  return sz;
}


// custom save method
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<!is_default_serializable<neo::decay_t<T>>::value, void>*
>
SizeType Serializer<Stream, SizeType>::_save(T&& t) {
  return t.save(*this);
}

// ----------------------------------------------------------------------------
// DeSerializer Definition
// ----------------------------------------------------------------------------

template <typename T>
struct is_default_deserializable: std::integral_constant<bool,
      std::is_arithmetic<T>::value    ||
      std::is_enum<T>::value          ||
      is_std_basic_string<T>::value   ||
      is_std_vector<T>::value         ||
      is_std_deque<T>::value          ||
      is_std_list<T>::value           ||
      is_std_forward_list<T>::value   ||
      is_std_map<T>::value            ||
      is_std_unordered_map<T>::value  ||
      is_std_set<T>::value            ||
      is_std_unordered_set<T>::value  ||
      is_std_duration<T>::value       ||
      is_std_time_point<T>::value     ||
      is_std_variant<T>::value        ||
      is_std_optional<T>::value       ||
      is_std_tuple<T>::value          ||
      is_std_array<T>::value> {};

// Class: Deserializer
template <typename Stream, typename SizeType = std::streamsize>
class Deserializer {

  public:

    Deserializer(Stream& stream);

    template <typename... T>
    SizeType operator()(T&&... items);

  private:

    Stream& _stream;

    // helper
    template <typename T>
    SizeType accumulate(T&& t);

    template <typename T, typename... Args>
    SizeType accumulate(T&& t, Args&&... items);

    // tuple helper
    struct Traversal{
        template<typename... Args>
        SizeType operator()(Args&&... args){
            return serializer.accumulate(std::forward<Args>(args)...);
        }

        Traversal(Deserializer<Stream, SizeType>& s)
            : serializer(s){}
        Deserializer<Stream, SizeType>& serializer;
    };

    // Function: _variant_helper
    template <
      size_t I = 0, typename... ArgsT,
      neo::enable_if_t<I==sizeof...(ArgsT)>* = nullptr
    >
    SizeType _variant_helper(size_t, absl::variant<ArgsT...>&);

    // Function: _variant_helper
    template <
      size_t I = 0, typename... ArgsT,
      neo::enable_if_t<I<sizeof...(ArgsT)>* = nullptr
    >
    SizeType _variant_helper(size_t, absl::variant<ArgsT...>&);

    template <typename T,
      neo::enable_if_t<std::is_arithmetic<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_basic_string<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_vector<neo::decay_t<T>>::value, void>* = nullptr,
      neo::enable_if_t<std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_vector<neo::decay_t<T>>::value, void>* = nullptr,
      neo::enable_if_t<!std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<
        is_std_deque<neo::decay_t<T>>::value ||
        is_std_list<neo::decay_t<T>>::value  ||
        is_std_forward_list<neo::decay_t<T>>::value,
        void
      >* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_map<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_unordered_map<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_set<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_unordered_set<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<std::is_enum<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_duration<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_time_point<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_optional<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_variant<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_tuple<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_array<neo::decay_t<T>>::value, void>* = nullptr,
      neo::enable_if_t<std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<is_std_array<neo::decay_t<T>>::value, void>* = nullptr,
      neo::enable_if_t<!std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>* = nullptr
    >
    SizeType _load(T&&);

    template <typename T,
      neo::enable_if_t<!is_default_deserializable<neo::decay_t<T>>::value, void>* = nullptr
    >
    SizeType _load(T&&);
};

// Constructor
template <typename Stream, typename SizeType>
Deserializer<Stream, SizeType>::Deserializer(Stream& stream) : _stream(stream) {
}

// Operator ()
template <typename Stream, typename SizeType>
template <typename T>
SizeType Deserializer<Stream, SizeType>::accumulate(T&& t) {
    return _load(std::forward<T>(t));
}

template <typename Stream, typename SizeType>
template <typename T, typename... Args>
SizeType Deserializer<Stream, SizeType>::accumulate(T&& t, Args&&... items) {
    return _load(std::forward<T>(t)) + accumulate(std::forward<Args>(items)...);
}

template <typename Stream, typename SizeType>
template <typename... T>
SizeType Deserializer<Stream, SizeType>::operator() (T&&... items) {
  // return (_load(std::forward<T>(items)) + ...);
  return accumulate(std::forward<T>(items)...);
}

// Function: _variant_helper
template <typename Stream, typename SizeType>
template <size_t I, typename... ArgsT, neo::enable_if_t<I==sizeof...(ArgsT)>*>
SizeType Deserializer<Stream, SizeType>::_variant_helper(size_t, absl::variant<ArgsT...>&) {
  return 0;
}

// Function: _variant_helper
template <typename Stream, typename SizeType>
template <size_t I, typename... ArgsT, neo::enable_if_t<I<sizeof...(ArgsT)>*>
SizeType Deserializer<Stream, SizeType>::_variant_helper(size_t i, absl::variant<ArgsT...>& v) {
  if(i == 0) {
    using type = ExtractType_t<I, absl::variant<ArgsT...>>;
    if(v.index() != I) {
      static_assert(
        std::is_default_constructible<type>::value,
        "Failed to archive variant (type should be default constructible T())"
      );
      v = type();
    }
    return _load(*absl::get_if<type>(&v));
  }
  return _variant_helper<I+1, ArgsT...>(i-1, v);
}

// arithmetic data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<std::is_arithmetic<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  _stream.read(reinterpret_cast<char*>(std::addressof(t)), sizeof(t));
  return sizeof(t);
}

// std::basic_string
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_basic_string<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  using U = neo::decay_t<T>;
  typename U::size_type num_chars;
  auto sz = _load(make_size_tag(num_chars));
  t.resize(num_chars);
  _stream.read(reinterpret_cast<char*>(t.data()), num_chars*sizeof(typename U::value_type));
  return sz + num_chars*sizeof(typename U::value_type);
}

// std::vector
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_vector<neo::decay_t<T>>::value, void>*,
  neo::enable_if_t<std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {

  using U = neo::decay_t<T>;

  typename U::size_type num_data;

  auto sz = _load(make_size_tag(num_data));

  t.resize(num_data);
  _stream.read(reinterpret_cast<char*>(t.data()), num_data * sizeof(typename U::value_type));
  sz += num_data * sizeof(typename U::value_type);

  return sz;
}

template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_vector<neo::decay_t<T>>::value, void>*,
  neo::enable_if_t<!std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {

  using U = neo::decay_t<T>;

  typename U::size_type num_data;

  auto sz = _load(make_size_tag(num_data));

  t.resize(num_data);
  for(auto && v : t) {
      sz += _load(v);
  }

  return sz;
}

// std::list and std::deque
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_deque<neo::decay_t<T>>::value ||
                   is_std_list<neo::decay_t<T>>::value  ||
                   is_std_forward_list<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  using U = neo::decay_t<T>;

  typename U::size_type num_data;
  auto sz = _load(make_size_tag(num_data));

  t.resize(num_data);
  for(auto && v : t) {
    sz += _load(v);
  }
  return sz;
}

// std::map
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_map<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {

  using U = neo::decay_t<T>;

  typename U::size_type num_data;
  auto sz = _load(make_size_tag(num_data));

  t.clear();
  auto hint = t.begin();

  typename U::key_type k;
  typename U::mapped_type v;

  for(size_t i=0; i<num_data; ++i) {
    sz += _load(make_kv_pair(k, v));
    hint = t.emplace_hint(hint, std::move(k), std::move(v));
  }
  return sz;
}

// std::unordered_map
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_unordered_map<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  using U = neo::decay_t<T>;
  typename U::size_type num_data;
  auto sz = _load(make_size_tag(num_data));

  t.clear();
  t.reserve(num_data);

  typename U::key_type k;
  typename U::mapped_type v;

  for(size_t i=0; i<num_data; ++i) {
    sz += _load(make_kv_pair(k, v));
    t.emplace(std::move(k), std::move(v));
  }

  return sz;
}

// std::set
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_set<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {

  using U = neo::decay_t<T>;

  typename U::size_type num_data;
  auto sz = _load(make_size_tag(num_data));

  t.clear();
  auto hint = t.begin();

  typename U::key_type k;

  for(size_t i=0; i<num_data; ++i) {
    sz += _load(k);
    hint = t.emplace_hint(hint, std::move(k));
  }
  return sz;
}

// std::unordered_set
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_unordered_set<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {

  using U = neo::decay_t<T>;

  typename U::size_type num_data;
  auto sz = _load(make_size_tag(num_data));

  t.clear();
  t.reserve(num_data);

  typename U::key_type k;

  for(size_t i=0; i<num_data; ++i) {
    sz += _load(k);
    t.emplace(std::move(k));
  }
  return sz;
}

// enum data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<std::is_enum<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  using U = neo::decay_t<T>;
  neo::underlying_type_t<U> k;
  auto sz = _load(k);
  t = static_cast<U>(k);
  return sz;
}

// duration data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_duration<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  using U = neo::decay_t<T>;
  typename U::rep count;
  auto s = _load(count);
  t = U{count};
  return s;
}

// time point data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_time_point<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  using U = neo::decay_t<T>;
  typename U::duration elapsed;
  auto s = _load(elapsed);
  t = U{elapsed};
  return s;
}

// optional data type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_optional<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {

  using U = neo::decay_t<T>;

  bool has_value;
  auto s = _load(has_value);
  if(has_value) {
    if(!t) {
      t = typename U::value_type();
    }
    s += _load(*t);
  }
  else {
    t.reset();
  }
  return s;
}

// variant type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_variant<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  neo::decay_t<decltype(t.index())> idx;
  auto s = _load(idx);
  return s + _variant_helper(idx, t);
}

// tuple type
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_tuple<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  return absl::apply(Traversal(*this), std::forward<T>(t));
}

// array
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_array<neo::decay_t<T>>::value, void>*,
  neo::enable_if_t<std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {

  using U = neo::decay_t<T>;

  static_assert(std::tuple_size<U>::value > 0, "Array size can't be zero");

  _stream.read(reinterpret_cast<char*>(t.data()), sizeof(t));
  SizeType sz = sizeof(t);

  return sz;
}

template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<is_std_array<neo::decay_t<T>>::value, void>*,
  neo::enable_if_t<!std::is_arithmetic<typename neo::decay_t<T>::value_type>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {

  using U = neo::decay_t<T>;

  static_assert(std::tuple_size<U>::value > 0, "Array size can't be zero");

  SizeType sz = 0;
  for(auto && v : t) {
      sz += _load(v);
  }

  return sz;
}


// custom save method
template <typename Stream, typename SizeType>
template <typename T,
  neo::enable_if_t<!is_default_deserializable<neo::decay_t<T>>::value, void>*
>
SizeType Deserializer<Stream, SizeType>::_load(T&& t) {
  return t.load(*this);
}

}  // ned of namespace tf -----------------------------------------------------






