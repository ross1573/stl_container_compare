#ifndef __NODE_H__
#define __NODE_H__

#include <string>
#include <array>
#include <bitset>


class node_int64 {
public:
    static constexpr std::string_view node_string = "int64";
    
    std::size_t node_id;
    
    node_int64(std::size_t node_id) : node_id(node_id) {}
};

class node_small {
public:
    static constexpr std::string_view node_string = "small";
    
    std::size_t node_id;
    std::size_t node_type       = 0;
    std::string name            = "small";
    
    node_small(std::size_t node_id) : node_id(node_id) {}
};

class node_medium {
public:
    static constexpr std::string_view node_string = "medium";
    
    std::size_t node_id;
    std::size_t node_type       = 1;
    std::string name            = "medium";
    std::string sprite_id       = "medium";
    std::string description     = "medium";
    
    node_medium(std::size_t node_id) : node_id(node_id) {}
};

class node_large {
public:
    static constexpr std::string_view node_string = "large";
    
    std::size_t node_id;
    std::size_t node_type       = 2;
    std::string name            = "large";
    std::string sprite_id       = "large";
    std::string description     = "large";
    std::array<double, 8> data  = { 0.0, };
    
    node_large(std::size_t node_id) : node_id(node_id) {}
};

class node_extreme {
public:
    static constexpr std::string_view node_string = "extreme";
    
    std::size_t node_id;
    std::size_t node_type       = 3;
    std::string name            = "extreme";
    std::string sprite_id       = "extreme";
    std::string description     = "extreme";
    std::array<double, 8> _1    = { 0.0, };
    std::array<long, 4> _2      = { 0, };
    std::bitset<3> _3           = { 0, };
    std::array<double, 7> _4    = { 0.0, };
    
    node_extreme(std::size_t node_id) : node_id(node_id) {}
};

#endif /* __NODE_H__ */
