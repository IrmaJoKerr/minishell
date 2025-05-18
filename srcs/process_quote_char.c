/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:54:37 by bleow             #+#    #+#             */
/*   Updated: 2025/05/18 10:52:16 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Process quoted content, including variable expansion in double quotes.
Finds the appropriate command node for the content.
Modifies the content pointer if expansion is needed.
Returns:
- The command node.
- NULL if not found/error.
*/
t_node	*process_quoted_str(char **content_ptr, int quote_type, t_vars *vars)
{
	t_node	*cmd_node;
	char	*expanded;

	if (quote_type == TYPE_DOUBLE_QUOTE && ft_strchr(*content_ptr, '$'))
	{
		expanded = expand_quoted_str(*content_ptr, vars);
		free(*content_ptr);
		if (!expanded)
			return (NULL);
		*content_ptr = expanded;
	}
	cmd_node = find_cmd(vars->head, NULL, FIND_LAST, vars);
	return (cmd_node);
}

/*
Links a file node into the token list after a redirection node.
- Sets redirection->right pointer to the file node
- Handles proper linked list connections
- Updates vars->current pointer
Works with handle_redir_target().
*/
void	link_file_to_redir(t_node *redir_node, t_node *file_node, t_vars *vars)
{
	redir_node->right = file_node;
	if (redir_node->next)
	{
		file_node->next = redir_node->next;
		redir_node->next->prev = file_node;
	}
	redir_node->next = file_node;
	file_node->prev = redir_node;
	vars->current = file_node;
}

/*
Handles quoted text as a redirection target.
- Finds the relevant redirection node in the token list.
- Creates a file node with the quoted content.
- Links the file node to the redirection operator.
- Sets error code if no valid redirection is found.
Returns:
- 1 on success (content is freed)
- 0 on failure (content is freed)
Works with process_quote_char() for handling quoted filenames.
Example:
- For "echo > "file.txt"", creates file node for "file.txt".
*/
int	handle_redir_target(char *content, t_vars *vars)
{
	t_node	*redir_node;
	t_node	*file_node;

	redir_node = find_last_redir(vars);
	if (redir_node && is_redirection(redir_node->type))
	{
		file_node = initnode(TYPE_ARGS, content);
		if (!file_node)
		{
			free(content);
			return (0);
		}
		link_file_to_redir(redir_node, file_node, vars);
		free(content);
		return (1);
	}
	free(content);
	vars->error_code = ERR_SYNTAX;
	return (0);
}

/*
Master control function for processing quoted text in shell input.
This function controls the complete handling of quoted strings:
 - Extracts content from between quotes (single or double).
 - Handles variable expansion for double-quoted text.
 - Creates appropriate token structures based on context.
 - Processes special cases like redirection targets (">file.txt").
 - Manages token adjacency and merging.
Return:
 - 1 on successful processing.
 - 0 on extraction failure or other errors.
Example flows:
 - "echo hello" -> Creates command with argument.
 - echo "hello world" -> Appends argument to existing command.
 - echo > "file.txt" -> Creates file node for redirection.
*/
// int	process_quote_char(char *input, t_vars *vars, int is_redir_target)
// {
// 	int		quote_type;
// 	char	*content;
// 	t_node	*cmd_node;

// 	content = get_quoted_str(input, vars, &quote_type);
// 	if (!content)
// 		return (0);
// 	if (is_redir_target)
// 		return (handle_redir_target(content, vars));
// 	cmd_node = (process_quoted_str(&content, quote_type, vars));
// 	if (!cmd_node && vars->adj_state[0] == 0)
// 		return (make_quoted_cmd(content, input, vars));
// 	else if (!cmd_node)
// 	{
// 		if (!merge_quoted_token(input, content, vars))
// 			return (token_cleanup_error(content, vars));
// 		return (1);
// 	}
// 	if (!merge_quoted_token(input, content, vars))
// 	{
// 		append_arg(cmd_node, content, quote_type);
// 		cleanup_and_process_adj(content, input, vars);
// 	}
// 	return (1);
// }
// int process_quote_char(char *input, t_vars *vars, int is_redir_target)
// {
//     int     quote_type;
//     char    *content;
//     t_node  *cmd_node; // Renamed from saved_current for clarity, was used differently

//     fprintf(stderr, "DEBUG-QUOTE-START: Entering process_quote_char at pos %d with adj_state={%d,%d}, is_redir_target=%d\n", 
//             vars->pos, vars->adj_state[0], vars->adj_state[1], is_redir_target);

//     content = get_quoted_str(input, vars, &quote_type); // check_token_adj is called inside get_quoted_str
//     if (!content)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Failed to get quoted string content (get_quoted_str returned NULL).\n");
//         return (0);
//     }
//     fprintf(stderr, "DEBUG-QUOTE-PROCESS: Got quoted content: '%s' (type: %d from quote char, actual type may vary). Adjacency after get_quoted_str: {%d,%d}\n", 
//             content, quote_type, vars->adj_state[0], vars->adj_state[1]);
    
//     if (content && *content == '\0' && quote_type != 0) // Only for actual quotes ' or ", not for quote_type 0
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Detected empty quotes ('%s') with adj_state={%d,%d}\n", 
//                 content, vars->adj_state[0], vars->adj_state[1]);
        
//         if (vars->adj_state[0] && vars->adj_state[1])
//         {
//             fprintf(stderr, "DEBUG-QUOTE-PROCESS: Empty quotes acting as join point. Current token before finding last: %p ('%s')\n",
//                 (void*)vars->current, (vars->current && vars->current->args && vars->current->args[0]) ? vars->current->args[0] : "N/A");
            
//             t_node *last_token = vars->head; // Find the actual last token in the list to join with
//             if (last_token) {
//                 while (last_token->next)
//                     last_token = last_token->next;
//             }
//             // It's crucial that vars->current points to the token we want to append to *before* calling process_right_adj
//             // If the empty quote is meant to join 'hello' and 'world' in 'hello""world',
//             // vars->current should be the token containing 'hello'.
//             if (last_token && last_token->type != TYPE_PIPE) { // Don't set current to a pipe
//                  vars->current = last_token;
//             }

//             fprintf(stderr, "DEBUG-QUOTE-PROCESS: Set vars->current to last_token: %p ('%s'). Calling process_right_adj (make_exp_token_utils.c)\n",
//                 (void*)vars->current, (vars->current && vars->current->args && vars->current->args[0]) ? vars->current->args[0] : "N/A");
            
//             free(content);
//             process_right_adj(input, vars); // This is from make_exp_token_utils.c
//             fprintf(stderr, "DEBUG-QUOTE-PROCESS: Returned from process_right_adj for empty quotes. Returning 1.\n");
//             return (1);
//         }
//         // If not adj_state[0] && adj_state[1], it's an empty quote that might just be an empty arg or needs merging differently.
//         // The existing logic below will handle it via merge_quoted_token or make_quoted_cmd/append_arg.
//     }
    
//     if (is_redir_target)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Handling as redirection target: '%s'\n", content);
//         int result = handle_redir_target(content, vars); // handle_redir_target should free content
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: handle_redir_target returned %d\n", result);
//         return result;
//     }
    
//     fprintf(stderr, "DEBUG-QUOTE-PROCESS: Processing as regular quoted string: '%s'\n", content);
//     cmd_node = process_quoted_str(&content, quote_type, vars); // content might be re-assigned if expanded
//     fprintf(stderr, "DEBUG-QUOTE-PROCESS: process_quoted_str returned cmd_node: %p. Content after: '%s'\n", (void*)cmd_node, content);
    
//     if (!cmd_node && vars->adj_state[0] == 0)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: No cmd_node and no left adjacency. Creating new quoted command with content '%s'.\n", content);
//         return (make_quoted_cmd(content, input, vars)); // make_quoted_cmd should free content or use it
//     }
//     else if (!cmd_node) // Has left adjacency but no specific cmd_node found by process_quoted_str (e.g. current is a pipe)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: No cmd_node, but has left adj_state[0]=%d. Attempting merge_quoted_token for '%s'.\n", vars->adj_state[0], content);
//         if (!merge_quoted_token(input, content, vars)) // merge_quoted_token calls handle_tok_join
//         {
//             fprintf(stderr, "DEBUG-QUOTE-PROCESS: merge_quoted_token FAILED for '%s'. Cleaning up.\n", content);
//             return (token_cleanup_error(content, vars)); // Frees content
//         }
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: merge_quoted_token SUCCEEDED for '%s'.\n", content);
//         // content is typically freed by handle_tok_join if successful
//         return (1);
//     }
    
//     // cmd_node is valid (found by process_quoted_str, likely vars->current or similar)
//     fprintf(stderr, "DEBUG-QUOTE-PROCESS: Found cmd_node '%s' (%p). Attempting merge_quoted_token for content '%s'.\n", 
//             cmd_node->args ? cmd_node->args[0] : "NULL", (void*)cmd_node, content);
//     if (!merge_quoted_token(input, content, vars)) // merge_quoted_token calls handle_tok_join
//     {
//         // If merge fails (e.g., update_quote_types fails in handle_tok_join, or no left adj for merge logic)
//         // then append as a new argument.
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: merge_quoted_token FAILED for content '%s'. Appending as argument to cmd_node '%s'.\n", content, cmd_node->args[0]);
//         append_arg(cmd_node, content, quote_type); // append_arg will strdup content
//         cleanup_and_process_adj(content, input, vars); // Frees original content, handles right adj
//     }
//     else
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: merge_quoted_token SUCCEEDED for content '%s'. Content should have been handled (e.g. freed) by merge.\n", content);
//         // If merge_quoted_token succeeded, content was consumed (joined and freed) by handle_tok_join.
//         // Adjacency processing (right adj) is also handled within merge_quoted_token/handle_tok_join.
//     }
    
//     fprintf(stderr, "DEBUG-QUOTE-PROCESS: Exiting process_quote_char. Returning 1.\n");
//     return (1);
// }
// int process_quote_char(char *input, t_vars *vars, int is_redir_target)
// {
//     int     quote_type;
//     char    *content;
//     t_node  *cmd_node;
//     int     saved_adj[3];  // Added to save adjacency state

//     // Save adjacency state before it gets modified by get_quoted_str
//     saved_adj[0] = vars->adj_state[0];
//     saved_adj[1] = vars->adj_state[1];
//     saved_adj[2] = vars->adj_state[2];

//     fprintf(stderr, "DEBUG-QUOTE-START: Processing at pos %d with adjacency={%d,%d}\n", 
//             vars->pos, vars->adj_state[0], vars->adj_state[1]);

//     // Content extraction - this modifies adjacency state internally
//     content = get_quoted_str(input, vars, &quote_type);
//     if (!content)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Failed to get quoted string content\n");
//         return (0);
//     }

//     // Restore the saved adjacency state
//     vars->adj_state[0] = saved_adj[0];
//     vars->adj_state[1] = saved_adj[1];
//     vars->adj_state[2] = saved_adj[2];

//     fprintf(stderr, "DEBUG-QUOTE-RESTORED: Restored adjacency to {%d,%d} for content '%s'\n", 
//             vars->adj_state[0], vars->adj_state[1], content);
    
//     fprintf(stderr, "DEBUG-QUOTE-PROCESS: Processing quoted content: '%s' (type: %d)\n", 
//             content, quote_type);
    
//     // Special handling for empty quotes with adjacency
//     if (content && *content == '\0' && quote_type != 0)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Empty quotes with adjacency={%d,%d}\n", 
//                 vars->adj_state[0], vars->adj_state[1]);
        
//         // Both left and right adjacency means we need to connect tokens
//         if (vars->adj_state[0] && vars->adj_state[1])
//         {
//             fprintf(stderr, "DEBUG-QUOTE-PROCESS: Empty quotes acting as join point\n");
            
//             // Find the last token in the list to join with
//             t_node *last_token = vars->head;
//             if (last_token) {
//                 while (last_token->next)
//                     last_token = last_token->next;
                
//                 // Save the current token pointer that process_right_adj expects
//                 vars->current = last_token;
//                 fprintf(stderr, "DEBUG-QUOTE-PROCESS: Set vars->current to last token: %p ('%s')\n", 
//                        (void*)vars->current, vars->current->args ? vars->current->args[0] : "NULL");
//             }
            
//             free(content);  // Content is empty, free it
//             process_right_adj(input, vars);
//             fprintf(stderr, "DEBUG-QUOTE-PROCESS: Returned from process_right_adj for empty quotes\n");
//             return (1);
//         }
//     }
    
//     if (is_redir_target)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Handling as redirection target\n");
//         int result = handle_redir_target(content, vars);
//         return result;
//     }
    
//     fprintf(stderr, "DEBUG-QUOTE-PROCESS: Processing as regular quoted string\n");
//     cmd_node = process_quoted_str(&content, quote_type, vars);
    
//     if (!cmd_node && vars->adj_state[0] == 0)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Creating new quoted command\n");
//         return (make_quoted_cmd(content, input, vars));
//     }
//     else if (!cmd_node)
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: No command node found with adj_state=%d\n", 
//                 vars->adj_state[0]);
//         if (!merge_quoted_token(input, content, vars))
//         {
//             fprintf(stderr, "DEBUG-QUOTE-PROCESS: Token merge failed\n");
//             return (token_cleanup_error(content, vars));
//         }
//         return (1);
//     }
    
//     fprintf(stderr, "DEBUG-QUOTE-PROCESS: Found command node '%s'\n", 
//             cmd_node->args ? cmd_node->args[0] : "NULL");
//     if (!merge_quoted_token(input, content, vars))
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Appending as argument to command\n");
//         append_arg(cmd_node, content, quote_type);
        
//         // If we have right adjacency and valid content, process it
//         if (vars->adj_state[1] && content && *content != '\0')
//         {
//             fprintf(stderr, "DEBUG-QUOTE-PROCESS: Processing right adjacency after quotes\n");
//             cleanup_and_process_adj(content, input, vars);
//         }
//         else
//         {
//             // No right adjacency, just clean up
//             free(content);
//         }
//     }
//     else
//     {
//         fprintf(stderr, "DEBUG-QUOTE-PROCESS: Token was merged\n");
//     }
    
//     return (1);
// }
int process_quote_char(char *input, t_vars *vars, int is_redir_target)
{
    int     quote_type;
    char    *content;
    t_node  *cmd_node;
    int     saved_adj[3];  // Add this to save adjacency state

    // Save current position for proper adjacency check
    int saved_pos = vars->pos;
    
    // Check adjacency BEFORE get_quoted_str modifies vars->pos
    check_token_adj(input, vars);
    
    // Save the correctly determined adjacency state
    saved_adj[0] = vars->adj_state[0];
    saved_adj[1] = vars->adj_state[1];
    saved_adj[2] = vars->adj_state[2];

    fprintf(stderr, "DEBUG-QUOTE-START: Processing at pos %d with adjacency={%d,%d}\n", 
            vars->pos, vars->adj_state[0], vars->adj_state[1]);

    content = get_quoted_str(input, vars, &quote_type);
    if (!content)
    {
        vars->pos = saved_pos; // Restore position on error
        return (0);
    }

    // Restore the adjacency state detected at the quote character
    vars->adj_state[0] = saved_adj[0];
    vars->adj_state[1] = saved_adj[1]; 
    vars->adj_state[2] = saved_adj[2];

    fprintf(stderr, "DEBUG-QUOTE-RESTORED: Restored adjacency to {%d,%d} for content '%s'\n", 
            vars->adj_state[0], vars->adj_state[1], content);
    
    fprintf(stderr, "DEBUG-QUOTE-PROCESS: Processing quoted content: '%s' (type: %d)\n", 
            content, quote_type);
    
    // Special handling for empty quotes with adjacency
    if (content && *content == '\0' && quote_type != 0)
    {
        fprintf(stderr, "DEBUG-QUOTE-PROCESS: Empty quotes with adjacency={%d,%d}\n", 
                vars->adj_state[0], vars->adj_state[1]);
        
        // Both left and right adjacency means we need to connect tokens
        if (vars->adj_state[0] && vars->adj_state[1])
        {
            fprintf(stderr, "DEBUG-QUOTE-PROCESS: Empty quotes acting as join point\n");
            
            // Find the last token in the list to join with
            t_node *last_token = vars->head;
            if (last_token) {
                while (last_token->next)
                    last_token = last_token->next;
                
                // Save the current token pointer that process_right_adj expects
                vars->current = last_token;
                fprintf(stderr, "DEBUG-QUOTE-PROCESS: Set vars->current to last token: %p ('%s')\n", 
                       (void*)vars->current, vars->current->args ? vars->current->args[0] : "NULL");
            }
            
            free(content);  // Content is empty, free it
            process_right_adj(input, vars);
            fprintf(stderr, "DEBUG-QUOTE-PROCESS: Returned from process_right_adj for empty quotes\n");
            return (1);
        }
    }
    
    if (is_redir_target)
    {
        fprintf(stderr, "DEBUG-QUOTE-PROCESS: Handling as redirection target\n");
        int result = handle_redir_target(content, vars);
        return result;
    }
    
    fprintf(stderr, "DEBUG-QUOTE-PROCESS: Processing as regular quoted string\n");
    cmd_node = process_quoted_str(&content, quote_type, vars);
    
    if (!cmd_node && vars->adj_state[0] == 0)
    {
        fprintf(stderr, "DEBUG-QUOTE-PROCESS: Creating new quoted command\n");
        return (make_quoted_cmd(content, input, vars));
    }
    else if (!cmd_node)
    {
        fprintf(stderr, "DEBUG-QUOTE-PROCESS: No command node found with adj_state=%d\n", 
                vars->adj_state[0]);
        if (!merge_quoted_token(input, content, vars))
        {
            fprintf(stderr, "DEBUG-QUOTE-PROCESS: Token merge failed\n");
            return (token_cleanup_error(content, vars));
        }
        return (1);
    }
    
    fprintf(stderr, "DEBUG-QUOTE-PROCESS: Found command node '%s'\n", 
            cmd_node->args ? cmd_node->args[0] : "NULL");
    if (!merge_quoted_token(input, content, vars))
    {
        fprintf(stderr, "DEBUG-QUOTE-PROCESS: Appending as argument to command\n");
        append_arg(cmd_node, content, quote_type);
        
        // If we have right adjacency and valid content, process it
        if (vars->adj_state[1] && content && *content != '\0')
        {
            fprintf(stderr, "DEBUG-QUOTE-PROCESS: Processing right adjacency after quotes\n");
            cleanup_and_process_adj(content, input, vars);
        }
        else
        {
            // No right adjacency, just clean up
            free(content);
        }
    }
    else
    {
        fprintf(stderr, "DEBUG-QUOTE-PROCESS: Token was merged\n");
    }
    
    return (1);
}

/*
Validates that all redirection operators in the token list have valid targets.
- Ensures no redirection is at the end of input
- Ensures no redirection is followed by another operator
- Reports appropriate syntax errors
Returns:
- 1 if all redirections have valid targets
- 0 otherwise (with error_code set)
*/
int	validate_redir_targets(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
		{
			if (!validate_single_redir(current, vars))
				return (0);
		}
		current = current->next;
	}
	return (1);
}
