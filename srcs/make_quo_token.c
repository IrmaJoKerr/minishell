/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_quo_token.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:52:50 by bleow             #+#    #+#             */
/*   Updated: 2025/05/18 10:12:12 by bleow            ###   ########.fr       */
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
// int merge_quoted_token(char *input, char *content, t_vars *vars)
// {
//     char    *dummy_token;
//     int     join_success;

//     // Add debug print before adjacency check
//     fprintf(stderr, "DEBUG-MERGE-CHECK: Checking adjacency={%d,%d} for content '%s'\n", 
//             vars->adj_state[0], vars->adj_state[1], content);
    
//     if (!vars->adj_state[0])
//         return (0);
//     dummy_token = ft_strdup("");
//     if (!dummy_token)
//         return (0);
//     fprintf(stderr, "DEBUG-MERGE: Attempting to merge token '%s' with adj_state={%d,%d}\n",
//             content, vars->adj_state[0], vars->adj_state[1]);
//     join_success = handle_tok_join(input, vars, content, dummy_token);
//     if (!join_success)
//     {
//         free(dummy_token);
//     }
//     else
//     {
//         if (vars->adj_state[1])
//             process_right_adj(input, vars);
//         process_adj(NULL, vars);
//     }
//     return (join_success);
// }
// ...existing code...
int merge_quoted_token(char *input, char *content, t_vars *vars)
{
    char    *dummy_token;
    int     join_success;

    fprintf(stderr, "DEBUG-MERGE-CHECK: Entering merge_quoted_token. Checking adjacency={%d,%d} for content '%s'\n", 
            vars->adj_state[0], vars->adj_state[1], content);
    
    if (!vars->adj_state[0]) // If no left adjacency, cannot merge with a previous token.
    {
        fprintf(stderr, "DEBUG-MERGE-CHECK: No left adjacency (adj_state[0]=%d). Cannot merge. Returning 0.\n", vars->adj_state[0]);
        return (0);
    }

    // A dummy token might be used if handle_tok_join expects a 'token' string to free,
    // but here 'content' is the primary string being joined.
    dummy_token = ft_strdup(""); // This is to satisfy handle_tok_join's signature if it always frees 'token'
    if (!dummy_token)
    {
        fprintf(stderr, "DEBUG-MERGE: ft_strdup for dummy_token FAILED.\n");
        return (0); // Malloc failure
    }

    fprintf(stderr, "DEBUG-MERGE: Attempting to merge (handle_tok_join) content '%s' with adj_state={%d,%d}. Current token: '%s'\n",
            content, vars->adj_state[0], vars->adj_state[1], (vars->current && vars->current->args && vars->current->args[0]) ? vars->current->args[0] : "N/A");
            
    join_success = handle_tok_join(input, vars, content, dummy_token); // content is expanded_val, dummy_token is 'token'
    
    fprintf(stderr, "DEBUG-MERGE: handle_tok_join returned: %d for content '%s'\n", join_success, content);

    if (!join_success)
    {
        fprintf(stderr, "DEBUG-MERGE: handle_tok_join FAILED. Freeing dummy_token.\n");
        free(dummy_token); // dummy_token was not consumed by a successful join
        // 'content' is NOT freed here; the caller (process_quote_char) handles it if merge fails.
    }
    else // Join was successful
    {
        fprintf(stderr, "DEBUG-MERGE: handle_tok_join SUCCEEDED. Dummy_token and content should have been freed by it.\n");
        // dummy_token and content (as expanded_val) are freed by handle_tok_join upon success.
        // Right adjacency is also handled within handle_tok_join if adj_state[1] was true.
        // process_adj(NULL, vars) might be redundant if handle_tok_join already did it,
        // or it's for resetting adj_state for the *next* token.
        // The original code had:
        // if (vars->adj_state[1]) process_right_adj(input, vars); -> This is now inside handle_tok_join
        // process_adj(NULL, vars); -> This resets adj_state for the next token.
        process_adj(NULL, vars); // Reset adjacency for the next tokenizing step.
    }
    fprintf(stderr, "DEBUG-MERGE-CHECK: Exiting merge_quoted_token. Returning %d.\n", join_success);
    return (join_success);
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
int	make_quoted_cmd(char *content, char *input, t_vars *vars)
{
	t_node	*cmd_node;

	cmd_node = initnode(TYPE_CMD, content);
	if (!cmd_node)
	{
		return (token_cleanup_error(content, vars));
	}
	build_token_linklist(vars, cmd_node);
	cleanup_and_process_adj(content, input, vars);
	return (1);
}
