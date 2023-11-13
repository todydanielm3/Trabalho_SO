#include <time.h>
#include <stdlib.h>

int main () {
  clock_t begin;
  double timeSpent = 0.0;

  begin = clock();
  do {
    timeSpent = (double)(clock() - begin) / CLOCKS_PER_SEC;
  } while (timeSpent < 30.0);

  exit(0);
}