#include "../includes/minishell.h"

int	builtin_pwd(t_vars *vars)
{
	char	*cwd;

	if (!vars || !vars->env)
		return (1);
	cwd = get_env_value("PWD", vars->env);
	if (!cwd || !*cwd)
	{
		free(cwd);
		return (1);
	}
	printf("%s\n", cwd);
	free(cwd);
	return (0);
}
