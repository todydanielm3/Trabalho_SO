#include <time.h>
#include <stdlib.h>

int main () {
  clock_t begin;
  double timeSpent = 0.0;


  begin = clock();
  while(1) {
    timeSpent = (double)(clock() - begin) / CLOCKS_PER_SEC;
    if (timeSpent >= 30.0) {
      break;
    }
  }

  exit(0);
}