#ifndef __UTILITY_H__
#define __UTILITY_H__

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


template <typename _T, template <typename...> class _Container>
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
    typename _DataType,
    typename... _Args
>
inline _DataType
create_instance(const _Args& ...args)
{
    return _DataType(args...);
}

template <
    typename _DataType,
    typename... _Args
> requires std::is_pointer_v<_DataType>
inline std::unique_ptr<std::remove_pointer_t<_DataType>>
create_instance(const _Args& ...args)
{
    return std::make_unique<std::remove_pointer_t<_DataType>>(args...);
}


std::vector<std::size_t>
random_range(std::size_t iteration)
{
    std::vector<std::size_t> vec(iteration);
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (int i = 0; i < iteration; i++) {
        std::uniform_int_distribution<std::size_t> dis(0, i);
        vec[i] = dis(gen);
    }
    
    return vec;
}

std::vector<std::size_t>
random(std::size_t size)
{
    std::vector<std::size_t> vec(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> dis(0, size);
    
    for (int i = 0; i < size; i++) {
        vec[i] = dis(gen);
    }
    
    return vec;
}


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


#ifdef __DONT_FLUSH_CACHE
template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
void
__clflush_range(const _ContainerType &container) {}
#else
template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
void
__clflush_range(const _ContainerType &container) {
    std::for_each(container.begin(),
                container.end(),
                [](const typename _ContainerType::value_type &ele) {
                    _mm_clflush(&ele);
                });
    _mm_clflush(&container);
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires (!map_like<_DataType, _Container> && std::is_pointer_v<_DataType>)
void
__clflush_range(const _ContainerType &container) {
    std::for_each(container.begin(),
                container.end(),
                [](const typename _ContainerType::value_type &ele) {
                    _mm_clflush(ele.get());
                    _mm_clflush(&ele);
                });
    _mm_clflush(&container);
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires (map_like<_DataType, _Container> && !std::is_pointer_v<_DataType>)
void
__clflush_range(const _ContainerType &container) {
    std::for_each(container.begin(),
                container.end(),
                [](const typename _ContainerType::value_type &ele) {
                    _mm_clflush(&(ele.second));
                    _mm_clflush(&ele);
                });
    _mm_clflush(&container);
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires (map_like<_DataType, _Container> && std::is_pointer_v<_DataType>)
void
__clflush_range(const _ContainerType &container) {
    std::for_each(container.begin(),
                container.end(),
                [](const typename _ContainerType::value_type &ele) {
                    _mm_clflush(ele.second.get());
                    _mm_clflush(&ele);
                });
    _mm_clflush(&container);
}
#endif


template <
    typename _T
>
inline void
print_sizeof()
{
    std::cout << "size of " << _T::node_string << ": " << sizeof(_T) << '\n';
}

template <
    typename _1,
    typename... _T
> requires (sizeof...(_T) > 0)
inline void
print_sizeof()
{
    print_sizeof<_1>();
    print_sizeof<_T...>();
}

std::stringstream
print_container(std::size_t count,
                const std::string &container_str) {
    std::size_t total_len = count * 12 + 12;
    std::size_t head_len = total_len - (container_str.length() + 2);
    std::size_t head_left = head_len / 2;
    std::size_t head_right = head_len - head_left;
    std::stringstream head_line;
    
    for (std::size_t i = 0; i < head_left; i++)
        head_line << '=';
    head_line << ' ' << container_str << ' ';
    for (std::size_t i = 0; i < head_right; i++)
        head_line << '=';
    head_line << '\n';
    
    return head_line;
}

std::stringstream
print_dotted_line(std::size_t count) {
    std::size_t total_len = count * 12 + 12;
    std::stringstream dotted_line;
    
    for (std::size_t i = 0; i < total_len; i++)
        dotted_line << '-';
    dotted_line << '\n';
    
    return dotted_line;
}

std::stringstream
print_end_line(std::size_t count) {
    std::size_t total_len = count * 12 + 12;
    std::stringstream end_line;
    
    for (std::size_t i = 0; i < total_len; i++)
        end_line << '=';
    
    return end_line;
}

void print(const duration_map &times,
           const std::vector<std::string> &data_type,
           const std::string &container_str) {
    std::cout << '\n';
    std::cout << print_container(data_type.size() + 1, container_str).str();
    
    std::cout << "Data type";
    for (auto& ele : data_type)
        std::cout << "\t|" << std::setw(10) << ele;
    std::cout << std::endl;
    
    std::cout << print_dotted_line(data_type.size() + 1).str();
    
    for (auto& pack : times) {
        std::cout << (pack.first.length() > 8 ? pack.first : pack.first + '\t');
        for (auto& ele : pack.second)
            std::cout << "\t|" << std::setw(10)
#ifdef __USE_CHRONO_TIMER
                << std::chrono::duration_cast<time_unit>(ele).count();
#else
                << ele / 1000;
#endif
        std::cout << std::endl;
    }
    
    std::cout << print_end_line(data_type.size() + 1).str();
    std::cout << '\n';
}

void print_csv(const duration_map &times,
               const std::vector<std::string> &data_type,
               const std::string &path) {
    std::ofstream file(path);
    std::stringstream sstream;
    
    sstream << "function";
    for (auto& ele : data_type)
        sstream << ',' << ele;
    sstream << '\n';
    
    for (auto& pack : times) {
        sstream << pack.first;
        for (auto& ele : pack.second) {
            sstream << ','
#ifdef __USE_CHRONO_TIMER
                << std::chrono::duration_cast<time_unit>(ele).count();
#else
                << ele;
#endif
        }
        sstream << '\n';
    }
    
    file << sstream.str();
    file.close();
}

#endif /* __UTILITY_H__ */
