/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_exp_token_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 09:02:14 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 21:19:32 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates a new token based on the expanded value
Returns: 1 on success, 0 on failure
*/
int	new_exp_token(t_vars *vars, char *expanded_val, char *token)
{
	t_node		*exp_node;
	t_tokentype	token_type;

	token_type = TYPE_ARGS;
	if (!vars->head)
	{
		token_type = TYPE_CMD;
	}
	exp_node = initnode(token_type, expanded_val);
	if (!exp_node)
		return (0);
	build_token_linklist(vars, exp_node);
	free(token);
	free(expanded_val);
	return (1);
}

/*
Process argument joining for tokens.
Joins expanded_value with the last argument of current node
Returns: 
- The index of the joined argument.
- (-1) on failure.
*/
int	proc_join_args(t_vars *vars, char *expanded_val)
{
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
	free(vars->current->args[arg_idx]);
	vars->current->args[arg_idx] = joined;
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
	if (vars->current->arg_quote_type
		&& vars->current->arg_quote_type[arg_idx])
	{
		if (!update_quote_types(vars, arg_idx, expanded_val))
			return (0);
	}
	free(expanded_val);
	free(token);
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

/*
Reallocates and extends a single quote type array (int *).
Modifies *quo_arr_ptr directly.
Returns:
- 0 on success.
- 1 on error.
*/
int	realloc_quo_arr(int **quo_arr_ptr, size_t new_char_len)
{
	int		*old_int_arr;
	int		*new_int_arr;
	size_t	old_len;
	size_t	total_len;
	size_t	i;

	old_int_arr = *quo_arr_ptr;
	old_len = ft_intarrlen(old_int_arr);
	total_len = old_len + new_char_len;
	new_int_arr = malloc(sizeof(int) * (total_len + 1));
	if (!new_int_arr)
		return (0);
	if (old_len > 0)
		ft_memcpy(new_int_arr, old_int_arr, sizeof(int) * old_len);
	i = 0;
	while (i < new_char_len)
	{
		new_int_arr[old_len + i] = 0;
		i++;
	}
	new_int_arr[total_len] = -1;
	free(old_int_arr);
	*quo_arr_ptr = new_int_arr;
	return (1);
}

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

	if (!vars || !vars->current || !vars->current->arg_quote_type
		|| !vars->current->arg_quote_type[arg_idx] || !appended_text)
		return (0);
	appended_len = ft_strlen(appended_text);
	if (appended_len == 0)
		return (1);
	return (realloc_quo_arr(&(vars->current->arg_quote_type[arg_idx]),
			appended_len));
}
