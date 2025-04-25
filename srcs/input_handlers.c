/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 02:41:39 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 14:35:54 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Allocates memory and reads content from an open file descriptor.
- Allocates buffer of specified size plus null terminator
- Reads file content into the buffer
- Adds null terminator for string usage
Returns:
- Newly allocated string containing the file contents
- NULL if allocation failed or reading failed
*/
char	*allocate_and_read(int fd, size_t size)
{
	char	*buffer;
	ssize_t	bytes_read;

	buffer = malloc(size + 1);
	if (!buffer)
		return (NULL);
	bytes_read = read(fd, buffer, size);
	if (bytes_read == -1)
	{
		free(buffer);
		return (NULL);
	}
	buffer[bytes_read] = '\0';
	return (buffer);
}

/*
Function reads an entire file into memory.
Returns:
- A newly allocated string containing the file contents,
- NULL if an error occurred.
*/
char	*read_entire_file(const char *filename)
{
	int			fd;
	char		*content;
	struct stat	file_stats;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return (NULL);
	if (fstat(fd, &file_stats) == -1)
	{
		close(fd);
		return (NULL);
	}
	content = allocate_and_read(fd, file_stats.st_size);
	close(fd);
	return (content);
}

/*
Processes user input and directs to appropriate command handler.
- Detects and handles multiline input with heredocs
- Processes single-line commands directly
- Cleans up heredoc resources after processing
- Resets redirection file descriptors
Main entry point for all command input processing in the shell.
*/
void	handle_input(char *input, t_vars *vars)
{
	if (ft_strchr(input, '\n'))
	{
		process_multiline_input(input, vars);
	}
	else
	{
		reset_shell(vars);
		process_command(input, vars);
	}
	if (vars->pipes && vars->pipes->heredoc_delim)
	{
		free(vars->pipes->heredoc_delim);
		vars->pipes->heredoc_delim = NULL;
	}
	reset_redirect_fds(vars);
}

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
Checks for non-whitespace characters after a certain position on a line.
Returns:
- 1 if only whitespace/end-of-string found.
- 0 if found something that's not whitespace and not the end. It's an error.
*/
int	check_trailing_chars(const char *line, int start_pos)
{
	int	i;

	i = start_pos;
	while (line[i] && ft_isspace(line[i]))
	{
		i++;
	}
	if (line[i] != '\0')
	{
		return (0);
	}
	return (1);
}
