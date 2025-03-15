/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:51:06 by lechan            #+#    #+#             */
/*   Updated: 2025/03/14 01:49:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Builtin unset command - removes variables from environment.
Find the position of an environment variable in the environment array.
Returns the position if found, or the end of array if not found.
*/
int	get_env_pos(char *var, char **env)
{
	int	i;
	int	j;
	int	len;

	i = 0;
	len = ft_strlen(var);
	while (env[i])
	{
		j = 0;
		while (env[i][j] == var[j] && j < len)
			j++;
		if (j == len && env[i][j] == '=')
			return (i);
		i++;
	}
	return (i);
}

/*
Reallocate environment array until the variable to modify.
Changes: -1 for removal, +1 for addition.
*/
char	**realloc_until_var(int changes, char **env, char *var, int count)
{
	char	**new_env;
	int		i;
	int		pos;

	i = 0;
	pos = get_env_pos(var, env);
	new_env = (char **)malloc((count + changes + 1) * sizeof(char *));
	while (i < pos)
	{
		new_env[i] = env[i];
		i++;
	}
	if (changes == -1)
		free(env[i++]);
	else if (changes == 1)
		new_env[i++] = ft_strdup(var);
	while (i - changes < count)
	{
		new_env[i] = env[i - changes];
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

/*
Modify the environment by adding or removing a variable.
Changes: -1 for removal, +1 for addition.
*/
void	modify_env(char ***env, int changes, char *var)
{
	char	**new_env;
	int		count;
	int		pos;

	pos = get_env_pos(var, *env);
	count = 0;
	while ((*env)[count])
		count++;
	if ((changes == -1 && pos == count) || (changes == 1 && pos
			< count && (*env)[pos][ft_strlen(var)] == '='))
		return ;
	new_env = realloc_until_var(changes, *env, var, count);
	free(*env);
	*env = new_env;
}

/*
Builtin unset command - removes variables from environment.
*/
int	builtin_unset(char **args, t_vars *vars)
{
	int	i;

	if (!args || !args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		if (get_env_value(args[i], vars->env))
			modify_env(&vars->env, -1, args[i]);
		i++;
	}
	return (0);
}
