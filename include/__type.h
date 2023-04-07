#ifndef __TYPE_H__
#define __TYPE_H__

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <utility>
#include <random>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <x86intrin.h>


#ifdef __USE_CHRONO_TIMER
    typedef std::chrono::microseconds                       time_unit;
    typedef std::chrono::high_resolution_clock              timer;
    typedef timer::duration                                 duration;
    typedef timer::time_point                               time_point;

    typedef std::vector<duration>                           duration_vec;
    typedef std::unordered_map<std::string, duration_vec>   duration_map;

    #define START_TIMER         time_point start_time = timer::now();
    #define END_TIMER           time_point end_time = timer::now();
    #define DURATION            end_time - start_time;
#else
    typedef unsigned long long                              time_point;
    typedef unsigned long long                              duration;

    typedef std::vector<duration>                           duration_vec;
    typedef std::unordered_map<std::string, duration_vec>   duration_map;

    #define START_TIMER         time_point start_time = __rdtsc();
    #define END_TIMER           time_point end_time = __rdtsc();
    #define DURATION            end_time - start_time;
#endif

#define VALIDATE(x, y) std::cout << "validation - " << x << ": " << y << std::endl;


template <
    typename _T
>
struct is_unique_ptr
    : public std::false_type {};

template <
    typename _T
>
struct is_unique_ptr<std::unique_ptr<_T>>
    : public std::true_type {};

template <
    typename _T
>
inline constexpr bool is_unique_ptr_v = is_unique_ptr<_T>::value;


template <
    typename _T
>
struct is_unique_ptr_pair
    : public std::false_type {};

template <
    typename _1,
    typename _2
> requires is_unique_ptr_v<_2>
struct is_unique_ptr_pair<std::pair<_1, _2>>
    : public std::true_type {};

template <
    typename _T
>
inline constexpr bool is_unique_ptr_pair_v = is_unique_ptr_pair<_T>::value;


template <
    typename _T,
    template <typename...>
        class _Container
>
concept map_like =
    std::is_same_v<std::map<std::size_t, _T>, _Container<std::size_t, _T>> ||
    std::is_same_v<std::unordered_map<std::size_t, _T>, _Container<std::size_t, _T>>;


template <
    typename _DataType,
    template <typename...>
        class _Container
>
struct container_type {
    typedef _DataType                           data_type;
    typedef _Container<_DataType>               type;
};

template <
    typename _DataType,
    template <typename...>
        class _Container
> requires map_like<_DataType, _Container>
struct container_type<_DataType, _Container> {
    typedef _DataType                           data_type;
    typedef _Container<std::size_t, _DataType>  type;
};

template <
    typename _DataType,
    template <typename...>
        class _Container
>
struct container_type<_DataType*, _Container> {
    typedef std::unique_ptr<_DataType>          data_type;
    typedef _Container<data_type>               type;
};

template <
    typename _DataType,
    template <typename...>
        class _Container
> requires map_like<_DataType, _Container>
struct container_type<_DataType*, _Container> {
    typedef std::unique_ptr<_DataType>          data_type;
    typedef _Container<std::size_t, data_type>  type;
};

template <
    typename _DataType,
    template <typename...>
        class _Container
>
using container_type_t = typename container_type<_DataType, _Container>::type;

template <
    typename _DataType,
    template <typename...>
        class _Container
>
using data_type_t = typename container_type<_DataType, _Container>::data_type;


template <
    typename _DataType
>
constexpr std::string_view
__data_type() {
    return _DataType::node_string;
}

template <
    typename _DataType
> requires std::is_pointer_v<_DataType>
std::string
__data_type() {
    typedef std::remove_pointer_t<_DataType> value_type;
    std::string size{ std::to_string(sizeof(value_type)) };
    return "ptr_" + size;
}


template <
    typename _DataType
>
void
__data_type(std::vector<std::string> &vec) {
    vec.emplace_back(__data_type<_DataType>());
}

template <
    typename _1,
    typename... _DataType
> requires (sizeof...(_DataType) > 0)
void
__data_type(std::vector<std::string> &vec) {
    vec.emplace_back(__data_type<_1>());
    __data_type<_DataType...>(vec);
}

template <
    typename... _DataType
>
constexpr std::vector<std::string>
get_data_types() {
    std::vector<std::string> vec;
    __data_type<_DataType...>(vec);
    return vec;
}

#endif /* __TYPE_H__ */
