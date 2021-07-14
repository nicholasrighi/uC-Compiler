int factorial(int n) {
	if (n <= 1) {
		return 1;
	}
	return n * factorial(n-1);
}

int main(void)
{
	int x[5];
	int sum;
	int i;

	i = 0;
	sum = 0;

	while (i < 5) {
		x[i] = factorial(i);
		sum = sum + x[i];
		i = i + 1;
	}

	if (sum > 37) {
		return 100;
	}

	return sum;
}
