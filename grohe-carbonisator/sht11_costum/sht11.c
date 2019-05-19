#include "config.h"
#include "sht11.h"

#define SHT11_CMD_TEMP	0x03
#define SHT11_CMD_HUMID	0x05
#define SHT11_CMD_WSTAT	0x06
#define SHT11_CMD_RSTAT	0x07
#define SHT11_CMD_RESET	0x1E

/////////////////////////////////////////////////////////////////////////////
// This version needs external pullups on SDA!
/////////////////////////////////////////////////////////////////////////////

static void	delay(void)	{ _delay_us(5); }

static void	scl_hi(void)    { setBits(PORT(SHT11_PORT), SHT11_SCL); }
static void	scl_lo(void)    { clrBits(PORT(SHT11_PORT), SHT11_SCL); }
static void	sda_hi(void)    { clrBits(DDR(SHT11_PORT), SHT11_SDA); }
static void	sda_lo(void)    { setBits(DDR(SHT11_PORT), SHT11_SDA); }
static void	scl_pulse(void) { scl_hi(); delay(); scl_lo(); }
static uint8_t	sda_val(void)   { return (PIN(SHT11_PORT) & SHT11_SDA) != 0; }

/////////////////////////////////////////////////////////////////////////////
static uint8_t i;
void
send(uint16_t b)
{
    for (i = 0; i < 8; ++i) {
	if (b & 0x80)
	    sda_hi();
	else
	    sda_lo();
	b <<= 1;
	delay();
	scl_pulse();
    }
    sda_hi();
    delay();
    scl_pulse();
}

static uint8_t
recv_data(void)
{
    uint8_t b = 0;
    for (i = 0; i < 8; ++i) {
	b <<= 1;
	if (sda_val())
	    b |= 1;
	scl_pulse();
	delay();
    }
    sda_lo();
    delay();
    scl_pulse();
    sda_hi();
    delay();
    return b;
}
static uint8_t
recv_data_noack(void)
{
    uint8_t b = 0;
    for (i = 0; i < 8; ++i) {
	b <<= 1;
	if (sda_val())
	    b |= 1;
	scl_pulse();
	delay();
    }
    return b;
}

static void
start(void)
{
    clrBits(PORT(SHT11_PORT), SHT11_SCL | SHT11_SDA);	// SCK output low, SDA input/high
    setBits(DDR(SHT11_PORT),  SHT11_SCL);
    clrBits(DDR(SHT11_PORT),  SHT11_SDA);
    delay();

    // reset communication
    for (i = 0; i < 10; ++i) {
	scl_pulse();
	delay();
    }

    // "start" sequence
    scl_hi(); delay();
    sda_lo(); delay();
    scl_lo(); delay();
    scl_hi(); delay();
    sda_hi(); delay();
    scl_lo(); delay();
}

/////////////////////////////////////////////////////////////////////////////
// Measurement sequence.

void
sht11_start_temp(void)
{
    start();
    send(SHT11_CMD_TEMP);
}

void
sht11_start_humid(void)
{
    start();
    send(SHT11_CMD_HUMID);
}

uint8_t
sht11_ready(void)
{
    return sda_val() == 0;
}

static int16_t
result(void)
{
    if (!sht11_ready())
	return 0;
    int16_t v = recv_data() << 8; v |= recv_data_noack();
    return v;
}

int16_t
sht11_result_temp(void)
{
    int16_t v = result();
#if SHT11_LOWRES
	v = v * 4 - 4000;
#else
	v -= 4000;
#endif
    return v;
}

int16_t
sht11_result_humid(void)
{
    int16_t v = result();
#if SHT11_LOWRES
	// inspired by Werner Hoch, modified for low resolution mode
	const int32_t C1 = (int32_t)(-4.0 * 100);
	const int32_t C2 = (int32_t)(0.648 * 100 * (1L<<24));
	const int32_t C3 = (int32_t)(-7.2e-4 * 100 * (1L<<24));
	v = (int16_t)((((C3 * v + C2) >> 7) * v + (1L<<16)) >> 17) + C1;
#else
	// inspired by Werner Hoch
	const int32_t C1 = (int32_t)(-4.0 * 100);
	const int32_t C2 = (int32_t)(0.0405 * 100 * (1L<<28));
	const int32_t C3 = (int32_t)(-2.8e-6 * 100 * (1L<<30));
	v = (int16_t)((((((C3 * v) >> 2) + C2) >> 11) * v + (1L<<16)) >> 17) + C1;
#endif
   return v;
}

/////////////////////////////////////////////////////////////////////////////
// Initialize.

void
sht11_init(void)
{
    start();
    send(SHT11_CMD_RESET);
    _delay_ms(11);

    start();
    send(SHT11_CMD_WSTAT);
    send(SHT11_LOWRES);
}

