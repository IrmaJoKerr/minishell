#include <stdio.h>

int ft_env(int argc, char **argv, char **env)
{
    int i;

    i = 0;
    if (env == NULL)
        return (1);
    while (env[i])
    {
        printf("%s\n", env[i]);
        i++;
    }
    return (0);
}

int main(int argc, char **argv, char **env)
{
    ft_env(argc, argv, env);
    return (0);
}