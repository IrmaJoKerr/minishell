/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errormsg.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:31:04 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 04:23:41 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles file and command errors with simple, consistent messages
- Updates vars->error_code with the provided error code
- Prints formatted error message to stderr based on error type
- Designed for most file and command error scenarios in minishell

Error codes:
- ERR_PERMISSIONS (126) - Permission denied
- ERR_CMD_NOT_FOUND (127) - File or command not found
- ERR_ISDIRECTORY (2) - Cannot execute directories 
- ERR_SYNTAX (2) - Syntax error
*/
void	shell_error(char *element, int error_code, t_vars *vars)
{
	ft_putstr_fd("bleshell: ", 2);
	if (error_code == ERR_PERMISSIONS)
	{
		ft_putstr_fd(element, 2);
		ft_putendl_fd(": Permission denied", 2);
	}
	else if (error_code == ERR_CMD_NOT_FOUND)
	{
		ft_putstr_fd(element, 2);
		ft_putendl_fd(": No such file or directory", 2);
	}
	else if (error_code == ERR_ISDIRECTORY && ft_strncmp(element, "syntax error", 12) != 0)
	{
		ft_putstr_fd(element, 2);
		ft_putendl_fd(": Is a directory", 2);
	}
	else if (error_code == ERR_SYNTAX)
	{
		ft_putendl_fd(element, 2);
	}
	if (vars)
		vars->error_code = error_code;
}

/*
Outputs a not found error message for file access issues.
- Formats message with filename for clarity.
- Sends output to standard error (fd 2).
*/
void	not_found_error(char *filename, t_vars *vars)
{
	ft_putstr_fd("bleshell: ", 2);
	ft_putstr_fd(filename, 2);
	ft_putendl_fd(": No such file or directory", 2);
	if (vars)
		vars->error_code = 1;
}

/*
Handles critical errors that require immediate program exit.
- Displays a critical error message.
- Cleans up memory through partial cleanup function calls.
- Exits program with status code 1.
Works with init_shell() and other initialization functions.
Example: When environment variable duplication fails:
- crit_error(vars)
- Displays error message and exits with code 1
*/
void	crit_error(t_vars *vars)
{
	ft_putstr_fd("bleshell: critical error: initialization failed\n", 2);
	if (vars && isatty(STDIN_FILENO) && vars->ori_term_saved)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &vars->ori_term_settings);
	}
	if (!vars)
	{
		exit(1);
		return ;
	}
	if (vars->env)
		free(vars->env);
	if (vars->pipes)
		cleanup_pipes(vars->pipes);
	exit(1);
}
