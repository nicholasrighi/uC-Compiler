int factorial(int n)
{
	if (n <= 1)
	{
		return 1;
	}
	return n * factorial(n - 1);
}

void factorial_fill(int a[], int n)
{
	int i;
	i = 0;
	while (i < n)
	{
		a[i] = factorial(i);
		i = i + 1;
	}
	return;
}

int main(void)
{
	int x[6];
	int sum;
	int i;
	int n;

	n = 6;
	sum = 0;
	i = 0;

	factorial_fill(x, n);

	while (i < n)
	{
		sum = sum + x[i];
		i = i + 1;
	}

	return sum;
}
