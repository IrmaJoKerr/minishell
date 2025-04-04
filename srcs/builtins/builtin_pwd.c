/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:51:01 by lechan            #+#    #+#             */
/*   Updated: 2025/04/05 02:21:53 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Built-in command: pwd. Prints working directory.
 * Returns 0 on success, 1 on failure.
 */
int	builtin_pwd(t_vars *vars)
{
	char	*pwd;
	int		cmdcode;

	cmdcode = 0;
	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		perror("pwd");
		cmdcode = 1;
	}
	else
	{
		ft_putendl_fd(pwd, STDOUT_FILENO);
		free(pwd);
	}
	vars->error_code = cmdcode;
	return (cmdcode);
}
