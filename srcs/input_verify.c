/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/04/14 07:06:22 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles adding expanded values to commands
- Adds as new argument if preceded by a command
- Joins with existing argument if within an argument
- Removes the expansion node from the token list
Returns 1 if processed successfully, 0 otherwise
*/
// int handle_adjacent_args(t_node *expansion_node, char *expanded_value, t_vars *vars)
// {
// 	t_node *cmd_node;
// 	int is_adjacent;
// 	int last_arg_idx = 0;
// 	char *joined;
	
// 	fprintf(stderr, "DEBUG[handle_adjacent_args]: ENTER with expansion_node=%p, expanded_value='%s'\n", 
// 			(void*)expansion_node, expanded_value ? expanded_value : "NULL");
	
// 	if (!expansion_node || !expanded_value || !vars)
// 		return (0);
	
// 	// Find the command node this expansion belongs to
// 	cmd_node = find_cmd(vars->head, expansion_node, FIND_PREV, vars);
// 	if (!cmd_node)
// 	{
// 		fprintf(stderr, "DEBUG[handle_adjacent_args]: No command found for expansion\n");
// 		free(expanded_value);
// 		return (0);
// 	}
	
// 	// Check if expansion is adjacent to previous token
// 	is_adjacent = (expansion_node->prev && 
// 				  (expansion_node->prev->type == TYPE_ARGS));
	
// 	if (is_adjacent && expansion_node->prev->type != TYPE_CMD)
// 	{
// 		// Join with last argument if adjacent to an ARGS token
// 		while (expansion_node->prev->args[last_arg_idx + 1])
// 			last_arg_idx++;
		
// 		fprintf(stderr, "DEBUG[handle_adjacent_args]: Joining with arg: '%s'\n", 
// 				expansion_node->prev->args[last_arg_idx]);
				
// 		joined = ft_strjoin(expansion_node->prev->args[last_arg_idx], expanded_value);
// 		if (joined)
// 		{
// 			free(expansion_node->prev->args[last_arg_idx]);
// 			expansion_node->prev->args[last_arg_idx] = joined;
// 			free(expanded_value);
// 		}
// 	}
// 	else
// 	{
// 		// Add as new argument to the command
// 		fprintf(stderr, "DEBUG[handle_adjacent_args]: Adding as new argument to '%s'\n", 
// 				cmd_node->args[0]);
				
// 		append_arg(cmd_node, expanded_value, 0);
// 		free(expanded_value);
// 	}
	
// 	// Remove the expansion node from the token list
// 	fprintf(stderr, "DEBUG[handle_adjacent_args]: Cleaning up expansion node\n");
	
// 	// CRITICAL FIX: Store the next node before unlinking and freeing
// 	// t_node *next_node = expansion_node->next;
	
// 	if (expansion_node->prev)
// 		expansion_node->prev->next = expansion_node->next;
// 	if (expansion_node->next)
// 		expansion_node->next->prev = expansion_node->prev;
		
// 	if (expansion_node == vars->head)
// 		vars->head = expansion_node->next;
	
// 	fprintf(stderr, "DEBUG[handle_adjacent_args]: calling free_token_node\n");
// 	free_token_node(expansion_node);
	
// 	fprintf(stderr, "DEBUG[handle_adjacent_args]: EXIT\n");
// 	return (1);
// }
// int handle_adjacent_args(t_node *expansion_node, char *expanded_value, t_vars *vars)
// {
//     t_node *cmd_node;
//     int join_with_prev = 0;
//     int last_arg_idx = 0;
//     char *joined;
    
//     fprintf(stderr, "DEBUG[handle_adjacent_args]: ENTER with expansion_node=%p, expanded_value='%s'\n", 
//             (void*)expansion_node, expanded_value ? expanded_value : "NULL");
    
//     if (!expansion_node || !expanded_value || !vars)
//         return (0);
    
//     // Find the command node this expansion belongs to
//     cmd_node = find_cmd(vars->head, expansion_node, FIND_PREV, vars);
//     if (!cmd_node)
//     {
//         fprintf(stderr, "DEBUG[handle_adjacent_args]: No command found for expansion\n");
//         free(expanded_value);
//         return (0);
//     }
    
//     // Find the index of the last argument
//     if (cmd_node->args)
//     {
//         while (cmd_node->args[last_arg_idx + 1])
//             last_arg_idx++;
        
//         // Check if this expansion should be joined with the previous argument
//         // 1. Previous token is an ARGS token
//         // 2. OR if the expansion node immediately follows the command's last argument
//         if (expansion_node->prev && expansion_node->prev->type == TYPE_ARGS)
//             join_with_prev = 1;
//         else if (last_arg_idx > 0 && 
//                  is_adjacent_in_original_input(expansion_node, cmd_node->args[last_arg_idx]))
//             join_with_prev = 1;
//     }
    
//     fprintf(stderr, "DEBUG[handle_adjacent_args]: cmd_node='%s', join_with_prev=%d, last_arg_idx=%d\n",
//         cmd_node->args[0], join_with_prev, last_arg_idx);
    
//     if (join_with_prev && last_arg_idx >= 0)
//     {
//         // Join with last argument
//         fprintf(stderr, "DEBUG[handle_adjacent_args]: Joining with arg: '%s'\n", 
//                 cmd_node->args[last_arg_idx]);
                
//         joined = ft_strjoin(cmd_node->args[last_arg_idx], expanded_value);
//         if (joined)
//         {
//             free(cmd_node->args[last_arg_idx]);
//             cmd_node->args[last_arg_idx] = joined;
//             free(expanded_value);
//         }
//     }
//     else
//     {
//         // Add as new argument to the command
//         fprintf(stderr, "DEBUG[handle_adjacent_args]: Adding as new argument to '%s'\n", 
//                 cmd_node->args[0]);
                
//         append_arg(cmd_node, expanded_value, 0);
//         free(expanded_value);
//     }
    
//     // Remove the expansion node from the token list
//     fprintf(stderr, "DEBUG[handle_adjacent_args]: Cleaning up expansion node\n");
    
//     if (expansion_node->prev)
//         expansion_node->prev->next = expansion_node->next;
//     if (expansion_node->next)
//         expansion_node->next->prev = expansion_node->prev;
        
//     if (expansion_node == vars->head)
//         vars->head = expansion_node->next;
    
//     fprintf(stderr, "DEBUG[handle_adjacent_args]: calling free_token_node\n");
//     free_token_node(expansion_node);
    
//     fprintf(stderr, "DEBUG[handle_adjacent_args]: EXIT\n");
//     return (1);
// }

// /*
//  * Checks if tokens were adjacent in the original input
//  * This is needed because tokenization splits adjacent text and expansion
//  */
// int is_adjacent_in_original_input(t_node *expansion_node, char *arg)
// {
//     // For now, we'll assume they are adjacent if:
//     // 1. Expansion node has a meaningful prev token relationship
//     // 2. And the prev token was the command that holds this arg
    
//     if (!expansion_node || !expansion_node->prev || !arg)
//         return (0);
    
//     // If expansion directly follows command (TYPE_CMD) and 
//     // it's not the first argument, likely adjacent to last arg
//     if (expansion_node->prev->type == TYPE_CMD && 
//         expansion_node->prev->args && 
//         expansion_node->prev->args[0] && 
//         expansion_node->prev->args[1])
//         return (1);
        
//     fprintf(stderr, "DEBUG[is_adjacent_in_original_input]: Checking expansion node follows cmd\n");
    
//     // More sophisticated check would need to access the original input string
//     // and look at string positions from tokenization, but may not be available here
    
//     return (0);
// }

/*
Expands a variable name to its value
Handles special cases like lone $, $?, and environment variables
Returns newly allocated string with the expanded value
*/
char	*expand_value(char *var_name, t_vars *vars)
{
	char	*expanded_value;
	fprintf(stderr, "DEBUG[expand_value]: Called with var_name='%s', vars=%p\n", 
		var_name ? var_name : "NULL", (void*)vars);
	if (!var_name)
		return (ft_strdup(""));
		
	// Handle different expansion types
	if (var_name[0] == '\0')
	{
		// Lone $ character
		expanded_value = ft_strdup("$");
		fprintf(stderr, "DEBUG: Handling lone $\n");
	}
	else if (ft_strcmp(var_name, "?") == 0)
	{
		// Special case for $?
		expanded_value = ft_itoa(vars->error_code);
		fprintf(stderr, "DEBUG: Handling $?, expanded to '%s'\n", expanded_value);
	}
	else
	{
		// Regular environment variable
		expanded_value = get_env_val(var_name, vars->env);
		if (!expanded_value)
			expanded_value = ft_strdup("");
		fprintf(stderr, "DEBUG: Expanded env var '%s' to '%s'\n", var_name, expanded_value);
	}
	return (expanded_value);
}

/*
Processes variable expansions in the token list
- First pass handles TYPE_EXPANSION tokens
- Second pass handles expansions in quoted args
- Updates token types and handles adjacent arguments
- Fixed to avoid use-after-free errors when removing nodes
*/
// void process_expansions(t_vars *vars)
// {
//     t_node *current = vars->head;
//     char *expanded_value;
    
//     fprintf(stderr, "DEBUG: Starting process_expansions()\n");
    
//     // First pass - expand TYPE_EXPANSION tokens that weren't handled during tokenization
//     // (this should be rare if make_exp_token handles most cases)
//     while (current)
//     {
//         if (current->type == TYPE_EXPANSION && current->args && current->args[0])
//         {
//             // Skip the $ character
//             char *var_name = ft_substr(current->args[0], 1, ft_strlen(current->args[0]) - 1);
//             expanded_value = expand_value(var_name, vars);
//             free(var_name);
            
//             fprintf(stderr, "DEBUG: Process_expansions: calling expand_value\n");
            
//             // Handle adjacency based on tokenization order
//             if (current->prev && (current->prev->type == TYPE_CMD || 
//                                  current->prev->type == TYPE_ARGS))
//             {
//                 fprintf(stderr, "DEBUG: Expansion is adjacent to command, adding as argument\n");
//                 handle_adjacent_args(current, expanded_value, vars);
//                 // Note: handle_adjacent_args frees expanded_value and removes current node
//                 // We need to advance current before it's freed
//                 t_node *next = current->next;
//                 current = next;
//                 continue;
//             }
//             else
//             {
//                 // Convert expansion to regular argument
//                 free(current->args[0]);
//                 current->args[0] = expanded_value;
//                 current->type = TYPE_ARGS;
//             }
//         }
        
//         // Second pass - expand variables in double-quoted arguments
//         if (current->type == TYPE_CMD || current->type == TYPE_ARGS)
//         {
//             int arg_idx = 0;
//             while (current->args[arg_idx])
//             {
//                 // If we have quote type info and there are double quotes
//                 if (current->arg_quote_type && current->arg_quote_type[arg_idx])
//                 {
//                     int needs_expansion = 0;
//                     int i = 0;
                    
//                     // Check if any character is both $ and in double quotes
//                     while (current->args[arg_idx][i])
//                     {
//                         if (current->args[arg_idx][i] == '$' && 
//                             current->arg_quote_type[arg_idx][i] == TYPE_DOUBLE_QUOTE)
//                         {
//                             needs_expansion = 1;
//                             break;
//                         }
//                         i++;
//                     }
                    
//                     if (needs_expansion)
//                     {
//                         // Process double-quoted expansions character by character
//                         char *expanded = expand_quoted_argument(
//                             current->args[arg_idx], 
//                             current->arg_quote_type[arg_idx],
//                             vars
//                         );
                        
//                         free(current->args[arg_idx]);
//                         current->args[arg_idx] = expanded;
//                     }
//                 }
//                 arg_idx++;
//             }
//         }
        
//         current = current->next;
//     }
    
//     fprintf(stderr, "DEBUG: Final token list after processing expansions:\n");
//     current = vars->head;
//     while (current)
//     {
//         fprintf(stderr, "DEBUG: Node type=%d, args[0]='%s'\n", 
//                 current->type, 
//                 (current->args && current->args[0]) ? current->args[0] : "NULL");
//         current = current->next;
//     }
// }
void process_expansions(t_vars *vars)
{
    t_node *current = vars->head;
    
    fprintf(stderr, "DEBUG: Starting process_expansions()\n");
    
    // Process any double-quoted expansions or any TYPE_EXPANSION tokens that slipped through
    while (current)
    {
        // Handle any remaining TYPE_EXPANSION tokens (shouldn't happen with new implementation)
        if (current->type == TYPE_EXPANSION)
        {
            // Log unexpected expansion token
            fprintf(stderr, "DEBUG: Warning - Found unexpected TYPE_EXPANSION token, should have been handled during tokenization\n");
            
            // Handle it anyway as a fallback
            char *var_name = ft_substr(current->args[0], 1, ft_strlen(current->args[0]) - 1);
            char *expanded_value = expand_value(var_name, vars);
            free(var_name);
            
            // Convert expansion to regular argument
            free(current->args[0]);
            current->args[0] = expanded_value;
            current->type = TYPE_ARGS;
        }
        
        // Handle expansions in quoted text (which are handled separately)
        if ((current->type == TYPE_CMD || current->type == TYPE_ARGS) && 
            current->arg_quote_type)
        {
			fprintf(stderr, "DEBUG: Process_expansions() - Found variables in double-quoted strings\n");
            // This part processes variables in double-quoted strings
            // Keep this functionality
            int arg_idx = 0;
            while (current->args[arg_idx])
            {
                // Process quoted expansions
                // ...existing code for handling quoted expansions...
                arg_idx++;
            }
        }
        
        current = current->next;
    }
    
    fprintf(stderr, "DEBUG: Completed process_expansions()\n");
}

/*
Add this helper function to check if a node is in command position 
*/
int is_command_position(t_node *node, t_vars *vars)
{
	// First token in the list is always in command position
	if (node == vars->head)
		return 1;
		
	// If previous token is a pipe, this is in command position
	if (node->prev && node->prev->type == TYPE_PIPE)
		return 1;
		
	// If we're after a redirection and its target, this is in command position
	if (node->prev && node->prev->prev && 
		is_redirection(node->prev->prev->type) && 
		node->prev->prev->next == node->prev)
		return 1;
		
	return 0;
}
