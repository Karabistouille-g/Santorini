#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint> 

namespace santorini {

// Types d'actions possibles
enum class ActionType {
    NONE,
    MOVE,
    BUILD,
    SURRENDER
};

// La structure exacte qu'on va envoyer sur le réseau
// __attribute__((packed)) sert à éviter que le compilateur ajoute des trous de mémoire
struct Packet {
    ActionType type;
    int32_t workerId; // 1 ou 2 (quel Builder ?)
    int32_t x;
    int32_t y;
} __attribute__((packed));

}

#endif // COMMON_HPP