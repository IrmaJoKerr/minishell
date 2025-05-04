/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quote_char_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:56:41 by bleow             #+#    #+#             */
/*   Updated: 2025/05/04 23:46:52 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 Handles failure when merging quoted tokens
- Reports error message
- Frees content memory
- Resets adjacency state
Returns:
- 0 to indicate error
*/
int	token_cleanup_error(char *content, t_vars *vars)
{
	free(content);
	process_adj(NULL, vars);
	return (0);
}

/*
Cleans up after token processing and handles adjacency states.
- Frees content memory.
- Processes right adjacency if needed.
- Resets adjacency state.
*/
void	cleanup_and_process_adj(char *content, char *input, t_vars *vars)
{
	free(content);
	if (vars->adj_state[1])
		process_right_adj(input, vars);
	process_adj(NULL, vars);
}

/*
Finds the most relevant redirection node in the token list.
Checks in the following order of priority:
 - Current node if it's a redirection
 - Recent previous nodes (up to 3 back)
 - Last redirection in the entire token list
Returns:
 - Pointer to the located redirection node
 - NULL if no redirection node exists
Works with process_quote_char() for redirection target handling.
*/
// t_node	*find_last_redir(t_vars *vars)
// {
// 	t_node	*current;
// 	t_node	*last_redir;
// 	int		i;

// 	last_redir = NULL;
// 	if (vars->current && is_redirection(vars->current->type))
// 		return (vars->current);
// 	current = vars->current;
// 	i = 0;
// 	while (current && current->prev && i < 3)
// 	{
// 		current = current->prev;
// 		i++;
// 		if (is_redirection(current->type))
// 			return (current);
// 	}
// 	current = vars->head;
// 	while (current)
// 	{
// 		if (is_redirection(current->type))
// 			last_redir = current;
// 		current = current->next;
// 	}
// 	return (last_redir);
// }
// t_node *find_last_redir(t_vars *vars)
// {
//     t_node *current;
//     // t_node *last_redir = NULL;
//     int already_has_target = 0;
    
//     // First check if current node is a redirection
//     if (vars->current && is_redirection(vars->current->type)) {
//         // Check if it already has a target
//         if (vars->current->right != NULL) {
//             already_has_target = 1;
//         } else {
//             already_has_target = 0;
//         }
        
//         if (!already_has_target)
//             return vars->current;
//     }
    
//     // If not, look backwards for most recent redirection without a target
//     current = vars->current;
//     while (current && current->prev) {
//         current = current->prev;
//         if (is_redirection(current->type)) {
//             if (current->right == NULL)
//                 return current;
//         }
        
//         // Stop looking if we hit a pipe (different command context)
//         if (current->type == TYPE_PIPE)
//             break;
//     }
    
//     // If we get here, we're not in a redirection context
//     return NULL;
// }
// t_node *find_last_redir(t_vars *vars)
// {
//     t_node *current;
//     int already_has_target = 0;
    
//     // First check if current node is a redirection
//     if (vars->current && is_redirection(vars->current->type)) {
//         // Check if it already has a target
//         if (vars->current->right != NULL) {
//             already_has_target = 1;
//         } else {
//             already_has_target = 0;
//         }
        
//         if (!already_has_target)
//             return vars->current;
//     }
    
//     // If not, look backwards for most recent redirection without a target
//     current = vars->current;
//     while (current && current->prev) {
//         current = current->prev;
//         if (is_redirection(current->type)) {
//             if (current->right == NULL)
//                 return current;
//         }
        
//         // Stop looking if we hit a pipe (different command context)
//         if (current->type == TYPE_PIPE)
//             break;
//     }
    
//     // If we get here, we're not in a redirection context
//     return NULL;
// }
t_node *find_last_redir(t_vars *vars)
{
    t_node *current;
    int already_has_target = 0;
    
    // First check if current node is a redirection
    if (vars->current && is_redirection(vars->current->type)) {
        // Check if it already has a target
        if (vars->current->right != NULL) {
            already_has_target = 1;
        } else {
            already_has_target = 0;
        }
        
        if (!already_has_target)
            return vars->current;
    }
    
    // If not, look backwards for most recent redirection without a target
    current = vars->current;
    while (current && current->prev) {
        current = current->prev;
        if (is_redirection(current->type)) {
            if (current->right == NULL)
                return current;
        }
        
        // Stop looking if we hit a pipe (different command context)
        if (current->type == TYPE_PIPE)
            break;
    }
    
    // If we get here, we're not in a redirection context
    return NULL;
}

/*
Validates that a single redirection token has a valid target.
- Checks that the redirection has a next token
- Ensures the next token isn't another operator
- Reports appropriate syntax errors
Returns:
- 1 if the redirection has a valid target
- 0 otherwise (with error message shown)
*/
int	validate_single_redir(t_node *redir_node, t_vars *vars)
{
	t_node	*next;

	next = redir_node->next;
	if (!next)
	{
		tok_syntax_error_msg("newline", vars);
		return (0);
	}
	else if (is_redirection(next->type) || next->type == TYPE_PIPE)
	{
		if (next->args[0])
			tok_syntax_error_msg(next->args[0], vars);
		else
			tok_syntax_error_msg("operator", vars);
		return (0);
	}
	return (1);
}

/*
Finds if a node is being used as a redirection target.
Returns the redirection node pointing to this target, or NULL.
*/
// t_node *find_redir_for_target(t_node *target_node, t_vars *vars)
// {
//     t_node *current = vars->head;
    
//     if (!target_node || !vars || !vars->head)
//         return (NULL);
        
//     fprintf(stderr, "[DEBUG-REDIR-TARGET] Checking if node is a redirection target\n");
    
//     while (current) {
//         if (is_redirection(current->type) && current->right == target_node) {
//             fprintf(stderr, "[DEBUG-REDIR-TARGET] Found node is target for %s redirection\n", 
//                    get_token_str(current->type));
//             return current;
//         }
//         current = current->next;
//     }
    
//     fprintf(stderr, "[DEBUG-REDIR-TARGET] Node is not a redirection target\n");
//     return (NULL);
// }
// t_node *find_redir_for_target(t_node *target_node, t_vars *vars)
// {
//     t_node *current;
    
//     // Fast return using cached result if possible
//     if (vars->pipes->in_redir_context && vars->pipes->last_redir_target == target_node)
//     {
//         return vars->pipes->current_redirect;
//     }
    
//     if (!target_node || !vars || !vars->head)
//     {
//         return (NULL);
//     }
        
//     fprintf(stderr, "[DEBUG-REDIR-TARGET] Checking if node is a redirection target\n");
    
//     current = vars->head;
//     while (current) 
//     {
//         if (is_redirection(current->type) && current->right == target_node) 
//         {
//             fprintf(stderr, "[DEBUG-REDIR-TARGET] Found node is target for %s redirection\n", 
//                    get_token_str(current->type));
            
//             // Cache the result for future checks
//             vars->pipes->in_redir_context = 1;
//             vars->pipes->last_redir_target = target_node;
//             vars->pipes->current_redirect = current;
            
//             return current;
//         }
//         current = current->next;
//     }
    
//     // Reset context if this node isn't a redirection target
//     if (target_node == vars->pipes->last_redir_target)
//     {
//         vars->pipes->in_redir_context = 0;
//     }
        
//     fprintf(stderr, "[DEBUG-REDIR-TARGET] Node is not a redirection target\n");
//     return (NULL);
// }
t_node *find_redir_for_target(t_node *target_node, t_vars *vars)
{
    t_node *current;
    
    // Fast return using cached result if possible
    if (vars->pipes->in_redir_context && vars->pipes->last_redir_target == target_node)
    {
        fprintf(stderr, "[DEBUG-REDIR-TARGET] Using cached result: %p\n", 
               (void*)vars->pipes->current_redirect);
        return vars->pipes->current_redirect;
    }
    
    if (!target_node || !vars || !vars->head)
        return (NULL);
        
    fprintf(stderr, "[DEBUG-REDIR-TARGET] Checking if node is a redirection target\n");
    
    // Debug print before starting search
    fprintf(stderr, "[DEBUG-REDIR-DETAILED] Searching for target %p in redirection relationships\n", 
           (void*)target_node);
    
    current = vars->head;
    while (current) 
    {
        if (is_redirection(current->type) && current->right == target_node) 
        {
            fprintf(stderr, "[DEBUG-REDIR-TARGET] Found node is target for %s redirection\n", 
                   get_token_str(current->type));
            
            // Debug print after finding a match
            fprintf(stderr, "[DEBUG-REDIR-DETAILED] Found node %p is target for node %p (type %s)\n", 
                   (void*)target_node, (void*)current, get_token_str(current->type));
            
            // Cache the result for future checks
            vars->pipes->in_redir_context = 1;
            vars->pipes->last_redir_target = target_node;
            vars->pipes->current_redirect = current;
            
            return current;
        }
        current = current->next;
    }
    
    // Reset context if this node isn't a redirection target
    if (target_node == vars->pipes->last_redir_target)
        vars->pipes->in_redir_context = 0;
        
    fprintf(stderr, "[DEBUG-REDIR-TARGET] Node is not a redirection target\n");
    return (NULL);
}

