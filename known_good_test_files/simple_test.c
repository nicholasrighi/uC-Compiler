int y[6];
int n;

int factorial(int n) {
	if (n <= 1) {
		return 1;
	}
	return n * factorial(n - 1);
}

void fill(int x[], int n) {
	int i;
	i = 0;
	
	while (i < n) {
		x[i] = factorial(i);
		y[i] = i;
		i = i + 1;
	}
	return;
}

int main(void)
{
	int x[6];
	int sum;
	int i;

	sum = 0;
	n = 6;
	i = 0;

	fill(x, n);

	while (i < n) {
		sum = sum + x[i] + y[i];
		i = i + 1;
	}

	return sum;
}
