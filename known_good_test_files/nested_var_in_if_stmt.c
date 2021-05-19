int main(void) {
  int x;
  x = 3;
  {
    x = 4;
  }
  if (x) {
    x = 4;
  }
  return x;
}