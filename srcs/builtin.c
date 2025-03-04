/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:33:49 by bleow             #+#    #+#             */
/*   Updated: 2025/03/04 12:12:09 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check if the command is a shell builtin.
Returns 1 if it's a builtin command, 0 otherwise.
Shell builtins: echo, cd, pwd, export, unset, env, exit
Works with execute_builtin().
*/
int	is_builtin(char *cmd)
{
	return (!ft_strcmp(cmd, "echo") || !ft_strcmp(cmd, "cd")
		|| !ft_strcmp(cmd, "pwd") || !ft_strcmp(cmd, "export")
		|| !ft_strcmp(cmd, "unset") || !ft_strcmp(cmd, "env")
		|| !ft_strcmp(cmd, "exit"));
}

/*
Execute the builtin command based on the command name.
Takes command name, argument array, and vars struct.
Returns the exit status of the builtin command.
Each builtin sets it's own error codes and outputs if failure.
*/
int	execute_builtin(char *cmd, char **args, t_vars *vars)
{
	if (!ft_strcmp(cmd, "echo"))
		return (builtin_echo(args));
	if (!ft_strcmp(cmd, "cd"))
		return (builtin_cd(args, vars));
	if (!ft_strcmp(cmd, "pwd"))
		return (builtin_pwd());
	if (!ft_strcmp(cmd, "export"))
		return (builtin_export(args, vars));
	if (!ft_strcmp(cmd, "unset"))
		return (builtin_unset(args, vars));
	if (!ft_strcmp(cmd, "env"))
		return (builtin_env(vars));
	if (!ft_strcmp(cmd, "exit"))
		return (builtin_exit(args, vars));
	return (1);
}
