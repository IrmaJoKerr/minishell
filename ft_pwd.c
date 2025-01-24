#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void    ft_pwd(void)
{
    char cwd[1024];

    printf("%s\n",getcwd(cwd, sizeof(cwd)));
}

int main (){
    ft_pwd();
    return (0);
}