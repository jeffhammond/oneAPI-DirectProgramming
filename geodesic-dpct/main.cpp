/* This example is a very small one designed to show how compact SYCL code
 * can be. That said, it includes no error checking and is rather terse. */
#include <CL/sycl.hpp>
#include <dpct/dpct.hpp>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

float  distance_host ( int i, float  latitude_1, float  longitude_1, float  latitude_2, float  longitude_2 )
{
  float  dist ;
  float  rad_latitude_1 ;
  float  rad_latitude_2 ;
  float  rad_longitude_1 ;
  float  rad_longitude_2 ;

  float  BAZ , C , C2A , CU1 , CU2 , CX , CY , CZ ,
         D , E , FAZ , SA , SU1 , SX  , SY , TU1 , TU2 , X , Y ; 

  const float  GDC_DEG_TO_RAD = 3.141592654 / 180.0 ;  /* Degrees to radians      */
  const float GDC_FLATTENING = 1.0 - ( 6356752.31424518 / 6378137.0 ) ; 
  const float GDC_ECCENTRICITY = ( 6356752.31424518 / 6378137.0 ) ; 
  const float  GDC_ELLIPSOIDAL =  1.0 / ( 6356752.31414 / 6378137.0 ) / ( 6356752.31414 / 6378137.0 ) - 1.0 ;
  const float GC_SEMI_MINOR = 6356752.31424518f;
  const float EPS = 0.5e-5f;


  rad_longitude_1 = longitude_1 * GDC_DEG_TO_RAD ;
  rad_latitude_1 = latitude_1 * GDC_DEG_TO_RAD ;
  rad_longitude_2 = longitude_2 * GDC_DEG_TO_RAD ;
  rad_latitude_2 = latitude_2 * GDC_DEG_TO_RAD ;

  TU1 = GDC_ECCENTRICITY * sinf(rad_latitude_1) / cosf(rad_latitude_1);
  TU2 = GDC_ECCENTRICITY * sinf(rad_latitude_2) / cosf(rad_latitude_2);

  CU1 = 1.0f / sqrtf(TU1 * TU1 + 1.0f);
  SU1 = CU1 * TU1 ;
  CU2 = 1.0f / sqrtf(TU2 * TU2 + 1.0f);
  dist = CU1 * CU2 ;
  BAZ = dist * TU2 ;
  FAZ = BAZ * TU1 ;
  X = rad_longitude_2 - rad_longitude_1 ;

  do {
    SX = sinf(X);
    CX = cosf(X);
    TU1 = CU2 * SX ;
    TU2 = BAZ - SU1 * CU2 * CX ;
    SY = sqrtf(TU1 * TU1 + TU2 * TU2);
    CY = dist * CX + FAZ ;
    Y = atan2f(SY, CY);
    SA = dist * SX / SY ;
    C2A = - SA * SA + 1.0f;
    CZ = FAZ + FAZ ;
    if ( C2A > 0.0f ) CZ = -CZ / C2A + CY ;
    E = CZ * CZ * 2.0f - 1.0f ;
    C = ( ( -3.0f * C2A + 4.0f ) * GDC_FLATTENING + 4.0f ) * C2A *
      GDC_FLATTENING / 16.0f ;
    D = X ;
    X = ( ( E * CY * C + CZ ) * SY * C + Y ) * SA ;
    X = ( 1.0f - C ) * X * GDC_FLATTENING + rad_longitude_2 - rad_longitude_1 ;
  } while (fabsf(D - X) > EPS);

  X = sqrtf(GDC_ELLIPSOIDAL * C2A + 1.0f) + 1.0f;
  X = ( X - 2.0f ) / X ;
  C = 1.0f - X ;
  C = ( X * X / 4.0f + 1.0f ) / C ;
  D = ( 0.375f * X * X - 1.0f ) * X ;
  X = E * CY ;
  dist = 1.0f - E - E ;
  dist = ( ( ( ( SY * SY * 4.0f - 3.0f ) * dist * CZ * D / 6.0f -
          X ) * D / 4.0f + CZ ) * SY * D + Y ) * C * GC_SEMI_MINOR ;
  return dist;
}

void kernel_distance(const sycl::float4 *d_A, float *d_C, const int N,
                     sycl::nd_item<3> item_ct1) {

  const int wiID = item_ct1.get_group(2) * item_ct1.get_local_range().get(2) +
                   item_ct1.get_local_id(2);
  if (wiID >= N) return;

  const float  GDC_DEG_TO_RAD = 3.141592654 / 180.0 ;  /* Degrees to radians      */
  const float GDC_FLATTENING = 1.0 - ( 6356752.31424518 / 6378137.0 ) ; 
  const float GDC_ECCENTRICITY = ( 6356752.31424518 / 6378137.0 ) ; 
  const float  GDC_ELLIPSOIDAL =  1.0 / ( 6356752.31414 / 6378137.0 ) / ( 6356752.31414 / 6378137.0 ) - 1.0 ;
  const float GC_SEMI_MINOR = 6356752.31424518f;
  const float EPS                    = 0.5e-5f;
  float  dist, BAZ , C , C2A , CU1 , CU2 , CX , CY , CZ ,
         D , E , FAZ , SA , SU1 , SX  , SY , TU1 , TU2 , X , Y ;

  const float latitude_1 = d_A[wiID].x();
  const float longitude_1 = d_A[wiID].y();
  const float latitude_2 = d_A[wiID].z();
  const float longitude_2 = d_A[wiID].w();
  const float rad_longitude_1 = longitude_1 * GDC_DEG_TO_RAD ;
  const float rad_latitude_1 = latitude_1 * GDC_DEG_TO_RAD ;
  const float rad_longitude_2 = longitude_2 * GDC_DEG_TO_RAD ;
  const float rad_latitude_2 = latitude_2 * GDC_DEG_TO_RAD ;
  TU1 = GDC_ECCENTRICITY * sycl::sin((float)rad_latitude_1) /
        sycl::cos((float)rad_latitude_1);
  TU2 = GDC_ECCENTRICITY * sycl::sin((float)rad_latitude_2) /
        sycl::cos((float)rad_latitude_2);

  CU1 = 1.0f / sycl::sqrt(TU1 * TU1 + 1.0f);
  SU1 = CU1 * TU1 ;
  CU2 = 1.0f / sycl::sqrt(TU2 * TU2 + 1.0f);
  dist = CU1 * CU2 ;
  BAZ = dist * TU2 ;
  FAZ = BAZ * TU1 ;
  X = rad_longitude_2 - rad_longitude_1 ;

  do {
    SX = sycl::sin(X);
    CX = sycl::cos(X);
    TU1 = CU2 * SX ;
    TU2 = BAZ - SU1 * CU2 * CX ;
    SY = sycl::sqrt(TU1 * TU1 + TU2 * TU2);
    CY = dist * CX + FAZ ;
    Y = sycl::atan2(SY, CY);
    SA = dist * SX / SY ;
    C2A = - SA * SA + 1.0f;
    CZ = FAZ + FAZ ;
    if ( C2A > 0.0f ) CZ = -CZ / C2A + CY ;
    E = CZ * CZ * 2.0f - 1.0f ;
    C = ( ( -3.0f * C2A + 4.0f ) * GDC_FLATTENING + 4.0f ) * C2A *
      GDC_FLATTENING / 16.0f ;
    D = X ;
    X = ( ( E * CY * C + CZ ) * SY * C + Y ) * SA ;
    X = ( 1.0f - C ) * X * GDC_FLATTENING + rad_longitude_2 - rad_longitude_1 ;
  } while (sycl::fabs(D - X) > EPS);

  X = sycl::sqrt(GDC_ELLIPSOIDAL * C2A + 1.0f) + 1.0f;
  X = ( X - 2.0f ) / X ;
  C = 1.0f - X ;
  C = ( X * X / 4.0f + 1.0f ) / C ;
  D = ( 0.375f * X * X - 1.0f ) * X ;
  X = E * CY ;
  dist = 1.0f - E - E ;
  dist = ( ( ( ( SY * SY * 4.0f - 3.0f ) * dist * CZ * D / 6.0f -
          X ) * D / 4.0f + CZ ) * SY * D + Y ) * C * GC_SEMI_MINOR ;
  d_C[wiID] = dist;
}

void distance_device(const sycl::float4 *VA, float *VC, const size_t N,
                     const int iteration) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();

  sycl::range<3> grids((N + 255) / 256, 1, 1);
  sycl::range<3> threads(256, 1, 1);

  sycl::float4 *d_VA;
  float *d_VC;
  d_VA = sycl::malloc_device<sycl::float4>(N, q_ct1);
  q_ct1.memcpy(d_VA, VA, sizeof(sycl::float4) * N).wait();
  d_VC = sycl::malloc_device<float>(N, q_ct1);

  for (int n = 0; n < iteration; n++) {
    q_ct1.submit([&](sycl::handler &cgh) {
      auto dpct_global_range = grids * threads;

      cgh.parallel_for(
          sycl::nd_range<3>(
              sycl::range<3>(dpct_global_range.get(2), dpct_global_range.get(1),
                             dpct_global_range.get(0)),
              sycl::range<3>(threads.get(2), threads.get(1), threads.get(0))),
          [=](sycl::nd_item<3> item_ct1) {
            kernel_distance(d_VA, d_VC, N, item_ct1);
          });
    });
  }

  q_ct1.memcpy(VC, d_VC, sizeof(float) * N).wait();
  sycl::free(d_VA, q_ct1);
  sycl::free(d_VC, q_ct1);
}

void verify(int size, const float *output, const float *expected_output) {
  float error_rate = 0;
  for (int i = 0; i < size; i++) {
    if (fabs(output[i] - expected_output[i]) > error_rate) {
      error_rate = fabs(output[i] - expected_output[i]);
    }
  }
  printf("The maximum error in distance for single precision is %f\n", error_rate); 
}

int main(int argc, char** argv) {

  int iteration = atoi(argv[1]);

  int num_cities = 2097152; // 2 ** 21
  int num_ref_cities = 6; // bombay, melbourne, waltham, moscow, glasgow, morocco
  int index_map[] ={436483, 1952407, 627919, 377884, 442703, 1863423};
  int N = num_cities * num_ref_cities;
  int city = 0;
  float lat, lon;

  const char* filename = "locations.txt";
  printf("Reading city locations from file %s...\n", filename);
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    perror ("Error opening the file");
    exit(-1);
  }

  sycl::float4 *input =
      (sycl::float4 *)aligned_alloc(4096, N * sizeof(sycl::float4));
  float*  output = (float*) aligned_alloc(4096, N*sizeof(float));
  float*  expected_output = (float*) malloc(N*sizeof(float));

  while (fscanf(fp, "%f %f\n", &lat, &lon) != EOF) {
    input[city].x() = lat;
    input[city].y() = lon;
    city++;
    if (city == num_cities) break;  
  }
  fclose(fp);

  // duplicate for "num_ref_cities"
  for (int c = 1;  c < num_ref_cities; c++) {
    std::copy(input, input+num_cities, input+c*num_cities);
  }
  // each reference city is compared with 'num_cities' cities
  for (int c = 0;  c < num_ref_cities; c++) {
    int index = index_map[c] - 1;
    for(int j = c*num_cities; j < (c+1)*num_cities; ++j) {
      input[j].z() = input[index].x();
      input[j].w() = input[index].y();
    }
  }

  // run on the host for verification
  for (int i = 0; i < N; i++)
  {
    float lat1 = input[i].x();
    float lon1 = input[i].y();
    float lat2 = input[i].z();
    float lon2 = input[i].w();
    expected_output[i] = distance_host(i, lat1, lon1, lat2, lon2);
  }

  distance_device(input, output, N, iteration);

  verify(N, output, expected_output);

  free(input);
  free(output);
  free(expected_output);
  return 0;
}
