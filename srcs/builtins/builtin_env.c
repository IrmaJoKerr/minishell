#include "../includes/minishell.h"

int	builtin_env(t_vars *vars)
{
	int i;

	if (!vars || !vars->env)
		return (1);
	i = 0;
	while (vars->env[i])
	{
		printf("%s\n", vars->env[i]);
		i++;
	}
	return (0);
}
