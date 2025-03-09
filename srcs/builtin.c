/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:33:49 by bleow             #+#    #+#             */
/*   Updated: 2025/03/09 21:42:27 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check if the command is a shell builtin.
Returns 1 if it's a builtin command, 0 if not a builtin command.
Shell builtins: echo, cd, pwd, export, unset, env, exit
Works with execute_builtin().
*/
int	is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
    if (!ft_strcmp(cmd, "echo"))
        return (1);
    // if (!ft_strcmp(cmd, "cd"))
    //     return (1);
    if (!ft_strcmp(cmd, "pwd"))
        return (1);
    // if (!ft_strcmp(cmd, "export"))
    //     return (1);
    // if (!ft_strcmp(cmd, "unset"))
    //     return (1);
    if (!ft_strcmp(cmd, "env"))
        return (1);
    // if (!ft_strcmp(cmd, "exit"))
    //     return (1);
    return (0);
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
	// if (!ft_strcmp(cmd, "cd"))
	// 	return (builtin_cd(args, vars));
	if (!ft_strcmp(cmd, "pwd"))
		return (builtin_pwd(vars));
	// if (!ft_strcmp(cmd, "export"))
	// 	return (builtin_export(args, vars));
	// if (!ft_strcmp(cmd, "unset"))
	// 	return (builtin_unset(args, vars));
	if (!ft_strcmp(cmd, "env"))
		return (builtin_env(vars));
	// if (!ft_strcmp(cmd, "exit"))
	// 	return (builtin_exit(args, vars));
	return (1);
}
