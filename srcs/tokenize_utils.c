/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 01:15:21 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 21:18:23 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check token position handling
- Increments position counter if not in heredoc mode
Returns:
-1 if next_flag is set (to continue main loop).
- 0 to continue normal processing.
*/
int	chk_move_pos(t_vars *vars, int hd_is_delim)
{
	if (vars->next_flag)
		return (1);
	if (!hd_is_delim)
		vars->pos++;
	return (0);
}

/*
Processes right-adjacent text for the current token.
- Extracts text between vars->start and vars->pos
- Joins this text with the last argument of the current token
- Handles memory allocation and cleanup
- Updates token content to include adjacent characters
Example: 
For "echo hello"world, joins "world" to "hello"
	to create a single argument "helloworld"
*/
// void	handle_right_adj(char *input, t_vars *vars)
// {
// 	char	*adj_str;
// 	char	*joined;
// 	int		arg_idx;
// 	t_node	*last;

// 	if (vars->pos <= vars->start)
// 		return ;
// 	adj_str = ft_substr(input, vars->start, vars->pos - vars->start);
// 	if (!adj_str)
// 		return ;
// 	if (!vars->current || !vars->current->args || !vars->current->args[0])
// 	{
// 		last = vars->head;
// 		if (last)
// 		{
// 			while (last->next)
// 				last = last->next;
// 			vars->current = last;
// 		}
// 		if (!vars->current || !vars->current->args || !vars->current->args[0])
// 		{
// 			free(adj_str);
// 			return ;
// 		}
// 	}
// 	arg_idx = 0;
// 	while (vars->current->args[arg_idx + 1])
// 		arg_idx++;
// 	joined = ft_strjoin(vars->current->args[arg_idx], adj_str);
// 	if (joined)
// 	{
// 		free(vars->current->args[arg_idx]);
// 		vars->current->args[arg_idx] = joined;
// 		if (vars->current->arg_quote_type
// 			&& vars->current->arg_quote_type[arg_idx])
// 			update_quote_types(vars, arg_idx, adj_str);
// 	}
// 	free(adj_str);
// }
// ... existing code ...

/*
Ensures vars->current points to a valid token with arguments,
traversing to the end of the token list if necessary.
Returns the validated token or NULL if no suitable token is found.
Updates vars->current if it was changed.
*/
t_node	*get_valid_target_token(t_vars *vars)
{
	t_node	*node;

	node = vars->current;
	if (!node || !node->args || !node->args[0])
	{
		node = vars->head;
		if (node)
		{
			while (node->next)
				node = node->next;
		}
		if (!node || !node->args
			|| !node->args[0])
			return (NULL);
		vars->current = node;
	}
	return (vars->current);
}

/*
Appends the given text to a specific argument of the token.
Handles memory for the argument string and updates quote types.
Returns:
- 0 on full success.
- 1 if ft_strjoin fails.
- 2 if ft_strjoin succeeds but update_quote_types fails.
*/
int	join_arg_strings(t_node *tgt_append_tok, int arg_idx, char *append_str,
		t_vars *vars)
{
	char	*new_joined_arg;
	int		quote_update_status;

	new_joined_arg = ft_strjoin(tgt_append_tok->args[arg_idx], append_str);
	if (!new_joined_arg)
		return (1);
	free(tgt_append_tok->args[arg_idx]);
	tgt_append_tok->args[arg_idx] = new_joined_arg;
	quote_update_status = 0;
	if (tgt_append_tok->arg_quote_type && \
		tgt_append_tok->arg_quote_type[arg_idx])
	{
		if (update_quote_types(vars, arg_idx, append_str) != 0)
			quote_update_status = 2;
	}
	return (quote_update_status);
}

/*
Processes right-adjacent text for the current token.
- Extracts text between vars->start and vars->pos
- Joins this text with the last argument of the current token
- Handles memory allocation and cleanup
- Updates token content to include adjacent characters
Example: 
For "echo hello"world, joins "world" to "hello"
	to create a single argument "helloworld"
*/
void	handle_right_adj(char *input, t_vars *vars)
{
	char	*adj_str;
	int		arg_idx;
	t_node	*target_token;
	size_t	num_args;

	if (vars->pos <= vars->start)
		return ;
	adj_str = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!adj_str)
		return ;
	target_token = get_valid_target_token(vars);
	if (!target_token)
	{
		free(adj_str);
		return ;
	}
	num_args = ft_arrlen(target_token->args);
	if (num_args == 0)
	{
		free(adj_str);
		return ;
	}
	arg_idx = num_args - 1;
	join_arg_strings(target_token, arg_idx, adj_str, vars);
	free(adj_str);
}

// void	handle_right_adj(char *input, t_vars *vars)
// {
// 	char	*adj_str;
// 	char	*joined;
// 	int		arg_idx;
// 	t_node	*target_token;

// 	if (vars->pos <= vars->start)
// 		return ;
// 	adj_str = ft_substr(input, vars->start, vars->pos - vars->start);
// 	if (!adj_str)
// 		return ;
// 	target_token = get_valid_target_token(vars);
// 	if (!target_token)
// 	{
// 		free(adj_str);
// 		return ;
// 	}
// 	arg_idx = 0;
// 	while (target_token->args[arg_idx + 1])
// 		arg_idx++;
// 	joined = ft_strjoin(target_token->args[arg_idx], adj_str);
// 	if (joined)
// 	{
// 		free(target_token->args[arg_idx]);
// 		target_token->args[arg_idx] = joined;
// 		if (target_token->arg_quote_type
// 			&& target_token->arg_quote_type[arg_idx])
// 			update_quote_types(vars, arg_idx, adj_str);
// 	}
// 	free(adj_str);
// }

/*
Extracts raw delimiter string from input
- Sets start position at current position
- Advances position until delimiter end is found
- Validates non-empty delimiter
- Handles memory allocation
Returns:
- Raw delimiter string on success
- NULL on error (with error_code set)
*/
char	*get_delim_str(char *input, t_vars *vars, int *error_code)
{
	int		moves;
	char	*ori_delim_str;

	vars->start = vars->pos;
	while (input[vars->pos] && !ft_isspace(input[vars->pos])
		&& !is_operator_token(get_token_at(input, vars->pos, &moves)))
	{
		vars->pos++;
	}
	if (vars->pos == vars->start)
	{
		tok_syntax_error_msg("newline", vars);
		*error_code = 1;
		return (NULL);
	}
	ori_delim_str = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!ori_delim_str)
	{
		vars->error_code = ERR_DEFAULT;
		*error_code = 1;
		return (NULL);
	}
	return (ori_delim_str);
}

/*
Process any accumulated text before a special character.
Creates a token from the text between vars->start and vars->pos.
*/
void	handle_text(char *input, t_vars *vars)
{
	char	*token_preview;

	token_preview = NULL;
	if (vars->pos > vars->start)
	{
		token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
		set_token_type(vars, token_preview);
		handle_string(input, vars);
		if (token_preview)
		{
			free(token_preview);
		}
		vars->start = vars->pos;
	}
}
