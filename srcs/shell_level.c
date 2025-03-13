/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_level.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 14:19:12 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 02:53:25 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Gets the shell level from environment variables.
 * If SHLVL doesn't exist, creates it with value "1".
 * Stores the current SHLVL value in vars->shell_level.
 * Returns 1 on success, 0 on failure.
 */
int get_shell_level(t_vars *vars)
{
	int     i;
	char    *shlvl_str;
	char    *new_shlvl_str;
	 
	if (!vars || !vars->env)
		return (0);
	 
	i = 0;
	while (vars->env[i])
	{
		if (ft_strncmp(vars->env[i], "SHLVL=", 6) == 0)
		{
			shlvl_str = vars->env[i] + 6;
			vars->shell_level = ft_atoi(shlvl_str);
			return (1);
		}
		i++;
	}
	new_shlvl_str = ft_strdup("SHLVL=1");
	if (!new_shlvl_str)
		return (0);
	vars->env[i] = new_shlvl_str;
	vars->env[i+1] = NULL;
	vars->shell_level = 1;
	return (1);
}
 
/*
Increments the shell level environment variable (SHLVL) by 1.
Updates the value in the environment and in vars->shell_level.
Returns 1 on success, 0 on failure.
*/
int incr_shell_level(t_vars *vars)
{
	int     i;
	char    *new_shlvl;
	char	*new_env_entry;
 
	if (!vars || !vars->env)
		return (0);
	vars->shell_level++;
	i = 0;
	while (vars->env[i])
	{
		if (ft_strncmp(vars->env[i], "SHLVL=", 6) == 0)
		{
			new_shlvl = ft_itoa(vars->shell_level);
			if (!new_shlvl)
				return (0);
			new_env_entry = ft_strjoin("SHLVL=", new_shlvl);
			ft_safefree((void **)&new_shlvl);
			if (!new_env_entry)
				return (0);
			ft_safefree((void **)&vars->env[i]);
			vars->env[i] = new_env_entry;
			return (1);
		}
		i++;
	}
	return (0);
}
 