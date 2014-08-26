#include <stdint.h>

inline void writeb(uint8_t value, uint32_t addr)
{
	*((volatile uint8_t *)addr) = value;
}

inline uint8_t readb(uint32_t addr)
{
	return *(volatile uint8_t *)addr;
}

inline void writew(uint16_t value, uint32_t addr)
{
	*((volatile uint16_t *)addr) = value;
}

inline uint16_t readw(uint32_t addr)
{
	return *(volatile uint16_t *)addr;
}

inline void writel(uint32_t value, uint32_t addr)
{
	*((volatile uint32_t *)addr) = value;
}

inline uint32_t readl(uint32_t addr)
{
	return *(volatile uint32_t *)addr;
}
