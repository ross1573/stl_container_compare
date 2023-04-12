#ifndef __CLFLUSH_H__
#define __CLFLUSH_H__

#include "__type.h"


#ifdef __DONT_FLUSH_CACHE
template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
void
clflush_container(const _ContainerType &container) {}

#else
template <
    typename _DataType
>
[[clang::always_inline]]
inline void
__clflush_range(const _DataType &begin,
                const std::size_t &count) {
    static constexpr int __l_size = 64;
    const char* __ptr = (const char*)&begin;
    uintptr_t __last = (uintptr_t)&begin + (sizeof(_DataType) * count) - 1;
    __last = __last | (__l_size - 1);
    
    do {
        _mm_clflush(__ptr);
        __ptr += __l_size;
    }
    while (__ptr <= (const char*)__last);
}

template <
    typename _DataType
>
[[clang::always_inline]]
inline void
__clflush_(const _DataType &data) {
    _mm_clflush(&data);
}

template <
    typename _DataType
> requires (sizeof(_DataType) > 64)
[[clang::always_inline]]
inline void
__clflush_(const _DataType &data) {
    __clflush_range(data, 1);
}

template <
    typename _DataType
> requires is_unique_ptr_v<_DataType>
[[clang::always_inline]]
inline void
__clflush_(const _DataType &data) {
    __clflush_(*(data.get()));
}

template <
    typename _DataType
> requires is_unique_ptr_pair_v<_DataType>
[[clang::always_inline]]
inline void
__clflush_(const _DataType &data) {
    __clflush_(data.second);
    __clflush_range(data, 1);
}

template <
    typename _Iterator
>
[[clang::always_inline]]
inline void
__clflush_iter(const _Iterator &begin,
               const _Iterator &end) {
    _Iterator iter = begin;
    while(iter != end) {
        _Iterator next = std::next(iter);
        __clflush_(*iter);
        iter = next;
    }
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
void
clflush_container(const _ContainerType &container) {
    if (container.size() != 0)
        __clflush_iter(container.begin(), container.end());
    _mm_clflush(&container);
}

template <
    typename _Iterator
>
[[clang::always_inline]]
inline void
__clflush_array(const _Iterator &begin,
                const std::size_t &count) {
    __clflush_range(*begin, count);
}

template <
    typename _Iterator
> requires is_unique_ptr_v<typename std::iterator_traits<_Iterator>::value_type>
[[clang::always_inline]]
inline void
__clflush_array(const _Iterator &begin,
                const std::size_t &count) {
    __clflush_iter(begin, std::next(begin, (long)count));
    __clflush_range(*begin, count);
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires std::is_same_v<_ContainerType, container_type_t<_DataType, std::vector>>
void
clflush_container(const _ContainerType &container) {
    if (container.size() != 0)
        __clflush_array(container.begin(), container.size());
    _mm_clflush(&container);
}
#endif

#endif /* __CLFLUSH_H__ */
