#pragma omp target teams distribute parallel for thread_limit(local_work_size)
for (int thread_id = 0; thread_id < global_work_size; thread_id++)  
{
  // Find the center of the structuring element
  int el_center_i = strel_m / 2;
  int el_center_j = strel_n / 2;

  // Determine this thread's location in the matrix
  int i = thread_id % max_gicov_m;
  int j = thread_id / max_gicov_m;

  //if(j < max_gicov_n) {

  // Initialize the maximum GICOV score seen so far to zero
  float max = 0.0f;

  // Iterate across the structuring element in one dimension
  int el_i, el_j, x, y;

  for (el_i = 0; el_i < strel_m; el_i++) {
    y = i - el_center_i + el_i;
    // Make sure we have not gone off the edge of the matrix
    if ( (y >= 0) && (y < max_gicov_m) ) {
      // Iterate across the structuring element in the other dimension
      for (el_j = 0; el_j < strel_n; el_j++) {
        x = j - el_center_j + el_j;
        // Make sure we have not gone off the edge of the matrix
        //  and that the current structuring element value is not zero
        if ( (x >= 0) &&
            (x < max_gicov_n) &&
            (host_strel[(el_i * strel_n) + el_j] != 0) ) {
          // Determine if this is the maximal value seen so far
          int addr = (x * max_gicov_m) + y;
          float temp = host_gicov[addr];
          if (temp > max) max = temp;
        }
      }
    }
  }

  // Store the maximum value found
  //host_dilated[(i * max_gicov_n) + j] = max;
  host_dilated[thread_id] = max;
  //}

}
