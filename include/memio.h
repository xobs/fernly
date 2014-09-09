#ifndef __FV_MEMIO_H__
#define __FV_MEMIO_H__
#include <stdint.h>

static inline void writeb(uint8_t value, uint32_t addr)
{
	*((volatile uint8_t *)addr) = value;
}

static inline uint8_t readb(uint32_t addr)
{
	return *(volatile uint8_t *)addr;
}

static inline void writew(uint16_t value, uint32_t addr)
{
	*((volatile uint16_t *)addr) = value;
}

static inline uint16_t readw(uint32_t addr)
{
	return *(volatile uint16_t *)addr;
}

static inline void writel(uint32_t value, uint32_t addr)
{
	*((volatile uint32_t *)addr) = value;
}

static inline uint32_t readl(uint32_t addr)
{
	return *(volatile uint32_t *)addr;
}

#endif /* __FV_MEMIO_H__ */
