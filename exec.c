int main () {
  clock_t begin;
  double timeSpent = 0.0;

  begin = clock();
  while(true) {
    timeSpent = (double)(clock() - begin) / CLOCKS_PER_SEC;
    if (timeSpent >= 30.0) {
      break;
    }
  }

  exit(0);
}