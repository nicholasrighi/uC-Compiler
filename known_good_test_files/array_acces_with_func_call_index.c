int index(void) {
  return 3;
}

int main(void) {
  int x[4];
  x[index()] = 1;
  return x[index()];
}