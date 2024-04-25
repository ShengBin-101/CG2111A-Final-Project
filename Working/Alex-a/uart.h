#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include "cbuffer.h"

class uart
{
public:
    uart() = default;
    ~uart() = default;
    void begin(const unsigned long baud);
    void end(void);
    bool write(uint8_t c);
    void write(const char *c, int len);

    uint8_t read(void);
    bool available(void) const;
    void _rx_interrupt(void);
    void _tx_interrupt(void);

private:
    void turn_tx_on(void)
    {
        UCSR0B |= (1 << UDRIE0);
    }

    void turn_tx_off(void)
    {
        UCSR0B &= ~(1 << UDRIE0);
    }
    volatile CBuffer<uint8_t, 512> rx_buffer;
    volatile CBuffer<uint8_t, 512> tx_buffer;
};

extern uart serial;

#endif
