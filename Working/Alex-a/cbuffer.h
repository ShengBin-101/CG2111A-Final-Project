// Implementation of circular buffer
#ifndef CBUFFER_H
#define CBUFFER_H

#include <stdint.h>

// Give type and size of data type
template <typename T, int size>
class CBuffer
{
private:
    volatile T array[size];
    volatile int head = 0;
    volatile int tail = 0;

public:
    CBuffer(void) = default;
    bool write(T *data);
    bool read(T *data);
    bool is_full(void);
    bool is_empty(void);
};

template <typename T, int size>
bool CBuffer<T, size>::write(T *data)
{
    if (this->is_full())
    {
        return false;
    }

    this->head = (this->head + 1) % size;
    this->array[this->head] = *data;
    return true;
}

template <typename T, int size>
bool CBuffer<T, size>::read(T *data)
{
    if (this->is_empty())
    {
        return false;
    }
    this->tail = (this->tail + 1) % size;
    *data = array[tail];

    return true;
}

template <typename T, int size>
bool CBuffer<T, size>::is_full(void)
{
    if ((this->head + 1) % size == this->tail)
    {
        return true;
    }
    return false;
}

template <typename T, int size>
bool CBuffer<T, size>::is_empty(void)
{
    if (this->head == this->tail)
    {
        return true;
    }
    return false;
}
#endif
