#ifndef __MEASURE_H__
#define __MEASURE_H__

#include "node.h"
#include "algorithm.h"


template <
    typename _DataType,
    template <typename...>
        class _Container
>
void
measure(std::size_t iteration, duration_map &times)
{
    duration dur;
    container_type_t<_DataType, _Container> container;
    std::vector<std::size_t> random_vec;
    
    dur = push_back<_DataType, _Container>(container, iteration);
    times["push_back"].push_back(dur);
    container.clear();
    
    dur = push_front<_DataType, _Container>(container, iteration);
    times["push_front"].push_back(dur);
    container.clear();
    
    random_vec = random_range(iteration);
    dur = insert<_DataType, _Container>(container, random_vec);
    times["insert"].push_back(dur);
    
    random_vec = random(iteration / 10);
    dur = find<_DataType, _Container>(container, random_vec);
    times["find"].push_back(dur);

    dur = ::iteration<_DataType, _Container>(container);
    times["iteration"].push_back(dur);
}


template <
    typename _DataType,
    template <typename...>
        class _Container
> requires map_like<_DataType, _Container>
void
measure(std::size_t iteration, duration_map &times)
{
    duration dur;
    container_type_t<_DataType, _Container> container;
    std::vector<std::size_t> random_vec;
    
    random_vec = random_range(iteration);
    dur = insert<_DataType, _Container>(container, random_vec);
    times["insert"].push_back(dur);
    
    random_vec = random(iteration / 10);
    dur = find<_DataType, _Container>(container, random_vec);
    times["find"].push_back(dur);

    dur = ::iteration<_DataType, _Container>(container);
    times["iteration"].push_back(dur);
}


template <
    template <typename...>
        class _Container
>
void
measure(std::size_t iteration, duration_map &times)
{
    /* End of recursion */
}

template <
    template <typename...>
        class _Container,
    typename _1,
    typename... _DataType
>
void
measure(std::size_t iteration, duration_map &times)
{
    measure<_1, _Container>(iteration, times);
    measure<_Container, _DataType...>(iteration, times);
}

template <
    template <typename...>
        class _Container,
    typename... _DataType
>
duration_map
measure(std::size_t iteration)
{
    duration_map times;
    measure<_Container, _DataType...>(iteration, times);
    return times;
}

#endif /* __MEASURE_H__ */
