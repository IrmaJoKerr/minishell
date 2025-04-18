/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:51:05 by bleow             #+#    #+#             */
/*   Updated: 2025/04/18 00:57:59 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Checks file permissions before redirection starts.
- For read mode: Verifies file exists and is readable.
- For write mode: Checks if file exists and is writable.
- For new files: Returns 1 to allow creation.
Returns:
- 1 if file can be accessed with requested mode.
- 0 if not.
*/
int	chk_permissions(char *filename, int mode, t_vars *vars)
{
	fprintf(stderr, "DEBUG: Checking permissions for file: '%s', mode: %d\n", 
			filename, mode);
	if (mode == O_RDONLY)
	{
		if (access(filename, F_OK | R_OK) == -1)
		{
			ft_putstr_fd("bleshell: ", 2);
			ft_putstr_fd(filename, 2);
			if (access(filename, F_OK) == -1)
				ft_putendl_fd(": No such file or directory", 2);
			else
				ft_putendl_fd(": Permission denied", 2);
			vars->error_code = 1;
			return (0);
		}
	}
	else if (mode & O_WRONLY)
	{
		if (access(filename, F_OK) == 0)
		{
			if (access(filename, W_OK) == -1)
			{
				ft_putstr_fd("bleshell: ", 2);
				ft_putstr_fd(filename, 2);
				ft_putendl_fd(": Permission denied", 2);
				vars->error_code = 1;
				return (0);
			}
		}
		else
		{
			char *dir_path = ft_strdup(filename);
			char *last_slash = ft_strrchr(dir_path, '/');
			if (last_slash) {
				*last_slash = '\0';
				if (*dir_path == '\0')
					strcpy(dir_path, ".");
			}
			else
			{
				strcpy(dir_path, ".");
			}
			if (access(dir_path, W_OK) == -1)
			{
				ft_putstr_fd("bleshell: ", 2);
				ft_putstr_fd(filename, 2);
				ft_putendl_fd(": Permission denied", 2);
				free(dir_path);
				vars->error_code = 1;
				return (0);
			}
			free(dir_path);
		}
	}
	return (1);
}

/*
Determines if token is a redirection operator.
- Checks if token type matches any redirection types.
- Includes all input and output redirection variants.
Returns:
- 1 if token is a redirection.
- 0 if not.
Works with process_redirections() and other redirection handlers.

Example: When processing token list
- Returns 1 for tokens of type <, >, >>, or <<
- Returns 0 for command, pipe, or other token types
*/
int	is_redirection(t_tokentype type)
{
	if (type == TYPE_HEREDOC || type == TYPE_IN_REDIRECT
		|| type == TYPE_OUT_REDIRECT || type == TYPE_APPEND_REDIRECT)
		return (1);
	else
		return (0);
}

/*
Resets saved standard file descriptors.
- Restores original stdin and stdout if they were changed.
- Closes any open heredoc file descriptor.
- Updates the pipes state in vars.
Works with execute_cmd() to clean up after command execution.
*/
void reset_redirect_fds(t_vars *vars)
{
	fprintf(stderr, "[DEBUG] reset_redirect_fds: Restoring stdin from fd=%d, stdout from fd=%d\n", 
        vars->pipes ? vars->pipes->saved_stdin : -999,
        vars->pipes ? vars->pipes->saved_stdout : -999);
	fprintf(stderr, "[DEBUG] reset_redirect_fds: Current STDIN isatty=%d, STDOUT isatty=%d\n",
        isatty(STDIN_FILENO), isatty(STDOUT_FILENO));
	if (!vars || !vars->pipes)
		return;
	if (vars->pipes->saved_stdin > 2)
    {
        fprintf(stderr, "[DEBUG] reset_redirect_fds: Restoring STDIN from fd=%d to fd=%d\n", 
            vars->pipes->saved_stdin, STDIN_FILENO);
        int result = dup2(vars->pipes->saved_stdin, STDIN_FILENO);
        fprintf(stderr, "[DEBUG] reset_redirect_fds: dup2 result for STDIN=%d\n", result);
        close(vars->pipes->saved_stdin);
        vars->pipes->saved_stdin = -1;
    }
    
    if (vars->pipes->saved_stdout > 2)
    {
        fprintf(stderr, "[DEBUG] reset_redirect_fds: Restoring STDOUT from fd=%d to fd=%d\n", 
            vars->pipes->saved_stdout, STDOUT_FILENO);
        int result = dup2(vars->pipes->saved_stdout, STDOUT_FILENO);
        fprintf(stderr, "[DEBUG] reset_redirect_fds: dup2 result for STDOUT=%d\n", result);
        if (result == -1)
        {
            fprintf(stderr, "[ERROR] reset_redirect_fds: Failed to restore stdout: %s\n", 
                strerror(errno));
        }
        close(vars->pipes->saved_stdout);
        vars->pipes->saved_stdout = -1;
    }
	// if (vars->pipes->heredoc_fd > 2)
	// {
	// 	close(vars->pipes->heredoc_fd);
	// 	vars->pipes->heredoc_fd = -1;
	// }
	if (vars->pipes->heredoc_fd > 0 && vars->heredoc_mode != 1)
	{
    	close(vars->pipes->heredoc_fd);
    	vars->pipes->heredoc_fd = -1;
	}
	if (vars->pipes->redirection_fd > 2)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	vars->pipes->out_mode = OUT_MODE_NONE;
	vars->pipes->current_redirect = NULL;
	vars->pipes->last_heredoc = NULL;
	vars->pipes->last_in_redir = NULL;
	vars->pipes->last_out_redir = NULL;
	vars->pipes->cmd_redir = NULL;
	fprintf(stderr, "[DEBUG] reset_redirect_fds: After restoration, STDIN isatty=%d, STDOUT isatty=%d\n",
        isatty(STDIN_FILENO), isatty(STDOUT_FILENO));
}

t_node *get_next_redir(t_node *current, t_node *cmd)
{
    t_node *next = current->next;
    
    while (next)
    {
        if (is_redirection(next->type) && 
            get_redir_target(next, NULL) == cmd)
        {
            return next;
        }
        next = next->next;
    }
    return NULL;
}
