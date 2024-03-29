#ifndef TestC_h
#define TestC_h

#include <stdio.h>
#include <stdbool.h>

int getInput(void);

typedef struct {
  uint64_t value[4];
} UInt256;

bool eq256( const UInt256 *lhs, const UInt256 *rhs );

void bgraToGrayscale( unsigned char * __restrict dest, const unsigned char* src, int width, int height );
void bgraToGrayscale2( unsigned char * __restrict dest, const unsigned char* src, int width, int height );
void bgraToGrayscaleNeon (unsigned char * __restrict dest, const unsigned char * __restrict src, int width, int height);
#endif /* TestC_h */
