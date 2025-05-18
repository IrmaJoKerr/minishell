/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_exp_token_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 09:02:14 by bleow             #+#    #+#             */
/*   Updated: 2025/05/18 10:10:30 by bleow            ###   ########.fr       */
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
// 	fprintf(stderr, "DEBUG-JOIN: Joining '%s' with last argument of current token\n", 
//             expanded_val);
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

    if (!vars->current || !vars->current->args || !vars->current->args[0])
    {
        fprintf(stderr, "DEBUG-JOIN: No current token or args to join with.\n");
        return (0);
    }
    fprintf(stderr, "DEBUG-JOIN: Joining '%s' with last argument ('%s') of current token ('%s')\n",
            expanded_val,
            vars->current->args[ft_arrlen(vars->current->args) -1],
            vars->current->args[0]);
    arg_idx = proc_join_args(vars, expanded_val);
    if (arg_idx == -1)
    {
        fprintf(stderr, "DEBUG-JOIN: proc_join_args FAILED for expanded_val '%s'.\n", expanded_val);
        return (0);
    }
    fprintf(stderr, "DEBUG-JOIN: proc_join_args returned arg_idx %d. Current token args[0] now: '%s'\n", arg_idx, vars->current->args[0]);

    if (vars->current->arg_quote_type
        && vars->current->arg_quote_type[arg_idx])
    {
        fprintf(stderr, "DEBUG-JOIN: Attempting to update quote types for arg_idx %d\n", arg_idx);
        if (!update_quote_types(vars, arg_idx, expanded_val))
        {
            fprintf(stderr, "DEBUG-JOIN: update_quote_types FAILED for arg_idx %d with expanded_val '%s'. Aborting join success.\n", arg_idx, expanded_val);
            // If update_quote_types fails, the string was already joined by proc_join_args.
            // This might leave inconsistent state if not handled carefully.
            return (0);
        }
        fprintf(stderr, "DEBUG-JOIN: update_quote_types SUCCEEDED for arg_idx %d\n", arg_idx);
    }
    else
    {
        fprintf(stderr, "DEBUG-JOIN: No quote types to update for arg_idx %d or arg_quote_type array is NULL.\n", arg_idx);
    }

    free(expanded_val); // This was freed in the original sub_make_exp_token if new_exp_token was not called. Here it's joined.
    free(token);        // Original token string (e.g., "$VAR")
    if (vars->adj_state[1])
    {
        fprintf(stderr, "DEBUG-JOIN: Left join successful, now processing right adjacency.\n");
        process_right_adj(input, vars);
    }
    fprintf(stderr, "DEBUG-JOIN: Token join SUCCEEDED.\n");
    return (1);
}

/*
Process right adjacency for token joining
Parses and joins adjacent text to the current token
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
/*
Processes right-adjacent text after a quote or special character.
- Advances position until it finds a space, operator, or quote
- Calls handle_right_adj to join this text with the previous token
- Used when handling right adjacency after quotes
Example:
For echo hello""world, properly joins "world" to "hello"
*/
// void	process_right_adj(char *input, t_vars *vars)
// {
//     fprintf(stderr, "DEBUG-PROCESS-ADJ: Starting right adjacency scan at position %d\n", vars->pos);
//     vars->start = vars->pos;
//     while (input[vars->pos] && !ft_isspace(input[vars->pos])
//         && !ft_is_operator(input[vars->pos])
//         && input[vars->pos] != '\'' && input[vars->pos] != '"'
//         && input[vars->pos] != '$')
//     {
//         vars->pos++;
//     }
//     fprintf(stderr, "DEBUG-PROCESS-ADJ: Found right adjacent text ending at position %d\n", vars->pos);
//     handle_right_adj(input, vars);
//     vars->start = vars->pos;
// }
void	process_right_adj(char *input, t_vars *vars)
{
    fprintf(stderr, "DEBUG-PRIGHTADJ: Entering process_right_adj (make_exp_token_utils.c), input_pos %d, current_start %d\n", vars->pos, vars->start);
    vars->start = vars->pos; // Set start to current position for substr
    fprintf(stderr, "DEBUG-PRIGHTADJ: Set vars->start to %d (current vars->pos)\n", vars->start);

    while (input[vars->pos] && !ft_isspace(input[vars->pos])
        && !ft_is_operator(input[vars->pos])
        && input[vars->pos] != '\'' && input[vars->pos] != '"'
        && input[vars->pos] != '$')
    {
        fprintf(stderr, "DEBUG-PRIGHTADJ: Loop: char '%c' at pos %d is part of adjacent text.\n", input[vars->pos], vars->pos);
        vars->pos++;
    }
    fprintf(stderr, "DEBUG-PRIGHTADJ: Loop ended. Current char '%c' at pos %d. Text to process is from index %d to %d.\n", input[vars->pos] ? input[vars->pos] : '0', vars->pos, vars->start, vars->pos);
    fprintf(stderr, "DEBUG-PRIGHTADJ: Calling handle_right_adj (from tokenize_utils.c)\n");
    handle_right_adj(input, vars); // This is from tokenize_utils.c
    vars->start = vars->pos; // Update start for next token
    fprintf(stderr, "DEBUG-PRIGHTADJ: Exiting process_right_adj, new start for next token is %d\n", vars->start);
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
    int		*old_types;
    int		*new_types;
    size_t	old_len;
    size_t	expanded_len;
    size_t	total_len;

    fprintf(stderr, "DEBUG-UPDATEQ: Entering update_quote_types for arg_idx %d, expanded_val '%s'\n", arg_idx, expanded_val ? expanded_val : "NULL");
    if (!vars || !vars->current || !vars->current->arg_quote_type
        || !vars->current->arg_quote_type[arg_idx] || !expanded_val)
    {
        fprintf(stderr, "DEBUG-UPDATEQ: Invalid parameters or missing data.\n");
        return (0);
    }
    old_types = vars->current->arg_quote_type[arg_idx];
    old_len = 0;
    while (old_types[old_len] != -1)
        old_len++;
    expanded_len = ft_strlen(expanded_val);
    total_len = old_len + expanded_len;
    fprintf(stderr, "DEBUG-UPDATEQ: old_len: %zu, expanded_len: %zu, total_len: %zu\n", old_len, expanded_len, total_len);

    new_types = malloc(sizeof(int) * (total_len + 1));
    fprintf(stderr, "DEBUG-UPDATEQ: malloc for new_types (size %zu bytes): %p\n", sizeof(int) * (total_len + 1), (void*)new_types);
    if (!new_types)
    {
        fprintf(stderr, "DEBUG-UPDATEQ: MALLOC FAILED for new_types\n");
        // old_types is part of vars->current->arg_quote_type[arg_idx] and should not be freed here directly
        // as it's managed by the node. If this function fails, the caller should handle cleanup.
        return (0);
    }
    ft_memcpy(new_types, old_types, sizeof(int) * old_len);
    size_t i = 0;
    while (i < expanded_len)
    {
        new_types[old_len + i] = 0; // Assuming expanded part is unquoted
        i++;
    }
    new_types[total_len] = -1;
    free(vars->current->arg_quote_type[arg_idx]); // Free the old specific arg_quote_type array
    vars->current->arg_quote_type[arg_idx] = new_types;
    fprintf(stderr, "DEBUG-UPDATEQ: Successfully updated quote types for arg_idx %d. New array addr: %p\n", arg_idx, (void*)new_types);
    return (1);
}
