#include "../includes/minishell.h"

int	builtin_env(t_vars *vars)
{
    int i;

    if (!vars)
    {
        printf("Debug: vars is NULL\n");
        return (1);
    }
    if (!vars->env)
    {
        printf("Debug: vars->env is NULL\n");
        return (1);
    }
    printf("Debug: env array at %p\n", (void*)vars->env);
    i = 0;
    while (vars->env[i])
    {
        printf("Debug: env[%d] = %s\n", i, vars->env[i]);
        i++;
    }
    return (0);
}
