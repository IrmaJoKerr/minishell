/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:51:06 by lechan            #+#    #+#             */
/*   Updated: 2025/06/02 13:24:20 by bleow            ###   ########.fr       */
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
	t_envop	*to_proc;
	char	**new_env;
	int		old_len;

	to_proc = NULL;
	if ((!vars || !vars->env) && (vars != NULL))
		return (vars->error_code = 1);
	if (!args || !args[1])
		return (vars->error_code = 0);
	to_proc = parse_envop_list(args, -1);
	if (!to_proc)
		return (vars->error_code = 1);
	match_envline_to_env(to_proc, vars->env);
	new_env = proc_envop_list(to_proc, vars->env);
	if (new_env)
	{
		old_len = ft_arrlen(vars->env);
		ft_free_2d(vars->env, old_len);
		vars->env = new_env;
	}
	free_envop_list(to_proc);
	return (vars->error_code = 0);
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

	if (!key || !env)
		return (-1);
	envpos = 0;
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
