/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_exp_token_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 09:02:14 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 01:11:46 by bleow            ###   ########.fr       */
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
		fprintf(stderr, "DEBUG[new_exp_token]: Creating command token since it's standalone\n");
	}
	exp_node = initnode(token_type, expanded_val);
	if (!exp_node)
		return (0);
	build_token_linklist(vars, exp_node);
	free(token);
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
		arg_idx++;
	fprintf(stderr, "DEBUG[proc_join_args]: Joining '%s' with '%s'\n",
		   vars->current->args[arg_idx], expanded_val);
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
// int	handle_tok_join(char *input, t_vars *vars, char *expanded_val, char *token)
// {
// 	int	arg_idx;

// 	DBG_PRINTF(DEBUG_EXPAND, "handle_tok_join: Joining '%s' to current token\n", 
// 		expanded_val);
// 	if (!vars->current->args || !vars->current->args[0])
// 		return (0);
// 	arg_idx = proc_join_args(vars, expanded_val);
// 	if (arg_idx == -1)
// 		return (0);
// 	if (vars->current->arg_quote_type
// 		&& vars->current->arg_quote_type[arg_idx])
// 	{
// 		DBG_PRINTF(DEBUG_EXPAND, "handle_tok_join: Updating quote types\n");
// 		if (!update_quote_types(vars, arg_idx, expanded_val))
// 			return (0);
// 	}
// 	DBG_PRINTF(DEBUG_EXPAND, "handle_tok_join: Freeing expanded_val and token\n");
// 	free(expanded_val);
// 	free(token);
// 	if (vars->adj_state[1])
// 		process_right_adj(input, vars);
// 	return (1);
// }
int	handle_tok_join(char *input, t_vars *vars, char *expanded_val, char *token)
{
    int	arg_idx;
    
    fprintf(stderr, "[MEM_DEBUG] handle_tok_join: Entry with expanded_val=%p, token=%p\n", 
            (void*)expanded_val, (void*)token);
    if (!vars->current->args || !vars->current->args[0])
    {
        fprintf(stderr, "[MEM_DEBUG] handle_tok_join: Early failure, NOT freeing memory\n");
        return (0);  // Caller must free memory
    }
    arg_idx = proc_join_args(vars, expanded_val);
    if (arg_idx == -1)
    {
        fprintf(stderr, "[MEM_DEBUG] handle_tok_join: proc_join_args failed, NOT freeing memory\n");
        return (0);  // Caller must free memory
    }
    if (vars->current->arg_quote_type && vars->current->arg_quote_type[arg_idx])
    {
        fprintf(stderr, "[MEM_DEBUG] handle_tok_join: Updating quote types\n");
        if (!update_quote_types(vars, arg_idx, expanded_val))
        {
            fprintf(stderr, "[MEM_DEBUG] handle_tok_join: update_quote_types failed, NOT freeing\n");
            return (0);  // Caller must free memory
        }
    }
    fprintf(stderr, "[MEM_DEBUG] handle_tok_join: Success, freeing expanded_val=%p, token=%p\n", 
            (void*)expanded_val, (void*)token);
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
	int	initial_pos;
	
	fprintf(stderr, "DEBUG[process_process_right_adjcy]: Right adjacency detected after left joining\n");
	vars->start = vars->pos;
	initial_pos = vars->pos;
	while (input[vars->pos] && !ft_isspace(input[vars->pos]) && 
		   !ft_is_operator(input[vars->pos]) && 
		   input[vars->pos] != '\'' && input[vars->pos] != '"' &&
		   input[vars->pos] != '$')
	{
		vars->pos++;
	}
	fprintf(stderr, "DEBUG[process_process_right_adjcy]: Consumed %d adjacent characters: '%.*s'\n",
			vars->pos - initial_pos, vars->pos - initial_pos, input + initial_pos);
	handle_right_adj(input, vars);
	vars->start = vars->pos;
	fprintf(stderr, "DEBUG[process_process_right_adjcy]: Updated start=%d\n", vars->start);
}

/*
Updates quote types for joined arguments.
Extends the quote type int array to accommodate new characters.
Returns:
- 1 on success.
- 0 on failure.
*/
int	update_quote_types(t_vars *vars, int arg_idx, char *expanded_val)
{
	int	old_len;
	int	new_len;
	int	*new_quo_type;
	int	*old_quo_type;
	
	if (!vars->current->arg_quote_type
			|| !vars->current->arg_quote_type[arg_idx])
	{
		return (0);
	}
	old_len = 0;
	old_quo_type = vars->current->arg_quote_type[arg_idx];
	while (old_quo_type[old_len] != -1)
		old_len++;
	new_len = ft_strlen(expanded_val);
	new_quo_type = malloc(sizeof(int) * (old_len + new_len + 1));
	if (!new_quo_type)
		return (0);
	addon_quo_type_arr(new_quo_type, old_quo_type, new_len);
	free(old_quo_type);
	vars->current->arg_quote_type[arg_idx] = new_quo_type;
	return (1);
}
