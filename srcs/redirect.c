/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:51:05 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 16:23:43 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Finds the next redirection node associated with a specific command.
- Iterates through the token list starting from the node after 'current'.
- Checks if a node is a redirection type using is_redirection().
- Checks if the redirection targets the specified 'cmd' node using
  get_redir_target().
Returns:
- Pointer to the next t_node that is a redirection for 'cmd'.
- NULL if no further redirections for 'cmd' are found after 'current'.
Works with proce_redir_chain() redirection processing logic to handle
multiple redirections applied to the same command.
(e.g., cmd < in1 > out1 < in2).
*/
t_node	*get_next_redir(t_node *current, t_node *cmd)
{
	t_node	*next;

	next = current->next;
	while (next)
	{
		if (is_redirection(next->type)
			&& get_redir_target(next, NULL) == cmd)
		{
			return (next);
		}
		next = next->next;
	}
	return (NULL);
}

/*
Handles creation of a redirection token and its filename.
Creates the redirection node and extracts the filename.
*/
int	handle_redirection_token(char *input, int *i, t_vars *vars,
	t_tokentype type)
{
	char	*redir_str;
	t_node	*redir_node;
	int		moves;

	if (type == TYPE_HEREDOC || type == TYPE_APPD_REDIR)
		moves = 2;
	else
		moves = 1;
	redir_str = ft_substr(input, *i, moves);
	if (!redir_str)
		return (0);
	redir_node = initnode(type, redir_str);
	free(redir_str);
	if (!redir_node)
		return (0);
	*i += moves;
	if (!proc_redir_filename(input, i, redir_node))
	{
		free_token_node(redir_node);
		return (0);
	}
	build_token_linklist(vars, redir_node);
	vars->start = *i;
	return (1);
}

/*
Extracts a filename string from input based on quoting rules.
- Handles both quoted filenames ('file.txt' or "file.txt")
- Handles unquoted filenames (stops at whitespace or operators)
- Advances position pointer past the extracted filename
- Properly handles quote matching and extraction
Returns:
- Extracted filename string on success
- NULL on errors (missing closing quote, empty filename)
*/
char	*parse_and_get_filename(char *input, int *i_ptr, int tgt_start,
			char *quo_char)
{
	char	*file_str;

	if (input[*i_ptr] == '\'' || input[*i_ptr] == '"')
	{
		quo_char = &input[*i_ptr];
		(*i_ptr)++;
		tgt_start = *i_ptr;
		while (input[*i_ptr] && input[*i_ptr] != *quo_char)
			(*i_ptr)++;
		if (!input[*i_ptr])
			return (NULL);
		file_str = ft_substr(input, tgt_start, *i_ptr - tgt_start);
		(*i_ptr)++;
	}
	else
	{
		while (input[*i_ptr] && !ft_isspace(input[*i_ptr])
			&& !is_operator_token(get_token_at(input, *i_ptr, &(int){0})))
			(*i_ptr)++;
		if (*i_ptr == tgt_start)
			return (NULL);
		file_str = ft_substr(input, tgt_start, *i_ptr - tgt_start);
	}
	return (file_str);
}

/*
Processes and sets a redirection target filename.
- Skips leading whitespace to find filename start
- Calls parse_and_get_filename to extract the actual string
- Handles memory management of existing filenames
- Stores extracted filename in redirection node's args array
Returns:
- 1 on successful filename extraction and storage
- 0 on any error (empty input, extraction failure)
*/
int	proc_redir_filename(char *input, int *i, t_node *redir_node)
{
	char	*filename_str;
	int		tgt_start;
	char	quo_char;

	quo_char = 0;
	while (input[*i] && ft_isspace(input[*i]))
		(*i)++;
	if (!input[*i])
		return (0);
	tgt_start = *i;
	filename_str = parse_and_get_filename(input, i, tgt_start, &quo_char);
	if (!filename_str)
		return (0);
	if (redir_node->args[0])
		free(redir_node->args[0]);
	redir_node->args[0] = filename_str;
	return (1);
}
