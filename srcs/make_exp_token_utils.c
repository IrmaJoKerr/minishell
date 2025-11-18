/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_exp_token_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 09:02:14 by bleow             #+#    #+#             */
/*   Updated: 2025/11/18 19:50:59 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Process argument joining for tokens.
Joins expanded_value with the last argument of current node
Returns: 
- The index of the joined argument.
- (-1) on failure.
*/
int	proc_join_args(t_vars *vars, char *expanded_val)
{
	/* Tripwire removed here; writer-side code below will set compact flags
	   and log DEBUG NEW when appropriate. */

	int		arg_idx;
	char	*joined;

	arg_idx = 0;
	while (vars->current->args[arg_idx + 1])
	{
		arg_idx++;
	}
	joined = ft_strjoin(vars->current->args[arg_idx], expanded_val);
	if (!joined)
		return (-1);
	ft_safefree((void **)&vars->current->args[arg_idx]);
	vars->current->args[arg_idx] = joined;
	/* Writer-side: mark the final argument as (conservatively) unquoted
	   since we're appending expanded/unquoted text. Populate the compact
	   per-arg flag so readers can prefer it during migration. */
	if (set_arg_quote_flag(vars->current, arg_idx, 0))
		fprintf(stderr, "DEBUG NEW: %s set compact flag for arg %d\n", __func__, arg_idx);
	else
		fprintf(stderr, "DEBUG OLD: %s failed to set compact flag for arg %d\n", __func__, arg_idx);
	return (arg_idx);
}

/*
Handle token joining for expanded variables with left adjacency.
Returns:
- 1 on success.
- 0 on failure.
*/
int	handle_tok_join(char *input, t_vars *vars, char *expanded_val, char *token)
{
	int	arg_idx;

	if (!vars->current || !vars->current->args || !vars->current->args[0])
		return (0);
	arg_idx = proc_join_args(vars, expanded_val);
	if (arg_idx == -1)
		return (0);
	/* Prefer the compact per-arg accessor; fall back to legacy per-char
	   arrays when necessary. If the compact flag indicates the argument was
	   quoted (non-zero), we still need to update the per-char metadata so
	   older readers continue to work during migration. */
	if (get_arg_quote_flag(vars->current, arg_idx) != 0)
	{
		if (!update_quote_types(vars, arg_idx, expanded_val))
			return (0);
	}
	ft_safefree((void **)&expanded_val);
	ft_safefree((void **)&token);
	if (vars->adj_state[1])
		process_right_adj(input, vars);
	return (1);
}

/*
Process right adjacency for token joining
Parses and joins adjacent text to the current token
*/
void	process_right_adj(char *input, t_vars *vars)
{
	vars->start = vars->pos;
	while (input[vars->pos] && !ft_isspace(input[vars->pos])
		&& !ft_is_operator(input[vars->pos])
		&& input[vars->pos] != '\'' && input[vars->pos] != '"'
		&& input[vars->pos] != '$')
	{
		vars->pos++;
	}
	handle_right_adj(input, vars);
	vars->start = vars->pos;
}

/* Old manual realloc path removed. Use quote_accessor write adapters instead. */

/*
Updates quote types for joined arguments.
Extends the quote type int array for a specific argument to add on new chars.
The new characters from 'appended_text' are marked as unquoted (type 0).
Returns:
- 0 on success.
- 1 on failure.
*/
int	update_quote_types(t_vars *vars, int arg_idx, char *appended_text)
{
	size_t	appended_len;

	if (!vars || !vars->current || !appended_text)
		return (0);
	appended_len = ft_strlen(appended_text);
	if (appended_len == 0)
		return (1);
	/* New path: use accessor API to ensure and set appended quote types. */
	fprintf(stderr, "DEBUG NEW: update_quote_types using accessor for arg %d append %zu\n", arg_idx, appended_len);
	{
		t_node *node = vars->current;
		size_t curr_len;

		  /* Safely obtain current per-char quote array length. Use the
			  accessor `has_arg_quotype()` instead of directly dereferencing
			  `node->arg_quote_type` so this reader path is robust while the
			  migration is in-progress. If per-char metadata hasn't been
			  allocated yet, treat current length as 0. */
		  curr_len = 0;
		  if (has_arg_quotype(node, arg_idx))
				curr_len = ft_intarrlen(node->arg_quote_type[arg_idx]);
		if (!ensure_arg_quotype_len(node, arg_idx, curr_len + appended_len))
			return (0);
		for (size_t i = 0; i < appended_len; ++i)
		{
			if (!set_quote_type_at(node, arg_idx, (int)(curr_len + i), 0))
				return (0);
		}
		return (1);
	}
}
