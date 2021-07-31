int y;
int main(void)
{
  int x[3];
  y = 2;
  if (y == 2)
  {
    x[y] = 31;
  }
  else
  {
    x[y] = 17;
  }
  return x[y];
}