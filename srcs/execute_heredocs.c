/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_heredocs.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 13:18:09 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 15:33:18 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles parent process cleanup after heredoc child process completes.
- Terminates child process if still running
- Closes heredoc file descriptor
- Restores terminal and signal states
- Handles interrupted heredoc (SIGINT or exit code 130)
- Sets heredoc readiness flag on success
Returns:
- 1 on successful heredoc completion
- 0 if heredoc was interrupted
*/
int	process_hd_parent(pid_t child_pid, int child_status,
			int saved_signal_state, t_vars *vars)
{
	waitpid(child_pid, &child_status, 0);
	if (vars->pipes->hd_fd != -1)
	{
		close(vars->pipes->hd_fd);
		vars->pipes->hd_fd = -1;
	}
	manage_terminal_state(vars, TERM_RESTORE);
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

/*
Triggers interactive heredoc gathering for single-line commands.
- Calls get_interactive_hd.
- Closes TMP_BUF.
- Sets hd_text_ready flag.
Returns:
- 1 on success.
- 0 on failure.
*/
void	exec_hd_child(t_vars *vars)
{
	int				result;

	signal(SIGINT, SIG_DFL);
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
	{
		perror("bleshell: failed to set SIGQUIT handler for heredoc child");
		exit(EXIT_FAILURE);
	}
	result = get_interactive_hd(vars->pipes->hd_fd, vars);
	close(vars->pipes->hd_fd);
	if (result == -1)
		exit(130);
	else
		exit(0);
}
