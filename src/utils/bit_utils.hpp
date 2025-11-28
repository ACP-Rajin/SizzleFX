#ifndef BIT_UTILS_HPP
#define BIT_UTILS_HPP

#include <cstdint>

void set(uint8_t *byte,uint8_t n){
  *byte = *byte | (1u << n);
}
void clear(uint8_t *byte,uint8_t n){
  *byte = *byte & ~(1u << n);
}
void toggle(uint8_t *byte,uint8_t n){
  *byte = *byte ^ (1u << n);
}
void read(uint8_t *byte,uint8_t n){
  return (*byte >> n) & 1u;
}

#endif
