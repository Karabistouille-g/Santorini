#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

namespace santorini {

enum class ActionType {
    MOVE,
    BUILD
};

struct Packet {
    ActionType type;
    int32_t workerId; // 0 ou 1
    int32_t x;
    int32_t y;
} __attribute__((packed));

}

#endif