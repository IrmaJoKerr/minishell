/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/04/08 10:07:59 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Processes expansion tokens in the token list
 * Finds the command each expansion belongs to
 * Expands variables and adds them as arguments
 * Removes the expansion tokens when done
 */
void process_expansions(t_vars *vars)
{
    t_node *current = vars->head;
    char *var_name;
    char *expanded_value;
    t_node *cmd_node;

    while (current)
    {
        
        // Special case: standalone $? that's already been processed to a TYPE_ARGS with numeric value
        if (current->type == TYPE_ARGS && current->args && !current->prev && 
            ft_strchr("0123456789", current->args[0][0]))
        {
            fprintf(stderr, "DEBUG: Found standalone exit code: %s\n", current->args[0]);
            
            // Print the exit code directly like bash does
            ft_putendl_fd(current->args[0], STDOUT_FILENO);
            
            // Mark as handled by changing to a command
            current->type = TYPE_CMD;
            fprintf(stderr, "DEBUG: Changed standalone exit code to TYPE_CMD\n");
            
            // Continue with next token
            current = current->next;
            continue;
        }
            
        if (current->type == TYPE_EXPANSION || current->type == TYPE_EXIT_STATUS)
        {
            // Extract variable name from token (skip the $ prefix)
            var_name = current->args[0] + 1;
            fprintf(stderr, "DEBUG: Expansion var_name='%s'\n", var_name);
            
            // Always expand the value first (even for special cases)
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
            
            // Find the preceding command node (if any)
            cmd_node = find_cmd(vars->head, current, FIND_PREV, NULL);
            
            if (cmd_node)
            {
                // We have a command to add this as an argument to
                fprintf(stderr, "DEBUG: Adding '%s' as arg to command '%s'\n", 
                    expanded_value, cmd_node->args[0]);
                append_arg(cmd_node, expanded_value, 0);
                
                // Clean up the expansion node
                free(current->args[0]);
                current->type = TYPE_ARGS;
                fprintf(stderr, "DEBUG: Changed node to TYPE_ARGS\n");
                
                // Free expanded value as it's been copied by append_arg
                free(expanded_value);
            }
            else
            {
                // No command found - expansion is at command position
                // Special case for $? or other expansions at command position
                if (var_name[0] == '?')
                {
                    // For lone $?, print the expanded value directly
                    fprintf(stderr, "DEBUG: Printing standalone $?: '%s'\n", expanded_value);
                    ft_putendl_fd(expanded_value, STDOUT_FILENO);
                    
                    // Clean up and convert to a marker command
                    free(current->args[0]);
                    current->args[0] = ft_strdup("minishell_printed");
                    free(expanded_value);
                    current->type = TYPE_CMD;
                }
                else
                {
                    // For other expansions, convert to command as before
                    fprintf(stderr, "DEBUG: Converting expansion to command, value='%s'\n", expanded_value);
                    free(current->args[0]);
                    current->args[0] = expanded_value;
                    current->type = TYPE_CMD;
                    fprintf(stderr, "DEBUG: Node is now TYPE_CMD with args[0]='%s'\n", current->args[0]);
                    // No need to free expanded_value as it's now owned by the node
                }
            }
        }
        current = current->next;
    }
    
    // Debug: Print final token list state
    fprintf(stderr, "DEBUG: Final token list after processing expansions:\n");
    current = vars->head;
    while (current) 
    {
        fprintf(stderr, "DEBUG: Node type=%d, args[0]='%s'\n", 
            current->type, current->args ? current->args[0] : "NULL");
        current = current->next;
    }
}
