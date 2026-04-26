#ifndef RAND_H
#define RAND_H

#include <stddef.h>
#include <stdint.h>

void rand_bytes(uint8_t *buf, size_t len);

#endif