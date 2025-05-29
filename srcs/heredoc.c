/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 05:26:23 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Triggers interactive heredoc gathering for single-line commands.
- Opens TMP_BUF (O_TRUNC).
- Calls get_interactive_hd.
- Closes TMP_BUF.
- Sets hd_text_ready flag.
Returns:
- 1 on success.
- 0 on failure.
*/
// int	interactive_hd_mode(t_vars *vars)
// {
// 	int		write_fd;
// 	pid_t	pid;
// 	int		status;
// 	int		result;
// 	int		saved_signal_state;

// 	saved_signal_state = g_signal_received;
// 	write_fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
// 	if (write_fd == -1)
// 	{
// 		vars->error_code = ERR_DEFAULT;
// 		return (0);
// 	}
// 	pid = fork();
// 	if (pid == -1)
// 	{
// 		close(write_fd);
// 		vars->error_code = ERR_DEFAULT;
// 		g_signal_received = saved_signal_state;
// 		return (0);
// 	}
// 	if (pid == 0)
// 	{
// 		result = get_interactive_hd(write_fd, vars);
// 		close(write_fd);
// 		manage_terminal_state(vars, TERM_RESTORE);
// 		exit(result == -1 ? 130 : 0);
// 	}
// 	waitpid(pid, &status, 0);
// 	kill(pid, SIGTERM);
// 	close(write_fd);
// 	manage_terminal_state(vars, TERM_RESTORE);
// 	reset_terminal_after_heredoc();
// 	rl_on_new_line();
// 	g_signal_received = saved_signal_state;
// 	if (WIFSIGNALED(status) || (WIFEXITED(status)
// 			&& WEXITSTATUS(status) == 130))
// 	{
// 		unlink(TMP_BUF);
// 		vars->error_code = 130;
// 		return (0);
// 	}
// 	vars->hd_text_ready = 1;
// 	return (1);
// }

void	exec_hd_child(t_vars *vars)
{
	int	result;

	if (signal(SIGINT, hd_child_sigint_handler) == SIG_ERR)
	{
		perror("bleshell: failed to set SIGINT handler for heredoc child");
		exit(EXIT_FAILURE);
	}
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
	{
		perror("bleshell: failed to set SIGQUIT handler for heredoc child");
		exit(EXIT_FAILURE);
	}
	result = get_interactive_hd(vars->pipes->heredoc_fd, vars);
	close(vars->pipes->heredoc_fd);
	if (result == -1)
		exit(130);
	else
		exit(0);
}


int	process_hd_parent(pid_t child_pid, int child_status,
			int saved_signal_state, t_vars *vars)
{
	kill(child_pid, SIGTERM);
	if (vars->pipes->heredoc_fd != -1)
	{
		close(vars->pipes->heredoc_fd);
		vars->pipes->heredoc_fd = -1;
	}
	manage_terminal_state(vars, TERM_RESTORE);
	reset_terminal_after_heredoc();
	rl_on_new_line();
	g_signal_received = saved_signal_state;
	if (WIFSIGNALED(child_status) || (WIFEXITED(child_status)
			&& WEXITSTATUS(child_status) == 130))
	{
		unlink(TMP_BUF);
		vars->error_code = 130;
		return (0);
	}
	vars->hd_text_ready = 1;
	return (1);
}

int	interactive_hd_mode(t_vars *vars)
{
	pid_t	pid;
	int		status;
	int		saved_signal_state;

	saved_signal_state = g_signal_received;
	vars->pipes->heredoc_fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (vars->pipes->heredoc_fd == -1)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	pid = fork();
	if (pid == -1)
	{
		if (vars->pipes->heredoc_fd != -1)
		{
			close(vars->pipes->heredoc_fd);
			vars->pipes->heredoc_fd = -1;
		}
		vars->error_code = ERR_DEFAULT;
		g_signal_received = saved_signal_state;
		return (0);
	}
	if (pid == 0)
		exec_hd_child(vars);
	waitpid(pid, &status, 0);
	return (process_hd_parent(pid, status,
			saved_signal_state, vars));
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
	int		status;
	int		saved_signal_state;

	status = 0;
	saved_signal_state = g_signal_received;
	g_signal_received = -1;
	manage_terminal_state(vars, TERM_HEREDOC);
	while (1)
	{
		if (g_signal_received == -2)
		{
			manage_terminal_state(vars, TERM_RESTORE);
			g_signal_received = saved_signal_state;
			return (-1);
		}
		line = readline("heredoc> ");
		if (g_signal_received == -2)
		{
			manage_terminal_state(vars, TERM_RESTORE);
			if (line)
				free(line);
			g_signal_received = saved_signal_state;
			return (-1);
		}
		if (!line)
		{
			manage_terminal_state(vars, TERM_RESTORE);
			g_signal_received = saved_signal_state;
			break ;
		}
		if (ft_strcmp(line, vars->pipes->heredoc_delim) == 0)
		{
			free(line);
			manage_terminal_state(vars, TERM_RESTORE);
			g_signal_received = saved_signal_state;
			return (0);
		}
		if (!write_to_hd(write_fd, line, vars))
		{
			free(line);
			manage_terminal_state(vars, TERM_RESTORE);
			g_signal_received = saved_signal_state;
			return (-1);
		}
		free(line);
	}
	manage_terminal_state(vars, TERM_RESTORE);
	g_signal_received = saved_signal_state;
	return (status);
}

/*
Sets up redirection using the heredoc fd stored in vars->pipes->heredoc_fd.
Calls process_heredoc() to ensure content is ready
(either reads interactively or opens pre-stored file).
Returns:
- 1 on success.
- 0 on failure (e.g., TMP_BUF not ready, fd already open).
*/
int	handle_heredoc(t_node *node, t_vars *vars)
{
	if (!process_heredoc(node, vars))
	{
		return (0);
	}
	if (vars->pipes->heredoc_fd < 0)
	{
		return (0);
	}
	if (dup2(vars->pipes->heredoc_fd, STDIN_FILENO) == -1)
	{
		close(vars->pipes->heredoc_fd);
		vars->pipes->heredoc_fd = -1;
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	close(vars->pipes->heredoc_fd);
	vars->pipes->heredoc_fd = -1;
	return (1);
}

/*
Opens TMP_BUF for reading and stores the fd in vars->pipes->heredoc_fd.
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
	if (vars->pipes->heredoc_fd >= 0)
	{
		close(vars->pipes->heredoc_fd);
		vars->pipes->heredoc_fd = -1;
	}
	if (!vars->hd_text_ready)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	if (!read_tmp_buf(vars))
	{
		return (0);
	}
	return (1);
}

/*
Opens the temporary heredoc file (TMP_BUF) for reading.
Stores the file descriptor in vars->pipes->heredoc_fd on success.
Handles errors and attempts to unlink the TMP_BUF on failure.
Returns:
- 1 on success.
- 0 on failure.
*/
int	read_tmp_buf(t_vars *vars)
{
	int	fd;

	fd = open(TMP_BUF, O_RDONLY);
	if (fd == -1)
	{
		perror("bleshell: failed to open TMP_BUF file for reading");
		vars->error_code = ERR_DEFAULT;
		unlink(TMP_BUF);
		return (0);
	}
	vars->pipes->heredoc_fd = fd;
	return (1);
}
