/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tmp_buf_reader.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 21:25:54 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 17:53:57 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Initializes a read buffer structure.
- Reads content from TMP_BUF
- Sets up processing buffer
- Initializes tracking variables
Returns:
- 1 on success
- 0 on failure (memory allocation or read error)
*/
int	init_read_buf(t_read_buf *rb)
{
	if (!rb)
		return (0);
	rb->file_content = NULL;
	rb->buffer = NULL;
	rb->pos = 0;
	rb->in_quotes = 0;
	rb->quote_type = 0;
	rb->file_content = read_tmp_buf_file(TMP_BUF);
	if (!rb->file_content)
		return (0);
	rb->buffer = malloc(ft_strlen(rb->file_content) + 1);
	if (!rb->buffer)
	{
		free(rb->file_content);
		rb->file_content = NULL;
		return (0);
	}
	return (1);
}

/*
Cleans up resources in a read buffer structure.
- Frees allocated memory
- Resets state variables
- Optionally removes the temporary file
*/
void	cleanup_rd_buf(t_read_buf *rb, int remove_tmp)
{
	if (!rb)
		return ;
	if (rb->buffer)
	{
		free(rb->buffer);
		rb->buffer = NULL;
	}
	if (rb->file_content)
	{
		free(rb->file_content);
		rb->file_content = NULL;
	}
	rb->pos = 0;
	rb->in_quotes = 0;
	rb->quote_type = 0;
	if (remove_tmp)
		unlink(TMP_BUF);
}

/*
Updates quote tracking state based on current character.
- Handles both single and double quotes
- Tracks when entering or exiting quoted regions
*/
void	update_quote_state(char c, t_read_buf *rb)
{
	if ((c == '"' || c == '\'') && (!rb->in_quotes || rb->quote_type == c))
	{
		if (rb->in_quotes && rb->quote_type == c)
		{
			rb->in_quotes = 0;
			rb->quote_type = 0;
		}
		else if (!rb->in_quotes)
		{
			rb->in_quotes = 1;
			rb->quote_type = c;
		}
	}
}

/*
Processes a complete command line from the buffer.
- Null-terminates the buffer
- Processes non-empty commands
- Resets buffer position
*/
void	process_buffer_command(t_read_buf *rb, t_vars *vars)
{
	rb->buffer[rb->pos] = '\0';
	if (rb->pos > 0)
		process_command(rb->buffer, vars);
	rb->pos = 0;
}

/*
Reads from temporary buffer file and processes each line as a command.
- Preserves quotes across multiple lines
- Converts newlines in quoted regions to spaces
- Processes each complete command separately
- Cleans up resources when finished
*/
void	tmp_buf_reader(t_vars *vars)
{
	t_read_buf	rb;
	int			i;

	if (!init_read_buf(&rb))
		return ;
	i = 0;
	while (rb.file_content[i])
	{
		update_quote_state(rb.file_content[i], &rb);
		if (rb.file_content[i] == '\n')
		{
			if (rb.in_quotes)
				rb.buffer[rb.pos++] = ' ';
			else
				process_buffer_command(&rb, vars);
		}
		else
		{
			rb.buffer[rb.pos++] = rb.file_content[i];
		}
		i++;
	}
	if (rb.pos > 0)
		process_buffer_command(&rb, vars);
	cleanup_rd_buf(&rb, 1);
}
