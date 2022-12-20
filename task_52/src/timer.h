#ifndef OPEN_MP_TIMER_H_
#define OPEN_MP_TIMER_H_

#include <time.h>
#include <omp.h>

class Timer {
 public:
  void Start() { start_ = omp_get_wtime(); }
  void End() { end_ = omp_get_wtime(); }
  long int GetNanosec() { return 1000000000 * (end_ - start_); }
  //long int GetSecond() { return end_ - start_; }

 private:
  double start_, end_;
};

#endif  // OPEN_MP_TIMER_H_