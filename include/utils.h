#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdint.h>

# define do_div(n,base) ({                              \
        uint32_t __base = (base);                       \
        uint32_t __rem;                                 \
        (void)(((typeof((n)) *)0) == ((uint64_t *)0));  \
        if (((n) >> 32) == 0) {                 \
                __rem = (uint32_t)(n) % __base;         \
                (n) = (uint32_t)(n) / __base;           \
        } else                                          \
                __rem = __div64_32(&(n), __base);       \
        __rem;                                          \
 })

uint32_t _udiv64(uint64_t n, uint32_t d);
uint32_t __div64_32(uint64_t *n, uint32_t base);

#endif /* __UTILS_H__ */
