#include <iostream>

namespace Proxmox::Utils{

enum ByteSize{
    O,
    KO,
    MO,
    GO,
    TO
};

inline size_t convert(ByteSize from, ByteSize to, size_t initial_size){
    size_t factor[] = {1, 1024, 1024 * 1024, 1024 * 1024 * 1024, 1024ULL * 1024 * 1024 * 1024};
    return initial_size * factor[from] / factor[to];
}

};