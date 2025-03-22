/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:51:06 by lechan            #+#    #+#             */
/*   Updated: 2025/03/22 17:13:51 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Builtin unset command - removes variables from environment.
Returns 0 on success, 1 on failure.
*/
int	builtin_unset(char **args, t_vars *vars)
{
	int	i;
	int	cmdcode;

	cmdcode = 0;
	if ((!vars || !vars->env) && (vars != NULL))
	{
		cmdcode = 1;
		vars->error_code = cmdcode;
		return (cmdcode);
	}
	if (!args || !args[1])
	{
		vars->error_code = cmdcode;
		return (cmdcode);
	}
	i = 1;
	while (args[i])
	{
		if (get_env_val(args[i], vars->env))
			modify_env(&vars->env, -1, args[i]);
		i++;
	}
	vars->error_code = cmdcode;
	return (cmdcode);
}

/*
Process the environment variable at modification position
Returns the next index to process
*/
int	set_next_pos(int changes, char **env, int pos)
{
	void	*ptr;

	if (changes == -1)
	{
		ptr = (void *)env[pos];
		ft_safefree(&ptr);
		return (pos + 1);
	}
	else if (changes == 1)
	{
		return (pos + 1);
	}
	return (pos);
}

/*
Reallocate environment array until the variable to modify.
Changes: -1 for removal, +1 for addition.
*/
char	**realloc_until_var(int changes, char **env, char *var, int count)
{
	char	**new_env;
	int		pos;
	int		next_idx;

	pos = get_env_pos(var, env);
	new_env = (char **)malloc((count + changes + 1) * sizeof(char *));
	if (!new_env)
		return (NULL);
	copy_env_front(env, new_env, pos);
	if (changes == 1)
		new_env[pos] = ft_strdup(var);
	next_idx = set_next_pos(changes, env, pos);
	copy_env_back(env, new_env, next_idx, changes);
	return (new_env);
}

/*
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
Modify the environment by adding or removing a variable.
Changes: -1 for removal, +1 for addition.
*/
void	modify_env(char ***env, int changes, char *var)
{
	char	**new_env;
	int		count;
	int		pos;
	void	*ptr;

	pos = get_env_pos(var, *env);
	count = 0;
	while ((*env)[count])
		count++;
	if ((changes == -1 && pos == count) || (changes
			== 1 && pos < count && (*env)[pos][ft_strlen(var)] == '='))
		return ;
	new_env = realloc_until_var(changes, *env, var, count);
	ptr = (void *)*env;
	ft_safefree(&ptr);
	*env = new_env;
}
