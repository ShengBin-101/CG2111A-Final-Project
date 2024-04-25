// Implementation details of uart class
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "cbuffer.h"

void uart::begin(const unsigned long baud)
{
    // Define UCSR0A
    UCSR0A = 0;

    uint16_t ubrr = (F_CPU / (16 * baud)) - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    // Frame format of 8N1
    UCSR0C = 0;
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
    // Enable TX and RX
    UCSR0B = 0;
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    // Enable RX interrupt
    UCSR0B |= (1 << RXCIE0);
}

void uart::end(void)
{
    UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
    this->turn_tx_off();
    UCSR0B &= ~(1 << RXCIE0);
}

bool uart::write(uint8_t ch)
{
    if (this->tx_buffer.is_full())
    {
        return false;
    }

    // Writing straight if UDR0 is empty and buffer empty
    if ((UCSR0A & (1 << UDRE0)) && this->tx_buffer.is_empty())
    {
        UDR0 = ch;
    }
    else
    {
        this->tx_buffer.write(&ch);
        this->turn_tx_on();
    }
    return true;
}

void uart::write(const char *c, int len)
{
    for (int i = 0; i < len; i++)
    {
        this->write(c[i]);
    }
}

uint8_t uart::read(void)
{
    bool status;
    uint8_t data;
    status = this->rx_buffer.read(&data);
    if (!status)
    {
        return -1;
    }
    return data;
}

bool uart::available(void) const
{
    if (this->rx_buffer.is_empty())
    {
        return false;
    }
    return true;
}

void uart::_rx_interrupt(void)
{
    uint8_t data = UDR0;
    this->rx_buffer.write(&data);
}

void uart::_tx_interrupt(void)
{
    if (this->tx_buffer.is_empty())
    {
        this->turn_tx_off();
    }
    else
    {
        uint8_t data;
        this->tx_buffer.read(&data);
        UDR0 = data;
    }
}

uart serial;

ISR(USART0_RX_vect)
{
    serial._rx_interrupt();
}

ISR(USART0_UDRE_vect)
{
    serial._tx_interrupt();
}