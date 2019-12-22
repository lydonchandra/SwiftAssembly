#include "TestC.h"
#include <arm_neon.h>


int getInput(void) {
  return 42;
}

void bgraToGrayscale( unsigned char * __restrict dest, const unsigned char* src, int width, int height ) {
  int BYTES_PER_PIXEL = 4;
  int numPixels = width * height;
  
  for( int idx=0; idx < numPixels; idx++ ) {
      
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

void bgraToGrayscale2( unsigned char * __restrict dest, const unsigned char * __restrict src, int width, int height) {
  int idx;
  int r, g, b, a;
  int y, yNorm;
  int numPixels = width * height;
  int BYTES_PER_PIXEL = 4;

  for( idx=0; idx < numPixels; idx++ ) {
    r = *src++;
    g = *src++;
    b = *src++;
    a = *src++;
    
    y = (r * 77) + (g * 151) + (b * 28);
    yNorm = y >> 8;
    dest[0] = yNorm;
    dest[1] = yNorm;
    dest[2] = yNorm;
    dest[3] = a;
    dest += BYTES_PER_PIXEL;
  }
}


void bgraToGrayscaleNeon (unsigned char * __restrict dest, const unsigned char * __restrict src, int width, int height)
{
  int i;
  //vdup_n_u8(77) : load 77 onto all 8 lanes
  uint8x8_t r_fac = vdup_n_u8 (77);
  uint8x8_t g_fac = vdup_n_u8 (151);
  uint8x8_t b_fac = vdup_n_u8 (28);
  int numPixels = width * height;
  int n = numPixels / 8;

  // Convert per eight pixels
  for (i=0; i < n; ++i)
  {
    //vld4_u8 load 4-element structure
    //uint8x8x4_t vld4_u8(__transfersize(32) uint8_t const * ptr);
    //VLD4.8 {d0, d1, d2, d3}, [r0]
    //VLD4 loads four registers and deinterleaves. Use it to process ARGB image data.
    uint8x8x4_t rgb  = vld4_u8 (src);
    uint8x8x4_t result;
    uint16x8_t grayValTemp;
    uint8x8_t grayVal;

    //vmull_u8 = vector long multiply unsigned8
    //uint16x8_t vmull_u8(uint8x8_t a, uint8x8_t b);    // VMULL.U8 q0,d0,d0
    grayValTemp = vmull_u8 (rgb.val[0],      b_fac);
    
    //VMLAL multiplies each element in a vector by a scalar, and accumulates the results into the corresponding elements of the destination vector.
    grayValTemp = vmlal_u8 (grayValTemp, rgb.val[1], g_fac);
    grayValTemp = vmlal_u8 (grayValTemp, rgb.val[2], r_fac);

    grayVal = vshrn_n_u16 (grayValTemp, 8);
    //vector shift right
    result.val[0] = grayVal;
    result.val[1] = grayVal;
    result.val[2] = grayVal;
    //alpha channel at result.val[3] is left intact, not needed to display grayscale image
    //result.val[3] = grayVal;

    //Store a single vector into memory
    //void  vst1_u8(__transfersize(8) uint8_t * ptr, uint8x8_t val); // VST1.8 {d0}, [r0]
    vst4_u8 (dest, result);
    
    src  += 8*4;
    dest += 8*4;
  }
}


//BDDFrameworkTutorial`bgraToGrayscale:
//    0x1023c2340 <+0>:   sub    sp, sp, #0x30             ; =0x30
//    0x1023c2344 <+4>:   str    x0, [sp, #0x28]
//    0x1023c2348 <+8>:   str    x1, [sp, #0x20]
//    0x1023c234c <+12>:  str    w2, [sp, #0x1c]
//    0x1023c2350 <+16>:  str    w3, [sp, #0x18]
//->  0x1023c2354 <+20>:  orr    w2, wzr, #0x4
//    0x1023c2358 <+24>:  str    w2, [sp, #0x14]
//    0x1023c235c <+28>:  ldr    w2, [sp, #0x1c]
//    0x1023c2360 <+32>:  ldr    w3, [sp, #0x18]
//    0x1023c2364 <+36>:  mul    w2, w2, w3
//    0x1023c2368 <+40>:  str    w2, [sp, #0x10]
//    0x1023c236c <+44>:  str    wzr, [sp, #0xc]
//    0x1023c2370 <+48>:  ldr    w8, [sp, #0xc]
//    0x1023c2374 <+52>:  ldr    w9, [sp, #0x10]
//    0x1023c2378 <+56>:  cmp    w8, w9
//    0x1023c237c <+60>:  b.ge   0x1023c2460               ; <+288> at TestC.c:26:5
//    0x1023c2380 <+64>:  adrp   x8, 19
//    0x1023c2384 <+68>:  ldr    d0, [x8, #0x460]
//    0x1023c2388 <+72>:  adrp   x8, 19
//    0x1023c238c <+76>:  ldr    d1, [x8, #0x468]
//    0x1023c2390 <+80>:  adrp   x8, 19
//    0x1023c2394 <+84>:  ldr    d2, [x8, #0x470]
//    0x1023c2398 <+88>:  ldr    x8, [sp, #0x20]
//    0x1023c239c <+92>:  ldrb   w9, [x8]
//    0x1023c23a0 <+96>:  uxtb   w9, w9
//    0x1023c23a4 <+100>: ucvtf  s3, w9
//    0x1023c23a8 <+104>: fcvt   d4, s3
//    0x1023c23ac <+108>: fmul   d2, d4, d2
//    0x1023c23b0 <+112>: ldr    x8, [sp, #0x20]
//    0x1023c23b4 <+116>: ldrb   w9, [x8, #0x1]
//    0x1023c23b8 <+120>: uxtb   w9, w9
//    0x1023c23bc <+124>: ucvtf  s3, w9
//    0x1023c23c0 <+128>: fcvt   d4, s3
//    0x1023c23c4 <+132>: fmul   d1, d4, d1
//    0x1023c23c8 <+136>: fadd   d1, d2, d1
//    0x1023c23cc <+140>: ldr    x8, [sp, #0x20]
//    0x1023c23d0 <+144>: ldrb   w9, [x8, #0x2]
//    0x1023c23d4 <+148>: uxtb   w9, w9
//    0x1023c23d8 <+152>: ucvtf  s3, w9
//    0x1023c23dc <+156>: fcvt   d2, s3
//    0x1023c23e0 <+160>: fmul   d0, d2, d0
//    0x1023c23e4 <+164>: fadd   d0, d1, d0
//    0x1023c23e8 <+168>: fcvt   s3, d0
//    0x1023c23ec <+172>: str    s3, [sp, #0x8]
//    0x1023c23f0 <+176>: ldr    s3, [sp, #0x8]
//    0x1023c23f4 <+180>: fcvtzs w9, s3
//    0x1023c23f8 <+184>: ldr    x8, [sp, #0x28]
//    0x1023c23fc <+188>: strb   w9, [x8]
//    0x1023c2400 <+192>: ldr    s3, [sp, #0x8]
//    0x1023c2404 <+196>: fcvtzs w9, s3
//    0x1023c2408 <+200>: ldr    x8, [sp, #0x28]
//    0x1023c240c <+204>: strb   w9, [x8, #0x1]
//    0x1023c2410 <+208>: ldr    s3, [sp, #0x8]
//    0x1023c2414 <+212>: fcvtzs w9, s3
//    0x1023c2418 <+216>: ldr    x8, [sp, #0x28]
//    0x1023c241c <+220>: strb   w9, [x8, #0x2]
//    0x1023c2420 <+224>: ldr    x8, [sp, #0x20]
//    0x1023c2424 <+228>: ldrb   w9, [x8, #0x3]
//    0x1023c2428 <+232>: ldr    x8, [sp, #0x28]
//    0x1023c242c <+236>: strb   w9, [x8, #0x3]
//    0x1023c2430 <+240>: ldrsw  x8, [sp, #0x14]
//    0x1023c2434 <+244>: ldr    x10, [sp, #0x28]
//    0x1023c2438 <+248>: add    x8, x10, x8
//    0x1023c243c <+252>: str    x8, [sp, #0x28]
//    0x1023c2440 <+256>: ldrsw  x8, [sp, #0x14]
//    0x1023c2444 <+260>: ldr    x10, [sp, #0x20]
//    0x1023c2448 <+264>: add    x8, x10, x8
//    0x1023c244c <+268>: str    x8, [sp, #0x20]
//    0x1023c2450 <+272>: ldr    w8, [sp, #0xc]
//    0x1023c2454 <+276>: add    w8, w8, #0x1              ; =0x1
//    0x1023c2458 <+280>: str    w8, [sp, #0xc]
//    0x1023c245c <+284>: b      0x1023c2370               ; <+48> at TestC.c:13:23
//    0x1023c2460 <+288>: add    sp, sp, #0x30             ; =0x30
//    0x1023c2464 <+292>: ret
