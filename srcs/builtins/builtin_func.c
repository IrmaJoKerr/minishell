#include "../includes/minishell.h"

/*
int	builtin_echo(char **args)
{
    int j;

    if (!args || !args[0])
        return (1);
	j = 0;
    while (args[0][j])
        write(1, &args[0][j++], 1);
    if (ft_strcmp(args[1], "-n") != 0)
        write(1, "\n", 1);
    return (0);
}
*/

/*
Changes made:
1. Added a newline variable to check if the -n flag is present.
2. Added a check for the -n flag and set newline to 0 if present.
   (Normally a newline is printed at the end of the echo command)
3. Added a space after each argument if there is more than one argument.
4. Added a newline at the end if the -n flag is not present.
*/
int builtin_echo(char **args)
{
    int i;
	int j;
    int newline;
    
	if (!args || !args[0])
        return (1);
	i = 1;
	newline = 1;
    if (args[i] && ft_strcmp(args[i], "-n") == 0)
	{
        newline = 0;
        i++;
    }
	while (args[i])
	{
		j = 0;
		while (args[i][j])
			write(1, &args[i][j++], 1);
		if (args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}

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
