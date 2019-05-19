#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// SHT11 hum/temp sensor
#define SHT11_PORT	A
#define SHT11_SCL	(1<<PA5)
#define SHT11_SDA	(1<<PA6)
#define SHT11_LOWRES	0		// 0: 12/14bit (SHT11), 1: 8/12bit (SHT11+FOST02)

#define GLUE(a, b)	a##b
#define PORT(x)		GLUE(PORT, x)
#define PIN(x)		GLUE(PIN, x)
#define DDR(x)		GLUE(DDR, x)

#define setBits(port,mask)	do{ (port) |=  (mask); }while(0)
#define clrBits(port,mask)	do{ (port) &= ~(mask); }while(0)
#define tstBits(port,mask)	((port) & (mask))

