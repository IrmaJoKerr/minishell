/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/05/05 01:36:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Main entry point for tokenization and expansion. 
Tokenizes input and processes expansions.
Returns 1 on success, 0 on failure.
Example: For input "echo "hello
- Tokenizes the initial content
- Detects unclosed quotes and prompts for completion
- Returns completed command string with proper quotes
*/
int	process_input_tokens(char *command, t_vars *vars)
{
	if (!command || !*command)
		return (0);
	cleanup_token_list(vars);
	if (!tokenizer(command, vars))
		return (0);
	return (1);
}

/*
Processes quoted text in the input string.
- Detects if quote is a redirection target based on previous token
- Handles any accumulated text before the quote
- Calls process_quote_char to extract and process quoted content
- Sets next_flag if processing succeeds
- Restores original position on failure
*/
// void	tokenize_quote(char *input, t_vars *vars)
// {
// 	int	is_redir_target;
// 	int	saved_pos;

// 	is_redir_target = 0;
// 	if (vars->prev_type == TYPE_IN_REDIRECT
// 		|| vars->prev_type == TYPE_OUT_REDIRECT
// 		|| vars->prev_type == TYPE_APPEND_REDIRECT
// 		|| vars->prev_type == TYPE_HEREDOC)
// 	{
// 		is_redir_target = 1;
// 	}
// 	handle_text(input, vars);
// 	saved_pos = vars->pos;
// 	if (process_quote_char(input, vars, is_redir_target))
// 	{
// 		vars->next_flag = 1;
// 	}
// 	else
// 	{
// 		vars->pos = saved_pos;
// 	}
// }
// void	tokenize_quote(char *input, t_vars *vars)
// {
//     int	is_redir_target;
//     int	saved_pos;

//     is_redir_target = 0;
//     // ORIGINAL check based on previous token type
//     if (vars->prev_type == TYPE_IN_REDIRECT
//         || vars->prev_type == TYPE_OUT_REDIRECT
//         || vars->prev_type == TYPE_APPEND_REDIRECT
//         || vars->prev_type == TYPE_HEREDOC)
//     {
//         is_redir_target = 1;
//     }

//     // --- ADDED CHECK ---
//     // Also consider it a redirection target if we are adjacent to the left
//     // AND we are already inside a redirection context being built.
//     // Ensure adjacency is checked *before* this decision.
//     // Note: get_quoted_str inside process_quote_char calls check_token_adj.
//     // We might need to call it earlier here if handle_text modifies state needed by check_token_adj.
//     // Let's assume check_token_adj inside get_quoted_str is sufficient for now.
//     // However, for the logic here, we need the state *before* process_quote_char.
//     check_token_adj(input, vars); // Check adjacency state now
//     if (!is_redir_target && vars->adj_state[0] && vars->pipes->in_redir_context)
//     {
//          // --- ADDED DEBUG PRINT ---
//          fprintf(stderr, "[DEBUG-TOKENIZE-QUOTE] Setting is_redir_target=1 based on context and adjacency.\n");
//          // --- END ADDED DEBUG PRINT ---
//          is_redir_target = 1;
//     }
//     // --- END ADDED CHECK ---


//     handle_text(input, vars); // Process text accumulated *before* the quote starts
//     saved_pos = vars->pos;

//     // process_quote_char will call get_quoted_str which calls check_token_adj again.
//     // This is slightly redundant but ensures process_quote_char has the latest state.
//     if (process_quote_char(input, vars, is_redir_target))
//     {
//         vars->next_flag = 1;
//         // Note: process_quote_char and its sub-functions now handle updating
//         // vars->current and potentially vars->prev_type/curr_type implicitly
//         // through build_token_linklist etc. Explicit updates here might conflict.
//     }
//     else
//     {
//         // Restore position on error from process_quote_char
//         vars->pos = saved_pos;
//         // Consider setting an error flag or handling the error more explicitly
//         vars->error_code = ERR_SYNTAX; // Or a more specific error?
//     }
// }
// void	tokenize_quote(char *input, t_vars *vars)
// {
//     int	is_redir_target;
//     int	saved_pos;

//     is_redir_target = 0;
//     // --- ADDED DEBUG ---
//     fprintf(stderr, "[DEBUG-TQ-ENTRY] pos=%d, prev_type=%s, curr_type=%s, in_ctx=%d, last_target=%p\n",
//             vars->pos, get_token_str(vars->prev_type), get_token_str(vars->curr_type),
//             vars->pipes->in_redir_context, (void*)vars->pipes->last_redir_target);
//     // --- END ADDED ---

//     // ORIGINAL check based on previous token type
//     if (vars->prev_type == TYPE_IN_REDIRECT
//         || vars->prev_type == TYPE_OUT_REDIRECT // Check if this matches
//         || vars->prev_type == TYPE_APPEND_REDIRECT
//         || vars->prev_type == TYPE_HEREDOC)
//     {
//         is_redir_target = 1;
//         // --- ADDED DEBUG ---
//         fprintf(stderr, "[DEBUG-TQ-CHECK] is_redir_target=1 based on prev_type=%s\n", get_token_str(vars->prev_type));
//         // --- END ADDED ---
//     }

//     // --- ADDED CHECK ---
//     check_token_adj(input, vars); // Check adjacency state now
//     // --- ADDED DEBUG ---
//     fprintf(stderr, "[DEBUG-TQ-CHECK] Adjacency check: adj[0]=%d\n", vars->adj_state[0]);
//     // --- END ADDED ---
//     if (!is_redir_target && vars->adj_state[0] && vars->pipes->in_redir_context)
//     {
//          fprintf(stderr, "[DEBUG-TOKENIZE-QUOTE] Setting is_redir_target=1 based on context and adjacency.\n");
//          is_redir_target = 1;
//     }
//     // --- END ADDED CHECK ---


//     handle_text(input, vars); // Process text accumulated *before* the quote starts
//     // --- ADDED DEBUG ---
//     fprintf(stderr, "[DEBUG-TQ-AFTER-HANDLE-TEXT] pos=%d, prev_type=%s, curr_type=%s\n",
//             vars->pos, get_token_str(vars->prev_type), get_token_str(vars->curr_type));
//     // --- END ADDED ---

//     saved_pos = vars->pos;

//     // --- ADDED DEBUG ---
//     fprintf(stderr, "[DEBUG-TQ-CALLING-PQC] Calling process_quote_char with is_redir_target=%d\n", is_redir_target);
//     // --- END ADDED ---
//     if (process_quote_char(input, vars, is_redir_target))
//     {
//         vars->next_flag = 1;
//     }
//     else
//     {
//         vars->pos = saved_pos;
//         vars->error_code = ERR_SYNTAX;
//     }
// }
// void	tokenize_quote(char *input, t_vars *vars)
// {
//     int	is_redir_target;
//     int	saved_pos;
//     t_node *prev_node; // Use actual previous node

//     is_redir_target = 0;
//     prev_node = vars->current; // Get the actual last node added before this quote

//     // --- ADDED DEBUG ---
//     fprintf(stderr, "[DEBUG-TQ-ENTRY] pos=%d, prev_node type=%s, curr_type(state)=%s, in_ctx=%d, last_target=%p\n",
//             vars->pos,
//             prev_node ? get_token_str(prev_node->type) : "NULL", // Debug actual prev node
//             get_token_str(vars->curr_type), // State variable curr_type
//             vars->pipes->in_redir_context, (void*)vars->pipes->last_redir_target);
//     // --- END ADDED ---

//     // --- NEW LOGIC ---
//     // Check 1: Is the immediately preceding node a redirection operator needing a target?
//     if (prev_node && is_redirection(prev_node->type) && prev_node->right == NULL)
//     {
//         is_redir_target = 1;
//         fprintf(stderr, "[DEBUG-TQ-CHECK] is_redir_target=1 based on previous node type=%s needing target\n", get_token_str(prev_node->type));
//     }

//     // Check 2: Are we adjacent and already building a redirection target?
//     check_token_adj(input, vars); // Check adjacency state now
//     fprintf(stderr, "[DEBUG-TQ-CHECK] Adjacency check: adj[0]=%d\n", vars->adj_state[0]);
//     if (!is_redir_target && vars->adj_state[0] && vars->pipes->in_redir_context)
//     {
//         is_redir_target = 1;
//         fprintf(stderr, "[DEBUG-TOKENIZE-QUOTE] Setting is_redir_target=1 based on context and adjacency.\n");
//     }
//     // --- END NEW LOGIC ---


//     handle_text(input, vars); // Process text accumulated *before* the quote starts
//     // --- ADDED DEBUG ---
//     // Note: handle_text might change vars->current, affecting subsequent logic if needed.
//     fprintf(stderr, "[DEBUG-TQ-AFTER-HANDLE-TEXT] pos=%d, prev_type(state)=%s, curr_type(state)=%s, vars->current type=%s\n",
//             vars->pos, get_token_str(vars->prev_type), get_token_str(vars->curr_type),
//             vars->current ? get_token_str(vars->current->type) : "NULL");
//     // --- END ADDED ---

//     saved_pos = vars->pos;

//     // --- ADDED DEBUG ---
//     fprintf(stderr, "[DEBUG-TQ-CALLING-PQC] Calling process_quote_char with is_redir_target=%d\n", is_redir_target);
//     // --- END ADDED ---
//     if (process_quote_char(input, vars, is_redir_target))
//     {
//         vars->next_flag = 1;
//     }
//     else
//     {
//         vars->pos = saved_pos;
//         vars->error_code = ERR_SYNTAX; // Or a more specific error?
//     }
// }
void	tokenize_quote(char *input, t_vars *vars)
{
    int	is_redir_target;
    int	saved_pos;
    t_node *prev_node; // Use actual previous node

    is_redir_target = 0;
    prev_node = vars->current; // Get the actual last node added before this quote

    // --- ADDED DEBUG ---
    fprintf(stderr, "[DEBUG-TQ-ENTRY] pos=%d, prev_node type=%s, curr_type(state)=%s, in_ctx=%d, last_target=%p\n",
            vars->pos,
            prev_node ? get_token_str(prev_node->type) : "NULL", // Debug actual prev node
            get_token_str(vars->curr_type), // State variable curr_type
            vars->pipes->in_redir_context, (void*)vars->pipes->last_redir_target);
    // --- END ADDED ---

    // --- NEW LOGIC ---
    // Check 1: Is the immediately preceding node a redirection operator needing a target?
    if (prev_node && is_redirection(prev_node->type) && prev_node->right == NULL)
    {
        is_redir_target = 1;
        fprintf(stderr, "[DEBUG-TQ-CHECK] is_redir_target=1 based on previous node type=%s needing target\n", get_token_str(prev_node->type));
    }

    // Check 2: Are we adjacent and already building a redirection target?
    check_token_adj(input, vars); // Check adjacency state now
    fprintf(stderr, "[DEBUG-TQ-CHECK] Adjacency check: adj[0]=%d\n", vars->adj_state[0]);
    if (!is_redir_target && vars->adj_state[0] && vars->pipes->in_redir_context)
    {
        is_redir_target = 1;
        fprintf(stderr, "[DEBUG-TOKENIZE-QUOTE] Setting is_redir_target=1 based on context and adjacency.\n");
    }
    // --- END NEW LOGIC ---


    handle_text(input, vars); // Process text accumulated *before* the quote starts
    // --- ADDED DEBUG ---
    // Note: handle_text might change vars->current if it creates a node.
    // The 'prev_node' variable captured earlier still holds the node *before* handle_text ran.
    fprintf(stderr, "[DEBUG-TQ-AFTER-HANDLE-TEXT] pos=%d, prev_type(state)=%s, curr_type(state)=%s, vars->current type=%s\n",
            vars->pos, get_token_str(vars->prev_type), get_token_str(vars->curr_type),
            vars->current ? get_token_str(vars->current->type) : "NULL");
    // --- END ADDED ---

    saved_pos = vars->pos;

    // --- ADDED DEBUG ---
    fprintf(stderr, "[DEBUG-TQ-CALLING-PQC] Calling process_quote_char with is_redir_target=%d\n", is_redir_target);
    // --- END ADDED ---
    if (process_quote_char(input, vars, is_redir_target))
    {
        vars->next_flag = 1;
    }
    else
    {
        vars->pos = saved_pos; // Restore position on error
        vars->error_code = ERR_SYNTAX; // Set error code
    }
}

/*
Processes variable expansion tokens in the input string.
- Handles any accumulated text before the $ character
- Extracts and processes the expanded variable token
- Sets next_flag if expansion processing succeeds
- Updates position markers for continued tokenization
Triggered when $ character is encountered outside quotes.
*/
// void	tokenize_expan(char *input, t_vars *vars)
// {
// 	handle_text(input, vars);
// 	if (make_exp_token(input, vars))
// 		vars->next_flag = 1;
// }
void	tokenize_expan(char *input, t_vars *vars)
{
    handle_text(input, vars); // Process text before '$'
    if (make_exp_token(input, vars)) // Process the expansion itself
        vars->next_flag = 1;
    // Error handling within make_exp_token should set vars->error_code if needed
}

/*
Handle whitespace in the input.
Skips consecutive whitespace characters.
Sets next_flag if processing should continue from the loop.
*/
// void	tokenize_white(char *input, t_vars *vars)
// {
// 	handle_text(input, vars);
// 	while (input[vars->pos] && input[vars->pos] <= ' ')
// 	{
// 		vars->pos++;
// 	}
// 	vars->start = vars->pos;
// 	vars->next_flag = 1;
// }
void	tokenize_white(char *input, t_vars *vars)
{
    handle_text(input, vars); // Process text before whitespace
    while (input[vars->pos] && ft_isspace(input[vars->pos])) // Use ft_isspace
    {
        vars->pos++;
    }
    vars->start = vars->pos; // Next token starts after whitespace
    vars->next_flag = 1;     // Signal that pos was advanced
}

/*
Initialize tokenizer state
- Resets position counters
- Clears quote depth
- Resets heredoc state
- Resets adjacency, types, context flags, and token list pointers
*/
// void	init_tokenizer(t_vars *vars)
// {
// 	vars->pos = 0;
// 	vars->start = 0;
// 	vars->quote_depth = 0;
// 	if (vars->pipes->heredoc_delim)
// 	{
// 		free(vars->pipes->heredoc_delim);
// 		vars->pipes->heredoc_delim = NULL;
// 	}
// 	vars->pipes->hd_expand = 0;
// }
// void	init_tokenizer(t_vars *vars)
// {
//     vars->pos = 0;
//     vars->start = 0;
//     vars->quote_depth = 0;
//     if (vars->pipes->heredoc_delim)
//     {
//         free(vars->pipes->heredoc_delim);
//         vars->pipes->heredoc_delim = NULL;
//     }
//     vars->pipes->hd_expand = 0;
//     // Reset adjacency and types at the start of tokenization
//     vars->adj_state[0] = 0;
//     vars->adj_state[1] = 0;
//     vars->adj_state[2] = -1;
//     vars->curr_type = TYPE_NULL;
//     vars->prev_type = TYPE_NULL;
//     vars->head = NULL; // Ensure token list starts fresh
//     vars->current = NULL;
//     vars->pipes->in_redir_context = 0; // Reset redirection context
//     vars->pipes->last_redir_target = NULL;
// }
void	init_tokenizer(t_vars *vars)
{
    vars->pos = 0;
    vars->start = 0;
    vars->quote_depth = 0; // Assuming this is used for nested quotes/expansions
    if (vars->pipes->heredoc_delim)
    {
        free(vars->pipes->heredoc_delim);
        vars->pipes->heredoc_delim = NULL;
    }
    vars->pipes->hd_expand = 0;
    // Reset adjacency state
    vars->adj_state[0] = 0; // Left
    vars->adj_state[1] = 0; // Right
    vars->adj_state[2] = -1;// Guard
    // Reset type tracking
    vars->curr_type = TYPE_NULL;
    vars->prev_type = TYPE_NULL;
    // Reset token list pointers
    vars->head = NULL;
    vars->current = NULL;
    // Reset redirection context tracking
    vars->pipes->in_redir_context = 0;
    vars->pipes->last_redir_target = NULL;
    // Reset error code
    vars->error_code = 0;
}
