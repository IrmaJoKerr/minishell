/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:51:06 by lechan            #+#    #+#             */
/*   Updated: 2025/06/01 07:07:12 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Built-in command: unset. Removes variables from environment.
 * Handles valid identifiers according to POSIX rules.
 * Returns 0 on success, 1 on failure.
 */
int	builtin_unset(char **args, t_vars *vars)
{
	int		i;
	char	*is_env_val;

	is_env_val = NULL;
	if ((!vars || !vars->env) && (vars != NULL))
		return (vars->error_code = 1);
	if (!args || !args[1])
		return (vars->error_code = 0);
	i = 1;
	while (args[i])
	{
		is_env_val = get_env_val(args[i], vars->env);
		if (is_env_val)
			modify_env(&vars->env, -1, args[i]);
		free(is_env_val);
		i++;
	}
	return (vars->error_code = 0);
}

/*
Reallocate environment array until the variable to modify.
Changes: -1 for removal, +1 for addition.
*/
char	**realloc_until_var(int changes, char **env, char *key, int count)
{
	char	**new_env;
	int		pos;
	int		keylen;

	keylen = ft_strlen(key);
	pos = get_env_pos(key, env, keylen);
	new_env = (char **)malloc((count + changes + 1) * sizeof(char *));
	if (!new_env)
		return (NULL);
	// printf("pos : %d\n", pos);
	copy_env_front(env, new_env, pos);
	if (changes == 1)
	{
		new_env[pos] = ft_strdup(key);
		copy_env_add(env, new_env, (pos));
	}
	if (changes == 0)
	{
		free(new_env[pos]);
		new_env[pos + 1] = ft_strdup(key);
		copy_env_change(env, new_env, (pos + 1));
	}
	if (changes == -1)
		copy_env_unset(env, new_env, (pos + 1));
	// else
	// 	new_env[pos] = ft_strdup(key);
	ft_free_2d(env, ft_arrlen(env));
	// return (null_env(new_env, count + changes));
	return(new_env);
}

char	**null_env(char **new_env, int count)
{
	if (!new_env)
		return (NULL);
	new_env[count] = NULL;
	return (new_env);
}

/*
Find the position of an environment variable in the environment array.
Returns the position if found, or the end of array (count of elements)
if not found.
Returns -1 on NULL input for key or env.
*/
int	get_env_pos(char *key, char **env, int keylen)
{
	int	envpos;
	// int	strpos;

	if (!key || !env)
		return (-1);
	envpos = 0;
	// while (env[envpos])
	// {
	// 	if (ft_strlen(env[envpos]) < (size_t)keylen + 1)
	// 	{
	// 		envpos++ ;
	// 		continue ;
	// 	}
	// 	strpos = 0;
	// 	while (strpos < keylen && env[envpos][strpos] == key[strpos])
	// 		strpos++;
	// 	if (strpos == keylen && env[envpos][keylen] == '=')
	// 		return (envpos);
	// 	envpos++;
	// }
	// return (envpos);
	while (env[envpos])
	{
		if (ft_strncmp(env[envpos], key, keylen) == 0)
		{
			if (env[envpos][keylen] == '=' || env[envpos][keylen] == '\0')
				return (envpos);
		}
		envpos++;
	}
	return (envpos);
}

/*
Modify the environment by adding or removing a variable.
Changes: -1 for removal, +1 for addition.
*/
// void	modify_env(char ***env, int changes, char *key)
// {
// 	char	**new_env;
// 	int		count;
// 	int		pos;
// 	// void	*ptr;

// 	pos = get_env_pos(key, *env, ft_strlen(key));
// 	count = 0;
// 	while ((*env)[count])
// 		count++;
// 	if ((changes == -1 && pos == count) || (changes
// 			== 1 && pos < count && (*env)[pos][ft_strlen(key)] == '='))
// 		return ;
// 	new_env = realloc_until_var(changes, *env, key, count);
// 	// ptr = (void *)*env;
// 	// free(ptr);
// 	*env = new_env;
// }
void	modify_env(char ***env, int changes, char *key)
{
	char	**new_env;
	int		count;
	int		pos;
	char	*equal_pos;
	int		keylen;

	equal_pos = ft_strchr(key, '=');
	if (equal_pos)
		keylen = equal_pos - key;
	else
		keylen = ft_strlen(key);
	pos = get_env_pos(key, *env, keylen);
	count = 0;
	while ((*env)[count])
		count++;
	if (changes == -1 && pos == count)
		return ;
	if (changes == 1 && pos < count)
	{
		if (equal_pos && ft_strcmp((*env)[pos], key) == 0)
			changes = 0;
	}
	new_env = realloc_until_var(changes, *env, key, count);
	if (new_env)
		*env = new_env;
}
