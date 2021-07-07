int f(void) {
	return 1;
}

int main(void)
{
	int a[10];
	int i;

	i = 0;

	while (i < 10) {
		a[i] = i*i;
		i = i + 1;
	}

	return a[9];
}
