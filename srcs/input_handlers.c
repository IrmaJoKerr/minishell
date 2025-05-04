/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 02:41:39 by bleow             #+#    #+#             */
/*   Updated: 2025/05/02 03:46:30 by bleow            ###   ########.fr       */
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
	// if (DEBUG_ERROR) //DEBUG PRINT
	// 	fprintf(stderr, "[DEBUG] handle_input called with error_code=%d\n", vars->error_code); //DEBUG PRINT
	
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
	
	// if (DEBUG_ERROR) //DEBUG PRINT
	// 	fprintf(stderr, "[DEBUG] handle_input finished with error_code=%d\n", vars->error_code); //DEBUG PRINT
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
