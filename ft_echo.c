#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int ft_echo(int n, char **s)
{
    int i;
    int j;

    j = 1;
    while (s[j])
    {
        i = 0;
        while (s[j][i])
            write(1, &s[j][i++], 1);
        j++;
    }
    if (n == 0)
        write(1,"\n" ,1);
    return (0);
}

int main(int ac, char **av)
{
    //av[0] = "echo\0";
    // av[1] = "abcef";
    // av[2] = "jikfjg";
    ft_echo(0, av);
}