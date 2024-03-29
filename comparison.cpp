//#define __USE_CHRONO_TIMER
//#define __DONT_FLUSH_CACHE

#include "measure.h"


int main() {
    print_sizeof<
        node_int64,
        node_small,
        node_medium,
        node_large,
        node_extreme>();

    static constexpr std::size_t iteration = 50;
    auto data_type = get_data_types<
        node_int64,
        node_small,
        node_medium,
        node_large,
        node_extreme,
        node_int64*,
        node_small*,
        node_extreme*
    >();

    duration_map vector_dur, map_dur, hash_map_dur;
    static constexpr std::size_t loop = 3;
    
    for (std::size_t i = 0; i < loop; i++)
        vector_dur = measure<std::vector,
            node_int64,
            node_small,
            node_medium,
            node_large,
            node_extreme,
            node_int64*,
            node_small*,
            node_extreme*
        >(iteration);
    
    for (std::size_t i = 0; i < loop; i++)
        map_dur = measure<std::map,
            node_int64,
            node_small,
            node_medium,
            node_large,
            node_extreme,
            node_int64*,
            node_small*,
            node_extreme*
        >(iteration);
    
    for (std::size_t i = 0; i < loop; i++)
        hash_map_dur = measure<std::unordered_map,
            node_int64,
            node_small,
            node_medium,
            node_large,
            node_extreme,
            node_int64*,
            node_small*,
            node_extreme*
        >(iteration);
    
    print(vector_dur, data_type, "VECTOR");
    print(map_dur, data_type, "MAP");
    print(hash_map_dur, data_type, "HASH MAP");
    
    std::stringstream identifier;
    identifier
#ifdef __USE_CHRONO_TIMER
        << "_chrono"
#else
        << "_rdtsc"
#endif
#ifdef __DONT_FLUSH_CACHE
        << "_nonflush_"
#else
        << "_flush_"
#endif
        << iteration
        << ".csv";
    
    print_csv(vector_dur, data_type, "vector" + identifier.str());
    print_csv(map_dur, data_type, "map" + identifier.str());
    print_csv(hash_map_dur, data_type, "hashmap" + identifier.str());
}
