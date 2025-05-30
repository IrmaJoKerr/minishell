/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:25:54 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 13:25:18 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Joins a chunk to an existing string, freeing the original string.
- Takes base string and chunk to append.
- Creates new joined string and frees original.
Returns:
Newly allocated string with combined content.
Original input string if chunk is NULL.
New copy of chunk if base string is NULL.
Works with hd_expander().

Example: str = "Hello", chunk = " World"
- Returns: "Hello World"
- Original "Hello" string is freed
*/
char	*hd_merge_and_free(char *str, char *chunk)
{
	char	*new_str;

	if (!chunk)
		return (str);
	if (!str)
		return (ft_strdup(chunk));
	new_str = ft_strjoin(str, chunk);
	free(str);
	return (new_str);
}

/*
Cleans up resources after a heredoc error.
- Closes the file descriptor.
- Removes the temporary file.
Works with process_heredoc_path to centralize error handling.
*/
void	cleanup_heredoc_fd(int write_fd)
{
	close(write_fd);
	unlink(TMP_BUF);
}

/*
Process heredoc delimiter when expected
- Skips whitespace before delimiter
- Extracts and validates delimiter string
- Creates token for the delimiter
- Updates state after processing
Returns:
- 0 on error
- 1 on success with next_flag set (continue)
- 2 on waiting for more input (whitespace skipping)
*/
int	proc_hd_delim(char *input, t_vars *vars, int *hd_is_delim)
{
	char	*ori_delim_str;
	int		error_code;

	error_code = 0;
	if (ft_isspace(input[vars->pos]))
	{
		vars->pos++;
		vars->start = vars->pos;
		return (2);
	}
	ori_delim_str = get_delim_str(input, vars, &error_code);
	if (error_code || !ori_delim_str)
		return (0);
	if (!is_valid_delim(ori_delim_str, vars))
	{
		free(ori_delim_str);
		return (0);
	}
	maketoken(ori_delim_str, TYPE_ARGS, vars);
	free(ori_delim_str);
	*hd_is_delim = 0;
	vars->start = vars->pos;
	vars->next_flag = 1;
	return (1);
}

/*
Checks if a node is the target of a heredoc redirection.
- Identifies both direct targets (node follows heredoc)
- Identifies delimiter nodes matching heredoc_delim

Returns:
- 1 if node is a heredoc target (don't append to command)
- 0 if not a heredoc target (can append to command)
*/
int	is_heredoc_target(t_node *node, t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_HEREDOC)
		{
			if (current->next == node)
				return (1);
			if (vars->pipes && vars->pipes->heredoc_delim)
			{
				if (node->args && node->args[0]
					&& ft_strcmp(node->args[0]
						, vars->pipes->heredoc_delim) == 0)
					return (1);
			}
		}
		current = current->next;
	}
	return (0);
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
