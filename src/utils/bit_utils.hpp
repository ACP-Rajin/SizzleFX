#ifndef BIT_UTILS_HPP
#define BIT_UTILS_HPP

#include <cstdint>

namespace bit{
  inline void set(uint8_t *byte,uint8_t n){
    *byte = *byte | (1u << n);
  }
  inline void clear(uint8_t *byte,uint8_t n){
    *byte = *byte & ~(1u << n);
  }
  inline void toggle(uint8_t *byte,uint8_t n){
    *byte = *byte ^ (1u << n);
  }
  inline uint8_t read(uint8_t *byte,uint8_t n){
    return (*byte >> n) & 1u;
  }
}
#endif
