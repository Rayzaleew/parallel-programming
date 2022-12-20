
__kernel void findSequenceGpu(__global int* C, int N) {
  int i = get_global_id(0);
  
  int n = i + N;
  int len = 0;
  //printf("%d ", n);
  while (n > 1) {
    n = n % 2 == 0 ? n / 2 : 3 * n + 1;
    len++;
  }
  C[i] = len;
}
