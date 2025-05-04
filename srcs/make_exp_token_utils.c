/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_exp_token_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 09:02:14 by bleow             #+#    #+#             */
/*   Updated: 2025/05/05 01:39:48 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates a new token based on the expanded value
Returns: 1 on success, 0 on failure
*/
// int	new_exp_token(t_vars *vars, char *expanded_val, char *token)
// {
// 	t_node		*exp_node;
// 	t_tokentype	token_type;

// 	token_type = TYPE_ARGS;
// 	if (!vars->head)
// 	{
// 		token_type = TYPE_CMD;
// 	}
// 	exp_node = initnode(token_type, expanded_val);
// 	if (!exp_node)
// 		return (0);
// 	build_token_linklist(vars, exp_node);
// 	free(token);
// 	free(expanded_val);
// 	return (1);
// }
int	new_exp_token(t_vars *vars, char *expanded_val, char *token)
{
    t_node		*exp_node;
    t_tokentype	token_type;

    token_type = TYPE_ARGS;
    if (!vars->head) // Check if list is empty to determine CMD/ARGS
    {
        token_type = TYPE_CMD;
    }
    else if (vars->current && vars->current->type == TYPE_PIPE) // Check if previous was PIPE
    {
        token_type = TYPE_CMD;
    }
    exp_node = initnode(token_type, expanded_val); // initnode takes ownership of expanded_val
    if (!exp_node)
    {
        free(expanded_val); // Free if initnode fails
        free(token);
        return (0);
    }
    build_token_linklist(vars, exp_node);
    free(token);
    // expanded_val is now owned by exp_node
    return (1);
}

/*
Process argument joining for tokens.
Joins expanded_value with the last argument of current node
Returns: 
- The index of the joined argument.
- (-1) on failure.
*/
// int	proc_join_args(t_vars *vars, char *expanded_val)
// {
// 	int		arg_idx;
// 	char	*joined;

// 	arg_idx = 0;
// 	while (vars->current->args[arg_idx + 1])
// 	{
// 		arg_idx++;
// 	}
// 	joined = ft_strjoin(vars->current->args[arg_idx], expanded_val);
// 	if (!joined)
// 		return (-1);
// 	free(vars->current->args[arg_idx]);
// 	vars->current->args[arg_idx] = joined;
// 	return (arg_idx);
// }
int	proc_join_args(t_vars *vars, char *expanded_val)
{
    int		arg_idx;
    char	*joined;

    // Ensure current node and args exist
    if (!vars->current || !vars->current->args || !vars->current->args[0])
        return (-1);

    arg_idx = 0;
    while (vars->current->args[arg_idx + 1])
    {
        arg_idx++;
    }
    joined = ft_strjoin(vars->current->args[arg_idx], expanded_val);
    if (!joined)
        return (-1); // Malloc failure
    free(vars->current->args[arg_idx]); // Free old argument string
    vars->current->args[arg_idx] = joined; // Assign new joined string
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

// 	if (!vars->current->args || !vars->current->args[0])
// 		return (0);
// 	arg_idx = proc_join_args(vars, expanded_val);
// 	if (arg_idx == -1)
// 		return (0);
// 	if (vars->current->arg_quote_type
// 		&& vars->current->arg_quote_type[arg_idx])
// 	{
// 		if (!update_quote_types(vars, arg_idx, expanded_val))
// 			return (0);
// 	}
// 	free(expanded_val);
// 	free(token);
// 	if (vars->adj_state[1])
// 		process_right_adj(input, vars);
// 	return (1);
// }
// int	handle_tok_join(char *input, t_vars *vars, char *expanded_val, char *token)
// {
// 	int	arg_idx;

// 	if (!vars->current->args || !vars->current->args[0])
// 		return (0);
// 	arg_idx = proc_join_args(vars, expanded_val);
// 	if (arg_idx == -1)
// 		return (0);
// 	if (vars->current->arg_quote_type
// 		&& vars->current->arg_quote_type[arg_idx])
// 	{
// 		if (!update_quote_types(vars, arg_idx, expanded_val))
// 			return (0);
// 	}
// 	free(expanded_val);
// 	free(token);
// 	if (vars->adj_state[1])
// 		process_right_adj(input, vars);
// 	return (1);
// }
int	handle_tok_join(char *input, t_vars *vars, char *expanded_val, char *token)
{
    int	arg_idx;
    // --- ADDED DEBUG PRINT ---
    t_node *target_node = vars->current; // Node being merged onto
    fprintf(stderr, "[DEBUG-MERGE] handle_tok_join: Merging '%s' onto node %p (type=%s, content='%s')\n",
            expanded_val, // Content being merged
            (void*)target_node, // Node being merged onto
            target_node ? get_token_str(target_node->type) : "NULL",
            (target_node && target_node->args && target_node->args[0]) ? target_node->args[0] : "N/A");
    // --- END ADDED DEBUG PRINT ---


    if (!vars->current || !vars->current->args || !vars->current->args[0]) // Check if target node is valid for joining
    {
        // Cannot join, free resources and return error
        free(expanded_val);
        free(token);
        return (0);
    }
    arg_idx = proc_join_args(vars, expanded_val); // Tries to join expanded_val onto vars->current->args[last]
    if (arg_idx == -1)
    {
        // Join failed (likely malloc), free resources and return error
        free(expanded_val);
        free(token);
        return (0);
    }

    // If join succeeded, update quote types if they exist for that argument
    if (vars->current->arg_quote_type
        && vars->current->arg_quote_type[arg_idx])
    {
        if (!update_quote_types(vars, arg_idx, expanded_val))
        {
            // Update failed (likely malloc). The join already happened.
            // This is tricky. Ideally, revert join or signal error.
            // For now, free remaining resources and return error.
            free(expanded_val); // expanded_val should be freed even if update fails
            free(token);
            return (0);
        }
    }

    // If join and quote update (if applicable) succeeded:
    free(expanded_val); // Free the original expanded value string (now joined)
    free(token);        // Free the original token string (e.g., "$VAR")

    // Check for right adjacency AFTER joining the current expansion
    check_token_adj(input, vars); // Update adjacency state based on current pos
    if (vars->adj_state[1])
        process_right_adj(input, vars); // Process text immediately following the expansion

    return (1); // Success
}

/*
Process right adjacency for token joining
Parses and joins adjacent text to the current token
Example: echo hello$VARworld -> handle_tok_join joins $VAR to hello,
then process_right_adj is called to join "world" onto "hello[expanded_VAR]".
*/
// void	process_right_adj(char *input, t_vars *vars)
// {
// 	vars->start = vars->pos;
// 	while (input[vars->pos] && !ft_isspace(input[vars->pos])
// 		&& !ft_is_operator(input[vars->pos])
// 		&& input[vars->pos] != '\'' && input[vars->pos] != '"'
// 		&& input[vars->pos] != '$')
// 	{
// 		vars->pos++;
// 	}
// 	handle_right_adj(input, vars);
// 	vars->start = vars->pos;
// }
// void	process_right_adj(char *input, t_vars *vars)
// {
//     vars->start = vars->pos; // Start scanning from current position
//     // Scan forward for the end of the adjacent text segment
//     while (input[vars->pos] && !ft_isspace(input[vars->pos])
//         && !is_operator_char(input[vars->pos]) // Use simpler operator check
//         && input[vars->pos] != '\'' && input[vars->pos] != '"'
//         && input[vars->pos] != '$')
//     {
//         vars->pos++;
//     }
//     // handle_right_adj extracts text from start to pos and joins it
//     handle_right_adj(input, vars);
//     vars->start = vars->pos; // Update start for the next token
// }
void	process_right_adj(char *input, t_vars *vars)
{
    vars->start = vars->pos; // Start scanning from current position
    // Scan forward for the end of the adjacent text segment
    while (input[vars->pos] && !ft_isspace(input[vars->pos])
        && !ft_is_operator(input[vars->pos]) // Use ft_is_operator to check single char
        && input[vars->pos] != '\'' && input[vars->pos] != '"'
        && input[vars->pos] != '$')
    {
        vars->pos++;
    }
    // handle_right_adj extracts text from start to pos and joins it
    handle_right_adj(input, vars);
    vars->start = vars->pos; // Update start for the next token
}

/*
Updates quote types for joined arguments.
Extends the quote type int array to accommodate new characters.
Returns:
- 1 on success.
- 0 on failure.
*/
// int	update_quote_types(t_vars *vars, int arg_idx, char *expanded_val)
// {
// 	int	old_len;
// 	int	new_len;
// 	int	*new_quo_type;
// 	int	*old_quo_type;

// 	if (!vars->current->arg_quote_type
// 		|| !vars->current->arg_quote_type[arg_idx])
// 	{
// 		return (0);
// 	}
// 	old_len = 0;
// 	old_quo_type = vars->current->arg_quote_type[arg_idx];
// 	while (old_quo_type[old_len] != -1)
// 		old_len++;
// 	new_len = ft_strlen(expanded_val);
// 	new_quo_type = malloc(sizeof(int) * (old_len + new_len + 1));
// 	if (!new_quo_type)
// 		return (0);
// 	addon_quo_type_arr(new_quo_type, old_quo_type, new_len);
// 	free(old_quo_type);
// 	vars->current->arg_quote_type[arg_idx] = new_quo_type;
// 	return (1);
// }
int	update_quote_types(t_vars *vars, int arg_idx, char *expanded_val)
{
    int	old_len;
    int	expand_len; // Renamed from new_len for clarity
    int	total_len;
    int	*new_quo_type;
    int	*old_quo_type;

    // Ensure quote types exist for the node and specific argument index
    if (!vars->current || !vars->current->arg_quote_type
        || !vars->current->arg_quote_type[arg_idx])
    {
        // This case should ideally not happen if called correctly,
        // but return 1 (success) as there's nothing to update.
        return (1);
    }

    old_len = 0;
    old_quo_type = vars->current->arg_quote_type[arg_idx];
    // Calculate length of existing quote type array
    while (old_quo_type && old_quo_type[old_len] != -1)
        old_len++;

    expand_len = ft_strlen(expanded_val); // Length of the string appended
    if (expand_len == 0)
        return (1); // Nothing to append

    total_len = old_len + expand_len;
    new_quo_type = malloc(sizeof(int) * (total_len + 1)); // Allocate space for old + new + terminator
    if (!new_quo_type)
        return (0); // Malloc failure

    // Copy old quote types
    if (old_quo_type)
        ft_memcpy(new_quo_type, old_quo_type, old_len * sizeof(int));
    else
        old_len = 0; // Ensure old_len is 0 if old_quo_type was NULL

    // Append 0s for the expanded part (expanded values are treated as unquoted)
    int k = 0;
    while (k < expand_len)
    {
        new_quo_type[old_len + k] = 0;
        k++;
    }
    new_quo_type[total_len] = -1; // Add terminator

    free(old_quo_type); // Free the old array
    vars->current->arg_quote_type[arg_idx] = new_quo_type; // Assign the new array
    return (1); // Success
}
