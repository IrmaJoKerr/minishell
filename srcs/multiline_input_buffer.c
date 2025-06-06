/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiline_input_buffer.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:40:15 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 15:30:20 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Extracts the next line from a content buffer.
Returns:
- The extracted line.
- NULL on error.
*/
char	*extract_next_line(char **current_pos_ptr, t_vars *vars)
{
	char	*next_newline;
	char	*line;
	char	*current_pos;

	current_pos = *current_pos_ptr;
	next_newline = ft_strchr(current_pos, '\n');
	if (next_newline)
	{
		line = ft_substr(current_pos, 0, next_newline - current_pos);
		*current_pos_ptr = next_newline + 1;
	}
	else
	{
		line = ft_strdup(current_pos);
		*current_pos_ptr = NULL;
	}
	if (!line)
	{
		vars->error_code = ERR_DEFAULT;
		return (NULL);
	}
	return (line);
}

/*
Processes heredoc content from the input buffer.
Reads lines from 'content',checks against delimiter, writes to fd.
Returns:
- 1 if delimiter found.
- 0 if end of buffer without delimiter.
- -1 on error.
*/
int	proc_hd_buffer(int write_fd, char *content, t_vars *vars)
{
	char	*current_pos;
	char	*line;
	int		found;
	int		result;

	current_pos = content;
	found = 0;
	while (current_pos && *current_pos && !found)
	{
		line = extract_next_line(&current_pos, vars);
		if (!line)
			return (-1);
		result = proc_hd_line(write_fd, line, vars);
		if (result == 1)
			found = 1;
		else if (result == -1)
			return (-1);
	}
	return (found);
}

/*
Handles the case where the first line is not a heredoc command.
Writes the entire input to TMP_BUF and calls tmp_buf_reader().
Returns:
- 1 on success.
- 0 on failure (e.g., file I/O error).
 */
int	process_standard(char *input, t_vars *vars)
{
	int		fd;
	ssize_t	write_len;
	size_t	input_len;

	fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	input_len = ft_strlen(input);
	write_len = write(fd, input, input_len);
	if (write_len == -1 || (size_t)write_len != input_len)
	{
		close(fd);
		unlink(TMP_BUF);
		return (0);
	}
	close(fd);
	tmp_buf_reader(vars);
	return (1);
}

/*
Opens the temporary file for writing heredoc content.
Returns:
- File descriptor on success.
- -1 on failure (prints error).
 */
int	open_hd_tmp_buf(t_vars *vars)
{
	int	write_fd;

	write_fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (write_fd == -1)
	{
		vars->error_code = ERR_DEFAULT;
		return (-1);
	}
	return (write_fd);
}

/*
Calls the interactive heredoc reader if delimiter wasn't in buffer.
Returns: 
- 0 on success (or if no interactive needed).
- -1 on interactive error.
*/
int	handle_interactive_hd(int write_fd, int found_in_buf, t_vars *vars)
{
	int	status;

	status = 0;
	if (!found_in_buf)
	{
		close(write_fd);
		status = interactive_hd_mode(vars, g_signal_received, 0);
	}
	return (status);
}
