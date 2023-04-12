#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include <array>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include "utility.h"


template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
[[clang::always_inline]]
inline void
__insert_(_ContainerType &container,
          const std::vector<std::size_t> &order,
          const std::size_t idx)
{
    container.emplace(std::next(container.begin(), order[idx]),
                      create_instance<_DataType>(idx));
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires map_like<_DataType, _Container>
[[clang::always_inline]]
inline void
__insert_(_ContainerType &container,
          [[maybe_unused]] const std::vector<std::size_t> &order,
          const std::size_t idx)
{
    container.emplace(idx, create_instance<_DataType>(idx));
}


template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
duration
insert(_ContainerType &container,
       const std::vector<std::size_t> &order)
{
    duration dur(0);
    container = _ContainerType();
    
    for (std::size_t i = 0; i < order.size(); i++) {
        clflush_container<_DataType, _Container>(container);
        START_TIMER
        __insert_<_DataType, _Container>(container, order, i);
        END_TIMER
        dur += DURATION
    }
    
    VALIDATE("insert", container.size())
    return dur;
}


template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
[[clang::always_inline]]
inline void
__push_back(_ContainerType &container,
            const std::size_t idx)
{
    container.emplace_back(create_instance<_DataType>(idx));
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
duration
push_back(_ContainerType &container,
          const std::size_t iteration_value)
{
    duration dur(0);
    container = _ContainerType();
    
    for (std::size_t i = 0; i < iteration_value; i++) {
        clflush_container<_DataType, _Container>(container);
        START_TIMER
        __push_back<_DataType, _Container>(container, i);
        END_TIMER
        dur += DURATION
    }
    
    VALIDATE("push_back", container.size())
    return dur;
}


template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
[[clang::always_inline]]
inline void
__push_front(_ContainerType &container,
             const std::size_t idx)
{
    container.emplace_front(create_instance<_DataType>(idx));
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires std::is_same_v<_ContainerType, container_type_t<_DataType, std::vector>>
[[clang::always_inline]]
inline void
__push_front(_ContainerType &container,
             const std::size_t idx)
{
    container.emplace(container.begin(), create_instance<_DataType>(idx));
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
duration
push_front(_ContainerType &container,
           const std::size_t iteration_value)
{
    duration dur(0);
    container = _ContainerType();
    
    for (std::size_t i = 0; i < iteration_value; i++) {
        clflush_container<_DataType, _Container>(container);
        START_TIMER
        __push_front<_DataType, _Container>(container, i);
        END_TIMER
        dur += DURATION
    }
    
    VALIDATE("push_front", container.size());
    return dur;
}


template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
[[clang::always_inline]]
inline typename _ContainerType::const_iterator
__find_(const _ContainerType &container,
        const std::size_t idx)
{
    return std::find_if(container.begin(),
                        container.end(),
                        [&](const _DataType &ele) {
                            return ele.node_id == idx;
                        });
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires (std::is_pointer_v<_DataType> && !map_like<_DataType, _Container>)
[[clang::always_inline]]
inline typename _ContainerType::const_iterator
__find_(const _ContainerType &container,
        const std::size_t idx)
{
    return std::find_if(container.begin(),
                        container.end(),
                        [&](const data_type_t<_DataType, _Container> &ele) {
                            return ele->node_id == idx;
                        });
}


template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires map_like<_DataType, _Container>
[[clang::always_inline]]
inline typename _ContainerType::const_iterator
__find_(const _ContainerType &container,
        const std::size_t idx)
{
    return container.find(idx);
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
duration
find(const _ContainerType &container,
     const std::vector<std::size_t> &idx) {
    std::vector<typename _ContainerType::const_iterator> iters;
    duration dur(0);
    
    for (std::size_t i = 0; i < idx.size(); i++) {
        clflush_container<_DataType, _Container>(container);
        START_TIMER
        auto iter = __find_<_DataType, _Container>(container, idx[i]);
        END_TIMER
        iters.emplace_back(iter);
        dur += DURATION
    }
    
    VALIDATE("find", iters.size());
    return dur;
}


template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
[[clang::always_inline]]
inline void
__iteration_(const _ContainerType &container,
             std::size_t &sum) {
    std::for_each(container.begin(),
                container.end(),
                [&](const typename _ContainerType::value_type &value){
                    sum += value.node_id;
                });
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires (!std::is_pointer_v<_DataType> && map_like<_DataType, _Container>)
[[clang::always_inline]]
inline void
__iteration_(const _ContainerType &container,
             std::size_t &sum) {
    std::for_each(container.begin(),
                container.end(),
                [&](const typename _ContainerType::value_type &value){
                    sum += value.second.node_id;
                });
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires (std::is_pointer_v<_DataType> && !map_like<_DataType, _Container>)
[[clang::always_inline]]
inline void
__iteration_(const _ContainerType &container,
             std::size_t &sum) {
    std::for_each(container.begin(),
                container.end(),
                [&](const typename _ContainerType::value_type &value){
                    sum += value->node_id;
                });
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
> requires (std::is_pointer_v<_DataType> && map_like<_DataType, _Container>)
[[clang::always_inline]]
inline void
__iteration_(const _ContainerType &container,
             std::size_t &sum) {
    std::for_each(container.begin(),
                container.end(),
                [&](const typename _ContainerType::value_type &value){
                    sum += value.second->node_id;
                });
}

template <
    typename _DataType,
    template <typename...>
        class _Container,
    class _ContainerType = container_type_t<_DataType, _Container>
>
duration
iteration(const _ContainerType &container) {
    clflush_container<_DataType, _Container>(container);
    std::size_t sum = 0;
    START_TIMER
    __iteration_<_DataType, _Container>(container, sum);
    END_TIMER
    VALIDATE("iteration", sum);
    return DURATION
}

#endif /* __ALGORITHM_H__ */
