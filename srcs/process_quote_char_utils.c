/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:56:41 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 15:38:10 by bleow            ###   ########.fr       */
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

/*
Attempts to merge quoted content with the target of a previous redirection.
- Finds the most recent redirection node
- Joins quoted content with existing redirection target
- Handles memory cleanup for both success and failure
Returns:
- 1 on successful merge
- 0 on memory allocation failure
- (-1) when merge is not applicable
Works with process_quote_char() for left-adjacent tokens.
*/
int	try_left_merge(char *curr_text, t_vars *vars)
{
	t_node	*redir_node;
	char	*existing;
	char	*merged;

	redir_node = find_last_redir(vars);
	if (redir_node && redir_node->right && redir_node->right->args[0])
	{
		existing = redir_node->right->args[0];
		merged = ft_strjoin(existing, curr_text);
		if (merged)
		{
			free(redir_node->right->args[0]);
			redir_node->right->args[0] = merged;
			free(curr_text);
			return (1);
		}
		else
		{
			free(curr_text);
			vars->error_code = ERR_DEFAULT;
			return (0);
		}
	}
	return (-1);
}
