/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_multiline_input.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 22:12:20 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 09:12:45 by bleow            ###   ########.fr       */
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
	status = tok_first_line(input, first_line_end, vars);
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
