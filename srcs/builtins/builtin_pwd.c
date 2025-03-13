/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:51:01 by lechan            #+#    #+#             */
/*   Updated: 2025/03/13 02:20:00 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: pwd. Prints the current working directory.
*/
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
