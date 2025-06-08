/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/06/07 02:49:52 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <string.h>

/*
Creates child process to gather heredoc input interactively.
- Opens temporary file for storing heredoc content
- Forks child process to handle input collection
- Waits for child process to complete
- Processes child status via process_hd_parent
- Preserves signal state across heredoc operation
Returns:
- 1 on successful heredoc completion
- 0 on errors (file open, fork failure, interruption)
*/
int	interactive_hd_mode(t_vars *vars, int sss, int status)
{
	pid_t	pid;

	vars->pipes->hd_fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_APPEND, 0600);
	if (vars->pipes->hd_fd == -1)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	pid = fork();
	if (pid == -1)
	{
		if (vars->pipes->hd_fd != -1)
		{
			close(vars->pipes->hd_fd);
			vars->pipes->hd_fd = -1;
		}
		vars->error_code = ERR_DEFAULT;
		g_signal_received = sss;
		return (0);
	}
	if (pid == 0)
		exec_hd_child(vars);
	return (process_hd_parent(pid, status, sss, vars));
}

/*
Performs cleanup for heredoc termination scenarios.
- Restores terminal state to pre-heredoc settings
- Restores original signal handling state
- Frees line buffer if provided
- Returns the specified exit code unchanged
Used by get_interactive_hd() for consistent cleanup.
*/
int	hd_term(int saved_signal_state, t_vars *vars, int code, char **line)
{
	manage_terminal_state(vars, TERM_RESTORE);
	g_signal_received = saved_signal_state;
	if (line && *line)
	{
		ft_safefree((void **)&*line);
		*line = NULL;
	}
	return (code);
}

/*
Reads lines interactively until the delimiter is found or EOF is reached.
- Writes lines to the given open file descriptor `write_fd`.
- Uses vars->pipes->heredoc_delim as the delimiter.
- Performs expansion based on vars->pipes->hd_expand via write_to_hd.
Returns:
- 0 on success (delimiter found or EOF reached after warning).
- -1 on failure (write error or malloc error).
*/
int	get_interactive_hd(int write_fd, t_vars *vars)
{
	char	*line;
	int		saved_signal_state;

	saved_signal_state = g_signal_received;
	g_signal_received = -1;
	signal(SIGQUIT, SIG_IGN);
	manage_terminal_state(vars, TERM_HEREDOC);
	while (1)
	{
		if (g_signal_received == -2)
			return (hd_term(saved_signal_state, vars, -1, &line));
		line = readline("> ");
		if (!line)
		{
			g_signal_received = saved_signal_state;
			break ;
		}
		if (ft_strcmp(line, vars->pipes->heredoc_delim) == 0)
			return (hd_term(saved_signal_state, vars, 0, &line));
		if (!write_to_hd(write_fd, line, vars))
			return (hd_term(saved_signal_state, vars, -1, &line));
		ft_safefree((void **)&line);
	}
	return (hd_term(saved_signal_state, vars, 0, &line));
}

/*
Sets up redirection using the heredoc fd stored in vars->pipes->hd_fd.
Calls process_heredoc() to ensure content is ready
(either reads interactively or opens pre-stored file).
Returns:
- 1 on success.
- 0 on failure (e.g., TMP_BUF not ready, fd already open).
*/
int	handle_heredoc(t_node *node, t_vars *vars)
{
	if (!process_heredoc(node, vars))
		return (0);
	if (vars->pipes->hd_fd < 0)
		return (0);
	if (dup2(vars->pipes->hd_fd, STDIN_FILENO) == -1)
	{
		close(vars->pipes->hd_fd);
		vars->pipes->hd_fd = -1;
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	close(vars->pipes->hd_fd);
	vars->pipes->hd_fd = -1;
	return (1);
}

/*
Opens TMP_BUF for reading and stores the fd in vars->pipes->hd_fd.
Assumes heredoc content is already fully gathered in TMP_BUF.
Returns:
- 1 on success.
- 0 on failure (e.g., TMP_BUF not ready, fd already open).
- Sets vars->error_code to ERR_DEFAULT on failure.
*/
int	process_heredoc(t_node *node, t_vars *vars)
{
	if (!node || !vars || !vars->pipes)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	if (vars->pipes->hd_fd >= 0)
	{
		close(vars->pipes->hd_fd);
		vars->pipes->hd_fd = -1;
	}
	if (!vars->hd_text_ready)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	if (!read_tmp_buf(vars))
		return (0);
	return (1);
}
