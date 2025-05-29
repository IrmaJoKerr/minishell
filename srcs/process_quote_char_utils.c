/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:56:41 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 18:12:17 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 Handles failure when merging quoted tokens
- Reports error message
- Frees content memory
- Resets adjacency state
Returns:
- 0 to indicate error
*/
int	token_cleanup_error(char *content, t_vars *vars)
{
	free(content);
	process_adj(NULL, vars);
	return (0);
}

/*
Cleans up after token processing and handles adjacency states.
- Frees content memory.
- Processes right adjacency if needed.
- Resets adjacency state.
*/
void	cleanup_and_process_adj(char *content, char *input, t_vars *vars)
{
	free(content);
	if (vars->adj_state[1])
		process_right_adj(input, vars);
	process_adj(NULL, vars);
}

/*
Finds the most relevant redirection node in the token list.
Checks in the following order of priority:
 - Current node if it's a redirection
 - Recent previous nodes (up to 3 back)
 - Last redirection in the entire token list
Returns:
 - Pointer to the located redirection node
 - NULL if no redirection node exists
Works with process_quote_char() for redirection target handling.
*/
t_node	*find_last_redir(t_vars *vars)
{
	t_node	*current;
	t_node	*last_redir;
	int		i;

	last_redir = NULL;
	if (vars->current && is_redirection(vars->current->type))
		return (vars->current);
	current = vars->current;
	i = 0;
	while (current && current->prev && i < 3)
	{
		current = current->prev;
		i++;
		if (is_redirection(current->type))
			return (current);
	}
	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
			last_redir = current;
		current = current->next;
	}
	return (last_redir);
}

/*
Validates that a single redirection token has a valid target.
- Checks that the redirection has a next token
- Ensures the next token isn't another operator
- Reports appropriate syntax errors
Returns:
- 1 if the redirection has a valid target
- 0 otherwise (with error message shown)
*/
int	validate_single_redir(t_node *redir_node, t_vars *vars)
{
	t_node	*next;

	if (!redir_node->args || !redir_node->args[0])
	{
		tok_syntax_error_msg("newline", vars);
		return (0);
	}
	next = redir_node->next;
	if (next && next->type == TYPE_PIPE)
	{
		if (next->args && next->args[0])
			tok_syntax_error_msg(next->args[0], vars);
		else
			tok_syntax_error_msg("operator", vars);
		return (0);
	}
	return (1);
}
