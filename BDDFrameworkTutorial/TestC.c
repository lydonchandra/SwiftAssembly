#include "TestC.h"
#include <arm_neon.h>


int getInput(void) {
  return 42;
}

void bgraToGrayscale( unsigned char * __restrict dest, const unsigned char* src, uint width, uint height ) {
  uint BYTES_PER_PIXEL = 4;
  uint numPixels = width * height;
  
  for( uint idx=0; idx < numPixels; idx++ ) {
      
    float weight = ((float)src[0]) * 0.11 +
                  ((float)src[1]) * 0.59 +
                  ((float)src[2]) * 0.3;
    
    dest[0] = (int)weight;
    dest[1] = (int)weight;
    dest[2] = (int)weight;
    dest[3] = src[3];
    dest += BYTES_PER_PIXEL;
    src += BYTES_PER_PIXEL;
  }
}

void bgraToGrayscaleNeon (unsigned char * __restrict dest, unsigned char * __restrict src, uint numPixels)
{
    int i;
      uint8x8_t rfac = vdup_n_u8 (77);
      uint8x8_t gfac = vdup_n_u8 (151);
      uint8x8_t bfac = vdup_n_u8 (28);
      int n = numPixels / 8;

      // Convert per eight pixels
      for (i=0; i < n; ++i)
      {
          uint16x8_t  temp;
          uint8x8x4_t rgb  = vld4_u8 (src);
          uint8x8_t result;

          temp = vmull_u8 (rgb.val[0],      bfac);
          temp = vmlal_u8 (temp,rgb.val[1], gfac);
          temp = vmlal_u8 (temp,rgb.val[2], rfac);

          result = vshrn_n_u16 (temp, 8);
          vst1_u8 (dest, result);
          src  += 8*4;
          dest += 8;
      }
}
