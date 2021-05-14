int array_test(int a[], int b[]) {
  return a[1] + b[2];
}

int main(void) {
  int x[3];
  int y[4];
  x[1] = 5;
  y[2] = 6;
  return array_test(x, y);
}