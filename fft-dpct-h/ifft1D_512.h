#define DPCT_USM_LEVEL_NONE
#include <CL/sycl.hpp>
#include <dpct/dpct.hpp>
void ifft1D_512(T2 *work, sycl::nd_item<3> item_ct1, T *smem)
{

  int tid = item_ct1.get_local_id(2);
  int gid = item_ct1.get_group(2) * 512 + tid;
  int hi = tid>>3;
  int lo = tid&7;
  T2 data[8];
  //__local T smem[8*8*9];
  const int reversed[] = {0,4,2,6,1,5,3,7};

  // starting index of data to/from global memory
  for( int i = 0; i < 8; i++ ) data[i] = work[gid+i*64];

  IFFT8( data );

  //itwiddle8( data, tid, 512 );
  for( int j = 1; j < 8; j++ )
      data[j] = cmplx_mul(data[j] , exp_i((2*M_PI*reversed[j]/512)*(tid)) );

  //transpose(data, &smem[hi*8+lo], 66, &smem[lo*66+hi], 8, 0xf);
    for (int i = 0; i < 8; i++)
        smem[hi * 8 + lo + i * 66] = data[reversed[i]].x();
  item_ct1.barrier();
    for (int i = 0; i < 8; i++) data[i].x() = smem[lo * 66 + hi + i * 8];
  item_ct1.barrier();
    for (int i = 0; i < 8; i++)
        smem[hi * 8 + lo + i * 66] = data[reversed[i]].y();
  item_ct1.barrier();
    for (int i = 0; i < 8; i++) data[i].y() = smem[lo * 66 + hi + i * 8];
  item_ct1.barrier();

  IFFT8( data );

  //itwiddle8( data, hi, 64 );
  for( int j = 1; j < 8; j++ )
      data[j] = cmplx_mul(data[j] , exp_i((2*M_PI*reversed[j]/64)*hi) );


  //transpose(data, &smem[hi*8+lo], 8*9, &smem[hi*8*9+lo], 8, 0xE);
    for (int i = 0; i < 8; i++)
        smem[hi * 8 + lo + i * 72] = data[reversed[i]].x();
  item_ct1.barrier();
    for (int i = 0; i < 8; i++) data[i].x() = smem[hi * 72 + lo + i * 8];
  item_ct1.barrier();
    for (int i = 0; i < 8; i++)
        smem[hi * 8 + lo + i * 72] = data[reversed[i]].y();
  item_ct1.barrier();
    for (int i = 0; i < 8; i++) data[i].y() = smem[hi * 72 + lo + i * 8];

  IFFT8( data );

  for( int i = 0; i < 8; i++ ) {
    data[i].x() = data[i].x() / (T)512;
    data[i].y() = data[i].y() / (T)512;
  }

  //globalStores8(data, work, 64);
  for( int i = 0; i < 8; i++ )
    work[gid+i*64] = data[reversed[i]];
}