#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "__type.h"
#include "__clflush.h"


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
