int f(void) {
	return 2;
}

int main(void)
{
	int x[10];
	int i;

	i = 0;

	while (i < 10) {
		x[i] = i * f();
		i = i + 1;
	}

	return x[9] * 10;
}
