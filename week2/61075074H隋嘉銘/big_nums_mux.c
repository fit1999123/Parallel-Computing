#include <stdio.h>
#include <string.h>
int main()
{
    char a1[10001], b1[10001];
    int a[10001], b[10001], x, len, c[10001];

    gets(a1);
    gets(b1);
    int lena = strlen(a1);
    int lenb = strlen(b1);

    for(int i = 1; i <= lena; i++)
    {
        a[i] = a1[lena-i] - '0';
    }
    for(int i = 1; i <= lenb; i++)
    {
        b[i] = b1[lenb-i]-'0';
    }

	for(int i = 1; i <= lenb; i++)
    {
        for(int j = 1; j <= lena; j++)
        {
            c[i+j-1]+=a[j]*b[i];
        }
    }

    for(int i = 1; i < lena + lenb; i++)
	{
        if(c[i] > 9)
	    {
		    c[i+1]+=c[i]/10;
		    c[i]%=10;
	    }
    }

	len = lena + lenb;
    while(c[len]==0 && len>1)
    {
        len--;
    }

    for(int i = len; i >= 1; i--)
    {
        printf("%d",c[i]);
    }
}
