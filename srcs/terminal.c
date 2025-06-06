/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:46:05 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 15:25:38 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets up terminal settings for interactive heredoc input.
- Modifies termios structure based on original settings.
- Disables canonical mode (ICANON) for character-by-character input.
- Enables echoing of typed characters (ECHO).
- Disables echoing of control characters (ECHOCTL) (e.g., ^C won't show as ^C).
- Configures read to return after 1 character with no timeout (VMIN=1, VTIME=0).
This mode allows the shell to process heredoc input interactively while
displaying typed characters.
Works with manage_terminal_state() to provide special input handling
for heredocs.
*/
void	term_heredoc(t_vars *vars)
{
	struct termios	heredoc_term;

	heredoc_term = vars->ori_term_settings;
	heredoc_term.c_lflag &= ~(ICANON);
	heredoc_term.c_lflag |= ECHO;
	heredoc_term.c_cc[VMIN] = 1;
	heredoc_term.c_cc[VTIME] = 0;
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
		term_heredoc(vars);
	else if (action == TERM_RESTORE)
	{
		restore_terminal_fd(STDIN_FILENO, STDOUT_FILENO, O_RDONLY);
		restore_terminal_fd(STDOUT_FILENO, STDERR_FILENO, O_WRONLY);
		if (vars->ori_term_saved)
		{
			tcsetattr(STDIN_FILENO, TCSANOW, &vars->ori_term_settings);
		}
	}
	return ;
}

/*
Restores a file descriptor to the terminal if it's not already connected.
Can handle STDIN, STDOUT, or STDERR.
Works with manage_terminal_state().
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
