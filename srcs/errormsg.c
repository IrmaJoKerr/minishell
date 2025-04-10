/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errormsg.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:31:04 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 19:41:08 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles file and command errors with simple, consistent messages
- Updates vars->error_code with the provided error code
- Prints formatted error message to stderr based on error type
- Designed for all file and command error scenarios in minishell

Error codes:
- ERR_PERMISSIONS (126) - Permission denied
- ERR_CMD_NOT_FOUND (127) - File or command not found
- ERR_ISDIRECTORY (2) - Cannot execute directories 
- ERR_SYNTAX (2) - Syntax error
*/
void shell_error(char *element, int error_code, t_vars *vars)
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
    else if (error_code == ERR_ISDIRECTORY)
    {
        ft_putstr_fd(element, 2);
        ft_putendl_fd(": Is a directory", 2);
    }
    else if (error_code == ERR_SYNTAX)
    {
        ft_putstr_fd("syntax error near unexpected token '", 2);
        ft_putstr_fd(element, 2);
        ft_putendl_fd("'", 2);
    }
    if (vars)
        vars->error_code = error_code;
}

// /*
// Outputs a permission error message for redirection failures.
// - Formats message with filename for clarity.
// - Sends output to standard error (fd 2).
// Works with use_errno_error() and redirect_error().
// */
// void	file_access_error(char *filename, int type)
// {
// 	if (type == ERR_DEFAULT)
// 		ft_putstr_fd("bleshell: redirect: ", 2);
// 	else
// 		ft_putstr_fd("bleshell: cd: ", 2);
// 	ft_putstr_fd("bleshell: permission denied: ", 2);
// 	ft_putendl_fd(filename, 2);
// }

/*
Outputs a not found error message for file access issues.
- Formats message with filename for clarity.
- Sends output to standard error (fd 2).
*/
void	not_found_error(char *filename)
{
	ft_putstr_fd("No such file or directory: ", 2);
	ft_putendl_fd(filename, 2);
}

/*
Outputs a syntax error message for pipe syntax issues.
- Indicates unexpected token '|' in the command.
- Sends output to standard error (fd 2).
*/
void pipe_syntax_error(t_vars *vars)
{
	ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
	vars->error_code = 2;
}

// /*
// Outputs a system error message using errno value.
// - Gets the error string from errno.
// - Formats with filename and sends to standard error.
// - Uses error code for custom error reporting.
// Works with redirect_error().
// */
// void	use_errno_error(char *filename, int *error_code)
// {
// 	*error_code = errno;
// 	if (*error_code == 0)
// 		*error_code = 1;
// 	ft_putstr_fd("bleshell: redirect: ", 2);
// 	ft_putstr_fd(filename, 2);
// 	ft_putstr_fd(": ", 2);
// 	ft_putendl_fd(strerror(*error_code), 2);
// }

/*
Handles error messages for redirection operations.
- Displays formatted error for file redirection issues.
- Updates the shell's last command code.
- Supports both permission errors and system errors.
Returns:
Error code (1 for permission denied, or errno value).
Works with execute_redirection().

Example: For "cat > /root/file":
- redirect_error("/root/file", vars, 1)
- Displays "bleshell: redirect: /root/file: Permission denied"
- Returns 13 (EACCES)
*/
// int	redirect_error(char *filename, t_vars *vars, int use_errno)
// {
// 	int	error_code;

// 	error_code = 1;
// 	// Debug print for redirect errors
// 	fprintf(stderr, "DEBUG: Redirect error for file: '%s', errno: %d (%s)\n", 
// 		filename, errno, strerror(errno));
// 	if (use_errno)
// 		use_errno_error(filename, &error_code);
// 	else
// 		file_access_error(filename);
// 	vars->error_code = error_code;
// 	return (error_code);
// }

/*
Outputs a standardized error message and updates command code.
- Takes error message text and prints to stderr.
- Uses a consistent shell prefix format.
- Updates the pipeline's last_cmdcode for exit status.
Returns:
Error code for use in return statements.
Works with various command functions.

Example: For "cd /nonexistent":
- print_error("No such file or directory", vars, 1)
- Returns 1 for consistent error reporting
*/
// int	print_error(const char *msg, t_vars *vars, int error_code)
// {
// 	ft_putstr_fd("bleshell: ", 2);
// 	if (msg)
// 		ft_putendl_fd((char *)msg, 2);
// 	if (vars)
// 		vars->error_code = error_code;
// 	return (error_code);
// }

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
