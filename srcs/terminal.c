/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:46:05 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 22:51:13 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets up terminal settings specifically for heredoc input.
- Modifies termios structure to disable ECHO and ICANON flags.
- Uses the original terminal settings as a base.
- Makes user input invisible when entering heredoc content.
- Disables canonical mode for raw character input.
Works with manage_terminal_state() to provide special input handling
for heredocs.
*/
void	term_heredoc(t_vars *vars)
{
	struct termios	heredoc_term;

	heredoc_term = vars->ori_term_settings;
	heredoc_term.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &heredoc_term);
}

/*
Manages terminal states throughout different operations in the shell.
- Handles saving the original terminal settings (TERM_SAVE)
- Sets up special heredoc input mode with echo/canonical disabled
  (TERM_HEREDOC)
- Restores saved terminal settings when needed (TERM_RESTORE)
- Safely checks if settings were previously saved before operations
- Ensures terminal state consistency during command execution

Example:
- manage_terminal_state(vars, TERM_SAVE); == Save original settings
- manage_terminal_state(vars, TERM_HEREDOC); == Set up heredoc mode
- manage_terminal_state(vars, TERM_RESTORE); == Restore original settings
Works with interactive prompt handling and heredoc processing.
*/
void	manage_terminal_state(t_vars *vars, int action)
{
	if (action == TERM_SAVE)
	{
		if (!vars->ori_term_saved)
		{
			tcgetattr(STDIN_FILENO, &vars->ori_term_settings);
			vars->ori_term_saved = 1;
		}
	}
	else if (action == TERM_HEREDOC)
	{
		term_heredoc(vars);
	}
	else if (action == TERM_RESTORE)
	{
		if (vars->ori_term_saved)
		{
			tcsetattr(STDIN_FILENO, TCSANOW, &vars->ori_term_settings);
			rl_on_new_line();
		}
	}
	return ;
}

/*
Restores terminal settings after heredoc processing.
- Reconnects STDIN and STDOUT to the terminal if redirected.
- Restores canonical mode and echo settings.
- Ensures readline library knows about the restored terminal state.
Works with heredoc processing to maintain proper terminal behavior.

Example: After a heredoc redirects stdin
- Restores STDIN from STDOUT's terminal
- Restores STDOUT from STDERR's terminal
- Resets terminal attributes to interactive mode
*/
void	reset_terminal_after_heredoc(void)
{
	struct termios	term;

	restore_terminal_fd(STDIN_FILENO, STDOUT_FILENO, O_RDONLY);
	restore_terminal_fd(STDOUT_FILENO, STDERR_FILENO, O_WRONLY);
	if (isatty(STDIN_FILENO))
	{
		tcgetattr(STDIN_FILENO, &term);
		term.c_lflag |= (ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
		rl_on_new_line();
	}
}

/*
Restores a file descriptor to the terminal if it's not already connected.
Can handle STDIN, STDOUT, or STDERR.
Works with reset_terminal_after_heredoc().
*/
void	restore_terminal_fd(int target_fd, int source_fd, int mode)
{
	char	*tty_path;
	int		fd;

	if (!isatty(target_fd))
	{
		tty_path = ttyname(source_fd);
		if (tty_path)
		{
			fd = open(tty_path, mode);
			if (fd >= 0)
			{
				dup2(fd, target_fd);
				close(fd);
			}
		}
	}
}

/*
Sets up terminal mode for shell operation.
- Saves original terminal settings
- Configures terminal for interactive shell use
- Disables control character echoing
*/
void	setup_terminal_mode(t_vars *vars)
{
	struct termios	term;

	if (isatty(STDIN_FILENO))
	{
		tcgetattr(STDIN_FILENO, &vars->ori_term_settings);
		vars->ori_term_saved = 1;
		tcgetattr(STDIN_FILENO, &term);
		term.c_lflag &= ~ECHOCTL;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
	}
}
