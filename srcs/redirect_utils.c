/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 16:14:35 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 12:22:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
	if (type == TYPE_HEREDOC || type == TYPE_IN_REDIR
		|| type == TYPE_OUT_REDIR || type == TYPE_APPD_REDIR)
		return (1);
	else
		return (0);
}

/*
Restores a standard file descriptor (like stdin/stdout) from a saved fd.
- Checks if the saved_fd is valid (> 2).
- Duplicates saved_fd onto target_fd (e.g., STDIN_FILENO).
- Closes the saved_fd.
- Logs debug/error messages.
- Resets the saved_fd value to -1 via the pointer.
Works with reset_redirect_fds() to restore original file descriptors.
*/
void	restore_fd(int *saved_fd_ptr, int target_fd)
{
	int	result;

	if (*saved_fd_ptr > 2)
	{
		result = dup2(*saved_fd_ptr, target_fd);
		if (result == -1)
		{
			perror("dup2");
		}
		close(*saved_fd_ptr);
		*saved_fd_ptr = -1;
	}
}

/*
Resets specific redirection tracking variables within the t_pipe structure.
Works with reset_redirect_fds() to clean up redirection state after
command execution or FD restoration.
*/
void	reset_pipe_redir_state(t_pipe *pipes)
{
	if (!pipes)
		return ;
	pipes->out_mode = OUT_MODE_NONE;
	pipes->current_redirect = NULL;
	pipes->last_in_redir = NULL;
	pipes->last_out_redir = NULL;
	pipes->cmd_redir = NULL;
}

/*
Resets saved standard file descriptors and redirection state.
- Restores original stdin and stdout if they were changed.
- Closes any open heredoc or general redirection file descriptors.
- Resets internal redirection tracking variables.
Works with execute_cmd() to clean up after command execution.
*/
void	reset_redirect_fds(t_vars *vars)
{
	if (!vars || !vars->pipes)
		return ;
	restore_fd(&vars->pipes->saved_stdin, STDIN_FILENO);
	restore_fd(&vars->pipes->saved_stdout, STDOUT_FILENO);
	if (vars->pipes->hd_fd >= 0)
	{
		close(vars->pipes->hd_fd);
		vars->pipes->hd_fd = -1;
	}
	if (vars->pipes->redir_fd > 2)
	{
		close(vars->pipes->redir_fd);
		vars->pipes->redir_fd = -1;
	}
	reset_pipe_redir_state(vars->pipes);
}
