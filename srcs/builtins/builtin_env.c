/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:43 by lechan            #+#    #+#             */
/*   Updated: 2025/03/22 02:28:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: env. Prints the environment variables.
- Prints all environment variables to STDOUT.
Returns 0 on success. Returns 1 on failure.
*/
int	builtin_env(t_vars *vars)
{
	int	i;
	int	cmdcode;

	i = 0;
	cmdcode = 0;
	if (!vars || !vars->env)
	{
		cmdcode = 1;
		return (cmdcode);
	}
	while (vars->env[i])
	{
		printf("%s\n", vars->env[i]);
		i++;
	}
	vars->error_code = cmdcode;
	return (cmdcode);
}
