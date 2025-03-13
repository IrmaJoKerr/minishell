/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:43 by lechan            #+#    #+#             */
/*   Updated: 2025/03/13 02:18:50 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: env. Prints the environment variables.
*/
int	builtin_env(t_vars *vars)
{
	int	i;

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
