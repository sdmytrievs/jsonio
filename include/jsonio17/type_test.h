#pragma once

#include <type_traits>

namespace detail {

// Needed for some older versions of GCC
template<typename...>
struct voider { using type = void; };

// std::void_t will be part of C++17, but until then define it ourselves:
template<typename... T>
using void_t = typename voider<T...>::type;

template<typename T, typename U = void>
struct is_mappish_impl : std::false_type { };

template<typename T>
struct is_mappish_impl<T, void_t<typename T::key_type,
        typename T::mapped_type,
        decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>>
                                                                                    : std::true_type { };


template<typename T, typename U = void>
struct is_container_impl : std::false_type { };

template<typename T>
struct is_container_impl<T, void_t<typename T::value_type,
        typename T::mapped_type,
        decltype(std::declval<T>().size()),
decltype(std::declval<T>().begin()),
decltype(std::declval<T>().end()),
decltype(std::declval<T>().cbegin()),
decltype(std::declval<T>().cend())>>
                                     : std::true_type { };
}


template<typename T>
struct is_mappish : detail::is_mappish_impl<T>::type { };


//template<typename T>
//struct is_container : detail::is_container_impl<T>::type { };

// http://fuelcycle.org/cyclus/api/prettyprint_8hpp_source.html

//template <bool, typename S, typename T> struct conditional { };
//template <typename S, typename T> struct conditional<true,  S, T> { typedef S type; };
//template <typename S, typename T> struct conditional<false, S, T> { typedef T type; };

//template <bool, typename T> struct enable_if { };
//template <typename T> struct enable_if<true, T> { typedef T type; };


// SFINAE type trait to detect whether T::const_iterator exists.

template<typename T>
struct has_const_iterator
{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

// SFINAE type trait to detect whether "T::const_iterator T::begin/end() const" exist.

template <typename T>
struct has_begin_end
{
    struct Dummy { typedef void const_iterator; };
    typedef typename std::conditional<has_const_iterator<T>::value, T, Dummy>::type TType;
    typedef typename TType::const_iterator iter;

    struct Fallback { iter begin() const; iter end() const; };
    struct Derived : TType, Fallback { };

    template<typename C, C> struct ChT;

    template<typename C> static char (&f(ChT<iter (Fallback::*)() const, &C::begin>*))[1];
    template<typename C> static char (&f(...))[2];
    template<typename C> static char (&g(ChT<iter (Fallback::*)() const, &C::end>*))[1];
    template<typename C> static char (&g(...))[2];

    static bool const beg_value = sizeof(f<Derived>(0)) == 2;
    static bool const end_value = sizeof(g<Derived>(0)) == 2;
};

// Basic is_container template; specialize to have value "true" for all desired container types

//     template<typename T> struct is_container { static const bool value = has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value; };

template<typename T>
struct is_container : std::integral_constant<bool, has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value>
{ };

//     template<typename T, std::size_t N> struct is_container<T[N]> { static const bool value = true; };

//     template<std::size_t N> struct is_container<char[N]> { static const bool value = false; };


// other
// https://stackoverflow.com/questions/12042824/how-to-write-a-type-trait-is-container-or-is-vector
/*template<typename T>
struct has_const_iterator
{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

template <typename T>
struct has_begin_end
{
    template<typename C> static char (&f(typename std::enable_if<
      std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
      typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char (&f(...))[2];

    template<typename C> static char (&g(typename std::enable_if<
      std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
      typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char (&g(...))[2];

    static bool const beg_value = sizeof(f<T>(0)) == 1;
    static bool const end_value = sizeof(g<T>(0)) == 1;
};

template<typename T>
 struct is_container : std::integral_constant<bool, has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value>
 { };
*/


// other
// https://stackoverflow.com/questions/12042824/how-to-write-a-type-trait-is-container-or-is-vector
/*
 std::cout << is_container<std::vector<int>>::value << std::endl; //true
 std::cout << is_container<std::list<int>>::value << std::endl;   //true
 std::cout << is_container<std::map<int>>::value << std::endl;    //true
 std::cout << is_container<std::set<int>>::value << std::endl;    //true
 std::cout << is_container<int>::value << std::endl;              //false


template<typename T, typename _ = void>
struct is_container : std::false_type {};

template<typename... Ts>
struct is_container_helper {};

template<typename T>
struct is_container<
        T,
        std::conditional<
            false,
            is_container_helper<
                typename T::value_type,
                typename T::size_type,
                typename T::allocator_type,
                typename T::iterator,
                typename T::const_iterator,
                decltype(std::declval<T>().size()),
                decltype(std::declval<T>().begin()),
                decltype(std::declval<T>().end()),
                decltype(std::declval<T>().cbegin()),
                decltype(std::declval<T>().cend())
                >,
            void
            >
        > : public std::true_type {};


template <typename T,
          std::enable_if_t<is_mappish<T>{}, int> = 0>
auto foo(const T& items)
{
    // here be maps
}

template <typename T,
          std::enable_if_t<!is_mappish<T>{}, int> = 0>
auto foo(const T& items)
{
    // map-free zone
}
*/
