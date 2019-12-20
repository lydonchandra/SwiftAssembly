#ifndef TestC_h
#define TestC_h

#include <stdio.h>
#include <stdbool.h>

int getInput(void);

typedef struct {
  uint64_t value[4];
} UInt256;

bool eq256( const UInt256 *lhs, const UInt256 *rhs);

#endif /* TestC_h */
