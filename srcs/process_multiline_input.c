/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_multiline_input.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 22:12:20 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 21:47:15 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes multiline input strings.
- If no newline is found, treats as single-line input.
- If the first line contains a heredoc (<<), processes heredoc content
  into TMP_BUF and sets hd_text_ready=1 before executing the command.
- Otherwise, writes the full input to TMP_BUF and calls tmp_buf_reader().
Returns:
- 1 on successful processing.
- 0 on failure (e.g., syntax error, file I/O error).
*/
int	process_multiline_input(char *input, t_vars *vars)
{
	char	*first_line_end;
	char	*content_start;

	first_line_end = ft_strchr(input, '\n');
	if (!first_line_end)
	{
		process_command(input, vars);
		return (1);
	}
	content_start = first_line_end + 1;
	if (!proc_first_line(input, first_line_end, vars))
		return (0);
	if (vars->pipes->heredoc_delim != NULL)
		return (process_heredoc_path(input, first_line_end,
				content_start, vars));
	else
	{
		return (process_standard(input, vars));
	}
}

/*
Processes the first line of input for syntax analysis.
- Temporarily resets shell state.
- Tokenizes the first line to identify command structure.
- Validates syntax and checks for errors.
Returns:
- 1 if tokenization and syntax are valid.
- 0 if errors are detected (sets vars->error_code).
Works with process_multiline_input as preprocessing step.
*/
int	proc_first_line(char *input, char *first_line_end, t_vars *vars)
{
	int	status;

	reset_shell(vars);
	status = tokenize_first_line(input, first_line_end, vars);
	if (!status || vars->error_code == ERR_SYNTAX)
		return (0);
	return (1);
}

/*
Processes the heredoc path in multiline input.
- Validates heredoc syntax.
- Sets up temporary buffer for heredoc content.
- Processes content from buffer and interactive input.
- Executes command with heredoc input.
Returns:
- Result from heredoc processing (1 on success, 0 on error).
Works with process_multiline_input for heredoc branch.
*/
int	process_heredoc_path(char *input, char *first_line_end,
			char *content_start, t_vars *vars)
{
	int	write_fd;
	int	found_in_buf;
	int	status;
	int	first_line_len;

	first_line_len = first_line_end - input;
	status = chk_hd_first_line(input, first_line_len, vars);
	if (!status)
		return (0);
	write_fd = open_hd_tmp_buf(vars);
	if (write_fd == -1)
		return (0);
	found_in_buf = proc_hd_buffer(write_fd, content_start, vars);
	if (found_in_buf == -1)
	{
		cleanup_heredoc_fd(write_fd);
		return (0);
	}
	status = handle_interactive_hd(write_fd, found_in_buf, vars);
	if (status == -1)
	{
		cleanup_heredoc_fd(write_fd);
		return (0);
	}
	return (hd_proc_end(write_fd, input, first_line_end, vars));
}

/*
Tokenizes the first line of input.
- Temporarily null-terminates the input at nl_ptr.
- Tokenizes and restores the newline.
Returns:
- 1 on successful tokenization (syntax check separate).
- 0 on failure.
*/
int	tokenize_first_line(char *input, char *nl_ptr, t_vars *vars)
{
	int		result;
	char	original_char;

	original_char = *nl_ptr;
	*nl_ptr = '\0';
	result = tokenizer(input, vars);
	*nl_ptr = original_char;
	return (result);
}

/*
Finds the delimiter token (TYPE_ARGS) following a TYPE_HEREDOC token.
Returns: 
- Pointer to the delimiter token node.
- NULL if not found/valid.
*/
t_node	*find_delim_token(t_node *head)
{
	t_node	*current;
	t_node	*delimiter_node;

	current = head;
	while (current)
	{
		if (current->type == TYPE_HEREDOC)
		{
			delimiter_node = current->next;
			while (delimiter_node && delimiter_node->type != TYPE_ARGS)
			{
				delimiter_node = delimiter_node->next;
			}
			if (delimiter_node && delimiter_node->type == TYPE_ARGS)
				return (delimiter_node);
			else
				return (NULL);
		}
		current = current->next;
	}
	return (NULL);
}

/*
Finds the raw delimiter string within the first line, after the '<<'.
Returns:
- Pointer to the start of the raw delimiter in the line.
- NULL if error.
*/
char	*find_raw_delim(char *line_start, int len, const char *delim)
{
	char	*hd_operator_ptr;
	char	*raw_ptr;
	int		offset;
	int		rem_len;

	hd_operator_ptr = ft_strnstr(line_start, "<<", len);
	if (!hd_operator_ptr)
		return (NULL);
	offset = (hd_operator_ptr - line_start) + 2;
	rem_len = len - offset;
	if (rem_len <= 0)
		return (NULL);
	raw_ptr = ft_strnstr(line_start + offset, delim, rem_len);
	return (raw_ptr);
}

/*
Checks for non-whitespace characters immediately after the raw delimiter.
Returns:
- 1 if only whitespace (or end of string) follows.
- 0 otherwise.
*/
int	chk_hd_tail(char *line_start, char *raw_delim_ptr, char *delim,
				t_vars *vars)
{
	int		pos_after;
	char	current_char;

	pos_after = (raw_delim_ptr - line_start) + ft_strlen(delim);
	current_char = line_start[pos_after];
	while (current_char != '\0' && current_char != '\n')
	{
		if (!ft_isspace(current_char))
		{
			tok_syntax_error_msg("newline", NULL);
			return (0);
		}
		pos_after++;
		current_char = line_start[pos_after];
		vars->error_code = ERR_SYNTAX;
	}
	return (1);
}

/*
Finds the raw delimiter string in the line and handles the "not found" error.
Returns
- Pointer to the raw delimiter if found.
- NULL otherwise (error handled).
*/
char	*chk_raw_delim(char *line_start, int len, char *delim_arg, t_vars *vars)
{
	char	*raw_delim_ptr;
	char	original_char;

	raw_delim_ptr = find_raw_delim(line_start, len, delim_arg);
	if (!raw_delim_ptr)
	{
		original_char = line_start[len];
		line_start[len] = '\n';
		line_start[len] = original_char;
		vars->error_code = ERR_DEFAULT;
		return (NULL);
	}
	return (raw_delim_ptr);
}

/**
Validates the heredoc delimiter presence and trailing characters
on the first line.
Returns:
- 1 if valid.
- 0 if otherwise (sets vars->error_code on failure).
*/
int	chk_hd_first_line(char *line_start, int len, t_vars *vars)
{
	t_node	*delim_node;
	char	*raw_delim_ptr;
	char	*delim_arg;
	char	original_char;
	int		is_ok;

	delim_node = find_delim_token(vars->head);
	if (!delim_node || !delim_node->args || !delim_node->args[0])
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	delim_arg = delim_node->args[0];
	raw_delim_ptr = chk_raw_delim(line_start, len, delim_arg, vars);
	if (!raw_delim_ptr)
		return (0);
	original_char = line_start[len];
	line_start[len] = '\0';
	is_ok = chk_hd_tail(line_start, raw_delim_ptr, delim_arg, vars);
	line_start[len] = original_char;
	if (!is_ok)
		return (0);
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
Processes a heredoc line: 
- Checks if it matches delimiter. If match, it will return 1 and exit.
- Else it writes the line to file.
Returns:
- 1 if delimiter found.
- 0 if processed normally.
- -1 on error.
*/
int	proc_hd_line(int write_fd, char *line, t_vars *vars)
{
	size_t	delim_len;
	int		result;

	delim_len = ft_strlen(vars->pipes->heredoc_delim);
	if (ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0
		&& line[delim_len] == '\0')
	{
		free(line);
		return (1);
	}
	result = write_to_hd(write_fd, line, vars);
	free(line);
	if (!result)
		return (-1);
	return (0);
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
		status = get_interactive_hd(write_fd, vars);
	return (status);
}

/*
Executes the command stored in the (temporarily null-terminated) first line.
*/
void	exec_first_line(char *input, char *nl_ptr, t_vars *vars)
{
	char	original_char;

	original_char = *nl_ptr;
	*nl_ptr = '\0';
	process_command(input, vars);
	*nl_ptr = original_char;
}

/*
Finalizes heredoc processing: closes FD, sets ready flag, executes command.
Returns:
- 1 (Successful heredoc processing).
*/
int	hd_proc_end(int write_fd, char *input, char *nl_ptr, t_vars *vars)
{
	close(write_fd);
	vars->hd_text_ready = 1;
	exec_first_line(input, nl_ptr, vars);
	return (1);
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
