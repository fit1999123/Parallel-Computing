#include <stdio.h>
#include <string.h>
#define M 100
int main()
{
	char str_a[M], str_b[M];
	int num_a[M] = {0};
	int num_b[M] = {0};
	int num_c[M];
	int len_a, len_b;
	int i, j, k, n, f=0;
	scanf("%s %s", str_a, str_b);
	len_a = strlen(str_a);
	len_b = strlen(str_b);
	if(len_a>len_b)
		k = len_a;
	else
		k = len_b;
	num_c[0] = 0;
	if(len_a > len_b)
		n = 1;
	else if(len_a == len_b)
		n = strcmp(str_a, str_b);
	else
		n = -1;
	for(i=len_a-1, j=0; i>=0; i--, j++)
	{
		num_a[j] = str_a[i] - '0';
	}
	for(i=len_b-1, j=0; i>=0; i--, j++)
	{
		num_b[j] = str_b[i] - '0';
	}
	for(i=0; i<k; i++)
	{
		if(n>=0)
		{
			if(num_a[i]-num_b[i] >= 0)
				num_c[i] = num_a[i] - num_b[i];
			else
			{
				num_c[i] = num_a[i] + 10 - num_b[i];
				num_a[i+1]--;
			}
		}
		else
		{
			if(num_b[i]-num_a[i] >= 0)
				num_c[i] = num_b[i] - num_a[i];
			else
			{
				num_c[i] = num_b[i] + 10 - num_a[i];
				num_b[i+1]--;
			}
		}
	}
	if(n<0)
		printf("-");
	for(i=k-1; i>=0; i--)
	{
		if(num_c[i])
			f=1;
		if(f||i==0)
			printf("%d", num_c[i]);
	}
	printf("\n");
	return 0;
}
