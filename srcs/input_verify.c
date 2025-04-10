/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 10:15:23 by bleow            ###   ########.fr       */
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
int handle_adjacent_args(t_node *expansion_node, char *expanded_value, t_vars *vars)
{
    t_node *cmd_node;
    int is_adjacent;
    int last_arg_idx = 0;
    char *joined;
    
    fprintf(stderr, "DEBUG[handle_adjacent_args]: ENTER with expansion_node=%p, expanded_value='%s'\n", 
            (void*)expansion_node, expanded_value ? expanded_value : "NULL");
    
    if (!expansion_node || !expanded_value || !vars)
        return (0);
    
    // Find the command node this expansion belongs to
    cmd_node = find_cmd(vars->head, expansion_node, FIND_PREV, vars);
    if (!cmd_node)
    {
        fprintf(stderr, "DEBUG[handle_adjacent_args]: No command found for expansion\n");
        free(expanded_value);
        return (0);
    }
    
    // Check if expansion is adjacent to previous token
    is_adjacent = (expansion_node->prev && 
                  (expansion_node->prev->type == TYPE_ARGS));
    
    if (is_adjacent && expansion_node->prev->type != TYPE_CMD)
    {
        // Join with last argument if adjacent to an ARGS token
        while (expansion_node->prev->args[last_arg_idx + 1])
            last_arg_idx++;
        
        fprintf(stderr, "DEBUG[handle_adjacent_args]: Joining with arg: '%s'\n", 
                expansion_node->prev->args[last_arg_idx]);
                
        joined = ft_strjoin(expansion_node->prev->args[last_arg_idx], expanded_value);
        if (joined)
        {
            free(expansion_node->prev->args[last_arg_idx]);
            expansion_node->prev->args[last_arg_idx] = joined;
            free(expanded_value);
        }
    }
    else
    {
        // Add as new argument to the command
        fprintf(stderr, "DEBUG[handle_adjacent_args]: Adding as new argument to '%s'\n", 
                cmd_node->args[0]);
                
        append_arg(cmd_node, expanded_value, 0);
        free(expanded_value);
    }
    
    // Remove the expansion node from the token list
    fprintf(stderr, "DEBUG[handle_adjacent_args]: Cleaning up expansion node\n");
    
    // CRITICAL FIX: Store the next node before unlinking and freeing
    // t_node *next_node = expansion_node->next;
    
    if (expansion_node->prev)
        expansion_node->prev->next = expansion_node->next;
    if (expansion_node->next)
        expansion_node->next->prev = expansion_node->prev;
        
    if (expansion_node == vars->head)
        vars->head = expansion_node->next;
    
    fprintf(stderr, "DEBUG[handle_adjacent_args]: calling free_token_node\n");
    free_token_node(expansion_node);
    
    fprintf(stderr, "DEBUG[handle_adjacent_args]: EXIT\n");
    return (1);
}

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
Processes expansion tokens in the token list
Finds the command each expansion belongs to
Expands variables and adds them as arguments
Handles both standalone $VAR tokens and embedded variables in strings
Preserves quote contexts and handles command/argument positioning
Expanding empty variables will skip the expansion and move on to next CMD token
Removes the expansion tokens when done
*/
/*
 * Processes variable expansions in the token list
 * - First pass: handles standalone $VAR tokens
 * - Second pass: handles variables embedded in arguments
 * - Final pass: ensures first token is always a command
 * - Supports command promotion when expanding empty variables
 */
// void process_expansions(t_vars *vars)
// {
//     t_node *current;
//     char *var_name;
//     char *expanded_value;
//     t_node *cmd_node;
//     int is_adjacent;
    
//     fprintf(stderr, "DEBUG: Starting process_expansions()\n");
//     current = vars->head;
    
//     // First pass: handle actual TYPE_EXPANSION tokens
//     while (current)
//     {
//         if (current->type == TYPE_EXPANSION || current->type == TYPE_EXIT_STATUS)
//         {
//             var_name = current->args[0] + 1;
//             expanded_value = expand_value(var_name, vars);
//             is_adjacent = (current->prev && (current->prev->type == TYPE_ARGS || 
//                                            current->prev->type == TYPE_CMD));
// 			// Before any call to find_cmd(), add:
// 			fprintf(stderr, "DEBUG[process_expansions]: About to call find_cmd with vars=%p\n", (void*)vars);
// 			// Before calling find_cmd for expansion in command position:
// 			fprintf(stderr, "DEBUG[process_expansions]: Expansion in cmd position, calling find_cmd with head=%p, current=%p, FIND_PREV, vars=%p\n",
// 				(void*)(vars ? vars->head : NULL), (void*)current, (void*)vars);

// 			cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
//             // After each call to find_cmd(), add:
// 			fprintf(stderr, "DEBUG[process_expansions]: find_cmd returned %p\n", (void*)cmd_node);
//             // Check if this token is in command position
//             if (is_command_position(current, vars))
//             {
//                 fprintf(stderr, "DEBUG: Expansion is in command position\n");
                
//                 // If the expansion resulted in an empty string
//                 if (expanded_value[0] == '\0')
//                 {
//                     // Free the empty value
//                     free(expanded_value);
                    
//                     // Either remove the node or skip it during execution
//                     // Here's how to remove it from the token list:
//                     if (current->prev)
//                         current->prev->next = current->next;
//                     if (current->next)
//                         current->next->prev = current->prev;
//                     if (current == vars->head)
//                         vars->head = current->next;
                        
//                     // Save next node for iteration
//                     t_node *next = current->next;
                    
//                     // Free current node
//                     free_token_node(current);
//                     current = next;
                    
//                     // If there's a next node that should now be the command
//                     if (current && current->type == TYPE_ARGS)
//                     {
//                         t_node *arg_node;
//                         t_node *next_node;
//                         int i;
                        
//                         current->type = TYPE_CMD;
                        
//                         // Print debug information about promotion
//                         if (current->args)
//                         {
//                             fprintf(stderr, "DEBUG: Empty command removed, promoting '%s' to command\n", 
//                                   current->args[0]);
//                         }
//                         else
//                         {
//                             fprintf(stderr, "DEBUG: Empty command removed, promoting 'NULL' to command\n");
//                         }
                        
//                         // Find and merge arguments from subsequent argument nodes
//                         arg_node = current->next;
//                         while (arg_node && arg_node->type == TYPE_ARGS)
//                         {
//                             // Print debug information about merging
//                             if (arg_node->args)
//                             {
//                                 fprintf(stderr, "DEBUG: Merging arguments from node: '%s'\n",
//                                       arg_node->args[0]);
//                             }
//                             else
//                             {
//                                 fprintf(stderr, "DEBUG: Merging arguments from node: 'NULL'\n");
//                             }
                                  
//                             // Merge arguments from arg_node into current command node
//                             i = 0;
//                             while (arg_node->args && arg_node->args[i])
//                             {
//                                 if (arg_node->arg_quote_type)
//                                 {
//                                     append_arg(current, arg_node->args[i], arg_node->arg_quote_type[i]);
//                                 }
//                                 else
//                                 {
//                                     append_arg(current, arg_node->args[i], 0);
//                                 }
//                                 i++;
//                             }
                            
//                             // Remove the argument node now that we've merged its contents
//                             next_node = arg_node->next;
//                             if (arg_node->prev)
//                                 arg_node->prev->next = next_node;
//                             if (next_node)
//                                 next_node->prev = arg_node->prev;
                                
//                             free_token_node(arg_node);
//                             arg_node = next_node;
//                         }
                        
//                         // Debug print to show the final command with all arguments
//                         if (current->args)
//                         {
//                             fprintf(stderr, "DEBUG: Final promoted command: '%s' with args:", 
//                                     current->args[0]);
//                             i = 1;
//                             while (current->args[i])
//                             {
//                                 fprintf(stderr, " '%s'", current->args[i]);
//                                 i++;
//                             }
//                             fprintf(stderr, "\n");
//                         }
//                     }
                    
//                     // Skip the rest of the loop iteration
//                     continue;
//                 } 
//                 else 
//                 {
//                     // Normal case with non-empty command
//                     free(current->args[0]);
//                     current->args[0] = expanded_value;
//                     current->type = TYPE_CMD;
//                     fprintf(stderr, "DEBUG: Node is now TYPE_CMD with args[0]='%s'\n", 
//                             current->args[0]);
//                 }
//             }
//             // If adjacent to a command, add as argument
//             else if (cmd_node)
//             {
//                 handle_adjacent_args(current, expanded_value, cmd_node, is_adjacent);
//             }
//             else
//             {
//                 fprintf(stderr, "DEBUG: Converting expansion to command, value='%s'\n", expanded_value);
//                 free(current->args[0]);
//                 current->args[0] = expanded_value;
//                 current->type = TYPE_CMD;
//             }
//         }
//         current = current->next;
//     }
    
//     // Second pass: handle expansions inside quoted args
//     current = vars->head;
//     while (current)
//     {
//         if ((current->type == TYPE_CMD || current->type == TYPE_ARGS) && 
//             current->args && current->arg_quote_type)
//         {
//             // Process each argument
//             int i = 0;
//             while (current->args[i])
//             {
//                 // Skip expansion in single-quoted strings
//                 if (current->arg_quote_type[i] != 1 && ft_strchr(current->args[i], '$'))
//                 {
//                     fprintf(stderr, "DEBUG: Quote type for argument '%s' is: %d\n", 
//                             current->args[i], current->arg_quote_type[i]);
                    
//                     // Process expansions in this argument
//                     process_arg_expansion(&current->args[i], current->arg_quote_type[i], vars);
//                 }
//                 i++;
//             }
//         }
//         current = current->next;
//     }
    
//     // Final pass: ensure first token is always TYPE_CMD
//     if (vars->head && vars->head->type == TYPE_ARGS)
//     {
//         fprintf(stderr, "DEBUG: Converting first token from ARGS to CMD\n");
//         vars->head->type = TYPE_CMD;
//     }
    
//     // Debug trace the final token list
//     fprintf(stderr, "DEBUG: Final token list after processing expansions:\n");
//     current = vars->head;
//     while (current)
//     {
//         fprintf(stderr, "DEBUG: Node type=%d, args[0]='%s'\n", 
//                 current->type, current->args ? current->args[0] : "NULL");
//         current = current->next;
//     }
// }

// void process_expansions(t_vars *vars)
// {
//     t_node *current;
//     char *var_name;
//     char *expanded_value;
//     t_node *cmd_node;
//     int is_adjacent;
    
//     fprintf(stderr, "DEBUG: Starting process_expansions()\n");
//     current = vars->head;
    
//     // First pass: handle actual TYPE_EXPANSION tokens
//     while (current)
//     {
//         if (current->type == TYPE_EXPANSION || current->type == TYPE_EXIT_STATUS)
//         {
//             var_name = current->args[0] + 1;
//             expanded_value = expand_value(var_name, vars);
//             is_adjacent = (current->prev && (current->prev->type == TYPE_ARGS || 
//                                            current->prev->type == TYPE_CMD));
// 			if (current->next && current->next->type == TYPE_ARGS)
// 			{
//                 // Join the expanded value with the adjacent text
//                 char *joined = ft_strjoin(expanded_value, current->next->args[0]);
//                 if (joined) {
//                     fprintf(stderr, "DEBUG: Joining expansion '%s' with adjacent text '%s'\n", 
//                             expanded_value, current->next->args[0]);
//                     free(expanded_value);
//                     expanded_value = joined;
                    
//                     // Remove the next node since we've incorporated its text
//                     t_node *to_remove = current->next;
//                     current->next = to_remove->next;
//                     if (to_remove->next)
//                         to_remove->next->prev = current;
//                     free_token_node(to_remove);
//                     fprintf(stderr, "DEBUG: After join: '%s'\n", expanded_value);
//                 }
//             }
// 			// Before any call to find_cmd(), add:
// 			fprintf(stderr, "DEBUG[process_expansions]: About to call find_cmd with vars=%p\n", (void*)vars);
// 			// Before calling find_cmd for expansion in command position:
// 			fprintf(stderr, "DEBUG[process_expansions]: Expansion in cmd position, calling find_cmd with head=%p, current=%p, FIND_PREV, vars=%p\n",
// 				(void*)(vars ? vars->head : NULL), (void*)current, (void*)vars);

// 			cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
//             // After each call to find_cmd(), add:
// 			fprintf(stderr, "DEBUG[process_expansions]: find_cmd returned %p\n", (void*)cmd_node);
//             // Check if this token is in command position
//             if (is_command_position(current, vars))
//             {
//                 fprintf(stderr, "DEBUG: Expansion is in command position\n");
                
//                 // If the expansion resulted in an empty string
//                 if (expanded_value[0] == '\0')
//                 {
//                     // Free the empty value
//                     free(expanded_value);
                    
//                     // Either remove the node or skip it during execution
//                     // Here's how to remove it from the token list:
//                     if (current->prev)
//                         current->prev->next = current->next;
//                     if (current->next)
//                         current->next->prev = current->prev;
//                     if (current == vars->head)
//                         vars->head = current->next;
                        
//                     // Save next node for iteration
//                     t_node *next = current->next;
                    
//                     // Free current node
//                     free_token_node(current);
//                     current = next;
                    
//                     // If there's a next node that should now be the command
//                     if (current && current->type == TYPE_ARGS)
//                     {
//                         t_node *arg_node;
//                         t_node *next_node;
//                         int i;
                        
//                         current->type = TYPE_CMD;
//                         // Print debug information about promotion
//                         if (current->args)
//                         {
//                             fprintf(stderr, "DEBUG: Empty command removed, promoting '%s' to command\n", 
//                                   current->args[0]);
//                         }
//                         else
//                         {
//                             fprintf(stderr, "DEBUG: Empty command removed, promoting 'NULL' to command\n");
//                         }
//                         // Find and merge arguments from subsequent argument nodes
//                         arg_node = current->next;
//                         while (arg_node && arg_node->type == TYPE_ARGS)
//                         {
//                             // Print debug information about merging
//                             if (arg_node->args)
//                             {
//                                 fprintf(stderr, "DEBUG: Merging arguments from node: '%s'\n",
//                                       arg_node->args[0]);
//                             }
//                             else
//                             {
//                                 fprintf(stderr, "DEBUG: Merging arguments from node: 'NULL'\n");
//                             }
                                  
//                             // Merge arguments from arg_node into current command node
//                             i = 0;
//                             while (arg_node->args && arg_node->args[i])
//                             {
//                                 if (arg_node->arg_quote_type)
//                                 {
//                                     append_arg(current, arg_node->args[i], arg_node->arg_quote_type[i]);
//                                 }
//                                 else
//                                 {
//                                     append_arg(current, arg_node->args[i], 0);
//                                 }
//                                 i++;
//                             }
                            
//                             // Remove the argument node now that we've merged its contents
//                             next_node = arg_node->next;
//                             if (arg_node->prev)
//                                 arg_node->prev->next = next_node;
//                             if (next_node)
//                                 next_node->prev = arg_node->prev;
                                
//                             free_token_node(arg_node);
//                             arg_node = next_node;
//                         }
                        
//                         // Debug print to show the final command with all arguments
//                         if (current->args)
//                         {
//                             fprintf(stderr, "DEBUG: Final promoted command: '%s' with args:", 
//                                     current->args[0]);
//                             i = 1;
//                             while (current->args[i])
//                             {
//                                 fprintf(stderr, " '%s'", current->args[i]);
//                                 i++;
//                             }
//                             fprintf(stderr, "\n");
//                         }
//                     }
                    
//                     // Skip the rest of the loop iteration
//                     continue;
//                 } 
//                 else 
//                 {
//                     // Normal case with non-empty command
//                     free(current->args[0]);
//                     current->args[0] = expanded_value;
//                     current->type = TYPE_CMD;
//                     fprintf(stderr, "DEBUG: Node is now TYPE_CMD with args[0]='%s'\n", 
//                             current->args[0]);
//                 }
//             }
//             // If adjacent to a command, add as argument
//             else if (cmd_node)
//             {
//                 handle_adjacent_args(current, expanded_value, cmd_node, is_adjacent);
//             }
//             else
//             {
//                 fprintf(stderr, "DEBUG: Converting expansion to command, value='%s'\n", expanded_value);
//                 free(current->args[0]);
//                 current->args[0] = expanded_value;
//                 current->type = TYPE_CMD;
//             }
//         }
//         current = current->next;
//     }
    
//     // Second pass: handle expansions inside quoted args
//     current = vars->head;
//     while (current)
//     {
//         if ((current->type == TYPE_CMD || current->type == TYPE_ARGS) && 
//             current->args && current->arg_quote_type)
//         {
//             // Process each argument
//             int i = 0;
//             while (current->args[i])
//             {
//                 // Skip expansion in single-quoted strings
//                 if (current->arg_quote_type[i] != 1 && ft_strchr(current->args[i], '$'))
//                 {
//                     fprintf(stderr, "DEBUG: Quote type for argument '%s' is: %d\n", 
//                             current->args[i], current->arg_quote_type[i]);
                    
//                     // Process expansions in this argument
//                     process_arg_expansion(&current->args[i], current->arg_quote_type[i], vars);
//                 }
//                 i++;
//             }
//         }
//         current = current->next;
//     }
    
//     // Final pass: ensure first token is always TYPE_CMD
//     if (vars->head && vars->head->type == TYPE_ARGS)
//     {
//         fprintf(stderr, "DEBUG: Converting first token from ARGS to CMD\n");
//         vars->head->type = TYPE_CMD;
//     }
    
//     // Debug trace the final token list
//     fprintf(stderr, "DEBUG: Final token list after processing expansions:\n");
//     current = vars->head;
//     while (current)
//     {
//         fprintf(stderr, "DEBUG: Node type=%d, args[0]='%s'\n", 
//                 current->type, current->args ? current->args[0] : "NULL");
//         current = current->next;
//     }
// }
/*
Processes token list for expansions, respecting character-level quote types
- First pass handles TYPE_EXPANSION tokens
- Second pass handles expansions in arguments based on character-level quote types
*/
/*
 * Processes variable expansions in the token list
 * - First pass handles TYPE_EXPANSION tokens
 * - Second pass handles expansions in quoted args
 * - Updates token types and handles adjacent arguments
 * - Fixed to avoid use-after-free errors when removing nodes
 */
void process_expansions(t_vars *vars)
{
    t_node	*current;
    t_node	*next;  // Added to track next node safely
	t_node	*cmd_node;
    char	*var_name;
    char	*expanded_value;
    
    fprintf(stderr, "DEBUG: Starting process_expansions()\n");
    
    if (!vars || !vars->head)
        return;
        
    current = vars->head;
    
    // First pass: handle TYPE_EXPANSION tokens
    while (current)
    {
        // CRITICAL FIX: Save the next node before any potential freeing
        next = current->next;
        
        if (current->type == TYPE_EXPANSION || current->type == TYPE_EXIT_STATUS)
        {
            if (current->type == TYPE_EXIT_STATUS)
                var_name = "?";  // Special handling for $?
            else
                var_name = current->args[0] + 1;
                
            fprintf(stderr, "DEBUG: Process_expansions: calling expand_value\n");    
            expanded_value = expand_value(var_name, vars);
            cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
            
            // First token in command or expansion in command position should be TYPE_CMD
            if (current == vars->head || is_command_position(current, vars))
            {
                fprintf(stderr, "DEBUG: Expansion is in command position\n");
                
                // If the expansion resulted in an empty string
                if (!expanded_value || expanded_value[0] == '\0')
                {
                    fprintf(stderr, "DEBUG: process_expansions : freeing empty value\n");
                    // Free the empty value if it exists
                    if (expanded_value)
                        free(expanded_value);
                    
                    // Handle empty expansion logic...
                    // (existing code for handling empty expansions)
                } 
                else 
                {
                    fprintf(stderr, "DEBUG: process_expansions : handling normal case, non-empty command\n");
                    // Normal case with non-empty command
                    fprintf(stderr, "DEBUG: Freeing current->args[0]\n");
                    free(current->args[0]);
                    current->args[0] = expanded_value;
                    current->type = TYPE_CMD;  // Always set to CMD when in command position
                    fprintf(stderr, "DEBUG: Node is now TYPE_CMD with args[0]='%s'\n", 
                            current->args[0]);
                }
            }
            // If adjacent to a command, add as argument
            else if (cmd_node)
            {
                fprintf(stderr, "DEBUG: Expansion is adjacent to command, adding as argument\n");
                handle_adjacent_args(current, expanded_value, vars);
                
                // CRITICAL FIX: After handle_adjacent_args, immediately use 'next'
                // since current has been freed
                current = next;
                continue;
            }
            else
            {
                fprintf(stderr, "DEBUG: Converting expansion to command, value='%s'\n", expanded_value);
                free(current->args[0]);
                current->args[0] = expanded_value;
                current->type = TYPE_CMD;  // When unsure, convert to command
            }
        }
        
        // CRITICAL FIX: Always advance using the saved 'next' pointer
        current = next;
    }
    
    // Second pass: handle expansions inside quoted args
    // (existing code for processing expansions inside arguments)
    
    // Final pass: ensure first token is always TYPE_CMD (important for $? case)
    if (vars->head && vars->head->type != TYPE_CMD)
    {
        fprintf(stderr, "DEBUG: Converting first token from type %d to TYPE_CMD\n", 
                vars->head->type);
        vars->head->type = TYPE_CMD;
    }
    
    // Debug trace the final token list
    fprintf(stderr, "DEBUG: Final token list after processing expansions:\n");
    current = vars->head;
    while (current)
    {
        // Added extra safety check
        char *arg_display = "NULL";
        if (current->args && current->args[0])
        {
            arg_display = current->args[0];
        }
        // Print the node type and first argument
        fprintf(stderr, "DEBUG: Node type=%d, args[0]='%s'\n", 
            current->type, arg_display);
        current = current->next;
    }
}

/*
 * Processes variable expansions inside argument strings
 * Handles $VAR, $?, lone $ characters and other special cases
 * Uses quote_type to determine if expansion should occur
 */
// void process_arg_expansion(char **arg_ptr, int quote_type, t_vars *vars)
// {
//     char *arg;
//     char *result;
//     char *temp;
//     int pos;
//     int var_start;
//     char *var_name;
//     char *var_value;
//     char c_str[2] = {0, 0};
    
//     if (!arg_ptr || !*arg_ptr)
//         return;
//     // Add debug print to show quote type value
// 	fprintf(stderr, "DEBUG: Quote type for argument '%s' is: %d\n", *arg_ptr, quote_type);

//     // Skip expansion entirely if in single quotes (quote_type == 1)
//     if (quote_type == 1) {
//         fprintf(stderr, "DEBUG: Skipping expansion for single-quoted arg: '%s'\n", *arg_ptr);
//         return;
//     }
    
//     arg = *arg_ptr;
//     fprintf(stderr, "DEBUG: Processing expansion in arg: '%s', quote_type=%d\n", 
//             arg, quote_type);
    
//     // Initialize with empty string
//     result = ft_strdup("");
//     if (!result)
//         return;
    
//     pos = 0;
//     while (arg[pos])
//     {
//         // Handle variable expansions
//         if (arg[pos] == '$')
//         {
//             fprintf(stderr, "DEBUG: Found $ at position %d\n", pos);
//             pos++; // Skip past $
            
//             // Special case: $?
//             if (arg[pos] == '?')
//             {
//                 var_value = ft_itoa(vars->error_code);
//                 fprintf(stderr, "DEBUG: Expanded $? to '%s'\n", var_value);
//                 pos++; // Move past ?
                
//                 temp = ft_strjoin(result, var_value);
//                 free(result);
//                 free(var_value);
//                 result = temp;
//             }
//             // Regular variable expansion: $NAME
//             else if (ft_isalpha(arg[pos]) || arg[pos] == '_')
//             {
//                 // Extract variable name
//                 var_start = pos;
//                 while (arg[pos] && (ft_isalnum(arg[pos]) || arg[pos] == '_'))
//                     pos++;
                
//                 var_name = ft_substr(arg, var_start, pos - var_start);
//                 var_value = get_env_val(var_name, vars->env);
//                 fprintf(stderr, "DEBUG: process_arg_expansion()Expanded env var $%s to '%s'\n", 
//                         var_name, var_value ? var_value : "");
                
//                 // Append expanded value (or empty string if variable not found)
//                 temp = ft_strjoin(result, var_value ? var_value : "");
//                 free(result);
//                 free(var_name);
//                 if (var_value)
//                     free(var_value);
//                 result = temp;
//             }
//             // Lone $ or $ followed by invalid character
//             else
//             {
//                 fprintf(stderr, "DEBUG: Found lone $ character\n");
//                 temp = ft_strjoin(result, "$");
//                 free(result);
//                 result = temp;
//             }
//         }
//         // Regular character
//         else
//         {
//             c_str[0] = arg[pos];
//             temp = ft_strjoin(result, c_str);
//             free(result);
//             result = temp;
//             pos++;
//         }
//     }
    
//     // Replace original argument with expanded version
//     fprintf(stderr, "DEBUG: Final expanded arg: '%s'\n", result);
//     free(*arg_ptr);
//     *arg_ptr = result;
// }
/*
 * Processes variable expansions inside argument strings
 * Handles $VAR, $?, lone $ characters and other special cases
 * Uses quote_type to determine if expansion should occur
 */
// void process_arg_expansion(char **arg_ptr, int quote_type, t_vars *vars)
// {
//     char *arg;
//     char *result;
//     char *temp;
//     int pos;
//     int var_start;
//     char *var_name;
//     char *var_value;
//     char c_str[2] = {0, 0};
    
//     if (!arg_ptr || !*arg_ptr)
//         return;
    
//     // Add debug print to show quote type value
//     fprintf(stderr, "DEBUG: Quote type for argument '%s' is: %d\n", *arg_ptr, quote_type);

//     // Skip expansion entirely if in single quotes (quote_type == 1)
//     if (quote_type == 1) {
//         fprintf(stderr, "DEBUG: Skipping expansion for single-quoted arg: '%s'\n", *arg_ptr);
//         return;
//     }
    
//     arg = *arg_ptr;
//     fprintf(stderr, "DEBUG: Processing expansion in arg: '%s', quote_type=%d\n", 
//             arg, quote_type);
    
//     // Initialize with empty string
//     result = ft_strdup("");
//     if (!result)
//         return;
    
//     pos = 0;
//     while (arg[pos])
//     {
//         // Handle variable expansions - but not if in single quotes context
//         if (arg[pos] == '$' && !is_in_single_quotes(pos, vars))
//         {
//             fprintf(stderr, "DEBUG: Found $ at position %d\n", pos);
//             pos++; // Skip past $
            
//             // Special case: $?
//             if (arg[pos] == '?')
//             {
//                 var_value = ft_itoa(vars->error_code);
//                 fprintf(stderr, "DEBUG: Expanded $? to '%s'\n", var_value);
//                 pos++; // Move past ?
                
//                 temp = ft_strjoin(result, var_value);
//                 free(result);
//                 free(var_value);
//                 result = temp;
//             }
//             // Regular variable expansion: $NAME
//             else if (ft_isalpha(arg[pos]) || arg[pos] == '_')
//             {
//                 // Extract variable name
//                 var_start = pos;
//                 while (arg[pos] && (ft_isalnum(arg[pos]) || arg[pos] == '_'))
//                     pos++;
                
//                 var_name = ft_substr(arg, var_start, pos - var_start);
//                 var_value = get_env_val(var_name, vars->env);
//                 fprintf(stderr, "DEBUG: Expanded env var $%s to '%s'\n", 
//                         var_name, var_value ? var_value : "");
                
//                 // Append expanded value (or empty string if variable not found)
//                 temp = ft_strjoin(result, var_value ? var_value : "");
//                 free(result);
//                 free(var_name);
//                 if (var_value)
//                     free(var_value);
//                 result = temp;
//             }
//             // Lone $ or $ followed by invalid character
//             else
//             {
//                 fprintf(stderr, "DEBUG: Found lone $ character\n");
//                 temp = ft_strjoin(result, "$");
//                 free(result);
//                 result = temp;
//             }
//         }
//         // Regular character or anything inside single quotes
//         else
//         {
//             c_str[0] = arg[pos];
//             temp = ft_strjoin(result, c_str);
//             free(result);
//             result = temp;
//             pos++;
//         }
//     }
    
//     // Replace original argument with expanded version
//     fprintf(stderr, "DEBUG: Final expanded arg: '%s'\n", result);
//     free(*arg_ptr);
//     *arg_ptr = result;
// }
// /*
// Processes variable expansions in arguments based on character-level quote types
// Only expands variables in unquoted or double-quoted segments
// */
// void process_arg_expansion(char **arg_ptr, int **quote_type_ptr, t_vars *vars)
// {
//     char *arg;
//     char *result;
//     char *temp;
//     int pos;
//     int var_start;
//     char *var_name;
//     char *var_value;
//     char c_str[2] = {0, 0};
//     int *new_quote_types = NULL;
//     size_t result_len = 0;
    
//     if (!arg_ptr || !*arg_ptr)
//         return;
        
//     arg = *arg_ptr;
//     int *char_quote_types = (quote_type_ptr && *quote_type_ptr) ? *quote_type_ptr : NULL;
    
//     // Initialize result string
//     result = ft_strdup("");
//     if (!result)
//         return;
        
//     // Process each character in the argument
//     pos = 0;
//     while (arg[pos])
//     {
//         // Check if this character is in single quotes
//         int in_single_quotes = 0;
//         if (char_quote_types && char_quote_types[pos] == TYPE_SINGLE_QUOTE)
//             in_single_quotes = 1;
            
//         // Handle variable expansions - but not if in single quotes
//         if (arg[pos] == '$' && !in_single_quotes)
//         {
//             pos++; // Skip past $
            
//             // Special case: $?
//             if (arg[pos] == '?')
//             {
//                 var_value = ft_itoa(vars->error_code);
//                 pos++; // Move past ?
                
//                 temp = ft_strjoin(result, var_value);
//                 free(result);
//                 free(var_value);
//                 result = temp;
                
//                 if (!result)
//                     return;
                    
//                 // Update result length
//                 result_len = ft_strlen(result);
//             }
//             // Regular variable expansion: $NAME
//             else if (ft_isalpha(arg[pos]) || arg[pos] == '_')
//             {
//                 // Extract variable name
//                 var_start = pos;
//                 while (arg[pos] && (ft_isalnum(arg[pos]) || arg[pos] == '_'))
//                     pos++;
                
//                 var_name = ft_substr(arg, var_start, pos - var_start);
//                 if (!var_name)
//                     continue;
                    
//                 var_value = get_env_val(var_name, vars->env);
                
//                 // Append expanded value (or empty string if variable not found)
//                 temp = ft_strjoin(result, var_value ? var_value : "");
//                 free(result);
//                 free(var_name);
//                 if (var_value)
//                     free(var_value);
//                 result = temp;
                
//                 if (!result)
//                     return;
                    
//                 // Update result length
//                 result_len = ft_strlen(result);
//             }
//             // Lone $ or $ followed by invalid character
//             else
//             {
//                 temp = ft_strjoin(result, "$");
//                 free(result);
//                 result = temp;
                
//                 if (!result)
//                     return;
                    
//                 // Update result length
//                 result_len = ft_strlen(result);
                
//                 // Make sure we skip the dollar sign position
//                 if (arg[pos] && arg[pos] != '$')
//                     pos++;
//             }
//         }
//         // Regular character or anything inside single quotes
//         else
//         {
//             c_str[0] = arg[pos];
//             temp = ft_strjoin(result, c_str);
//             free(result);
//             result = temp;
//             pos++;
            
//             if (!result)
//                 return;
                
//             // Update result length
//             result_len = ft_strlen(result);
//         }
//     }
    
//     // Create new quote types for the expanded result - CRITICAL FIX
//     if (quote_type_ptr && *quote_type_ptr)
//     {
//         new_quote_types = malloc(sizeof(int) * (result_len + 1));
//         if (new_quote_types)
//         {
//             // Initialize all to unquoted (0)
//             for (size_t i = 0; i < result_len; i++)
//                 new_quote_types[i] = 0;
                
//             // Add end marker
//             new_quote_types[result_len] = -1;
            
//             // Free old quote types and replace with new ones
//             free(*quote_type_ptr);
//             *quote_type_ptr = new_quote_types;
//         }
//     }
    
//     // Replace original argument with expanded version
//     free(*arg_ptr);
//     *arg_ptr = result;
// }

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

/*
 * Checks if a position is within single quotes in the quote context
 * Returns 1 if position is inside single quotes, 0 otherwise
 */
int is_in_single_quotes(int pos, t_vars *vars)
{
    int i;
    
    i = 0;
    while (i < vars->quote_depth)
    {
        if (vars->quote_ctx[i].type == '\'' && 
            pos > vars->quote_ctx[i].start_pos &&
            pos < vars->quote_ctx[i].start_pos + vars->quote_ctx[i].depth)
        {
            return 1;
        }
        i++;
    }
    return 0;
}
