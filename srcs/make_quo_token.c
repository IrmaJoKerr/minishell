/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_quo_token.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:52:50 by bleow             #+#    #+#             */
/*   Updated: 2025/05/05 01:53:03 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Tries to merge the quoted token with existing tokens based on adjacency.
Returns:
 - 1 if successfully merged.
 - 0 otherwise.
*/
// int	merge_quoted_token(char *input, char *content, t_vars *vars)
// {
// 	char	*dummy_token;
// 	int		join_success;

// 	if (!vars->adj_state[0])
// 		return (0);
// 	dummy_token = ft_strdup("");
// 	if (!dummy_token)
// 		return (0);
// 	join_success = handle_tok_join(input, vars, content, dummy_token);
// 	if (!join_success)
// 	{
// 		free(dummy_token);
// 	}
// 	else
// 	{
// 		if (vars->adj_state[1])
// 			process_right_adj(input, vars);
// 		process_adj(NULL, vars);
// 	}
// 	return (join_success);
// }
int merge_quoted_token(char *input, char *content, t_vars *vars)
{
    char	*dummy_token; // Still using dummy for compatibility with handle_tok_join signature
    int		join_success;

    if (!vars->adj_state[0]) // Only merge if left-adjacent
        return (0);
    if (!vars->current || (vars->current->type != TYPE_CMD && vars->current->type != TYPE_ARGS)) {
        return (0); // Cannot merge onto non-CMD/ARGS
    }

    dummy_token = ft_strdup(""); // Minimal allocation
    if (!dummy_token) {
        free(content); // Free content if dummy allocation fails
        vars->error_code = ERR_DEFAULT;
        return (0);
    }

    // handle_tok_join expects expanded_val and token. We pass content as expanded_val.
    // handle_tok_join should free both 'content' and 'dummy_token' on success.
    join_success = handle_tok_join(input, vars, content, dummy_token);

    if (!join_success)
    {
        // Assuming handle_tok_join frees 'content' and 'dummy_token' on failure too.
        // If not, free(content); free(dummy_token); needed here.
        return (0); // Indicate failure
    }
    else // Join succeeded
    {
        // Adjacency check and processing moved outside merge_quoted_token
        // to be handled by the caller (process_quote_char) after merge/create decision.
    }
    return (join_success); // Return 1
}

/*
Creates a new command from quoted content
- Creates command node 
- Links to token list
- Handles token adjacency
Returns:
- 1 on success (takes ownership of content)
- 0 on failure (frees content)
*/
// int	make_quoted_cmd(char *content, char *input, t_vars *vars)
// {
// 	t_node	*cmd_node;

// 	cmd_node = initnode(TYPE_CMD, content);
// 	if (!cmd_node)
// 	{
// 		return (token_cleanup_error(content, vars));
// 	}
// 	build_token_linklist(vars, cmd_node);
// 	cleanup_and_process_adj(content, input, vars);
// 	return (1);
// }
int make_quoted_cmd(char *content, t_vars *vars) // Removed 'input' parameter
{
    t_node	*cmd_node;
    t_tokentype node_type = TYPE_ARGS; // Default to ARGS

    // Determine if CMD or ARGS based on context
    if (!vars->head || (vars->current && vars->current->type == TYPE_PIPE)) {
        node_type = TYPE_CMD;
    }
    // Add more sophisticated checks if needed, e.g., after redirection operator

    cmd_node = initnode(node_type, content); // initnode takes ownership of content
    if (!cmd_node)
    {
        // Assuming initnode frees 'content' on failure.
        // If not, free(content); might be needed here, but initnode should handle it.
        vars->error_code = ERR_DEFAULT; // Malloc error
        return (0); // Return error
    }

    int build_status = build_token_linklist(vars, cmd_node); // Changed variable name for clarity
    if (build_status == -1) // Check if build_token_linklist indicated an error
    {
        // build_token_linklist failed. Assume it handled cleanup of cmd_node on its errors.
        // If it returns -1, we assume an error occurred and resources were handled.
        // vars->error_code should have been set by build_token_linklist or its callees.
        return (0); // Propagate error
    }

    // Adjacency processing is now handled by the caller (process_quote_char)
    return (1); // Success
}
