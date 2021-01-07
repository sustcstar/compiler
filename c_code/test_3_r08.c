#include<stdio.h>
int mod(int x,int n)
{
    return x -(x / n) * n;
}

int DigitSum(int y)
{
     if(y == 0)
        return 0;
     return mod(y, 10) + DigitSum(y / 10);
}

int main()
{
    int num;
    scanf("%d", &num);
    if(num < 0)
        printf("-1\n");
    else 
        printf("%d\n", DigitSum(num));
    return 0;
}