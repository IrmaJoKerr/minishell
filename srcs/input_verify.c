/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 19:19:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Processes expansion tokens in the token list
 * Finds the command each expansion belongs to
 * Expands variables and adds them as arguments
 * Removes the expansion tokens when done
 */
void	process_expansions(t_vars *vars)
{
    t_node *current = vars->head;
    char *var_name;
    char *expanded_value;
    t_node *cmd_node;

    while (current)
    {
        if (current->type == TYPE_EXPANSION || current->type == TYPE_EXIT_STATUS)
        {
            // Extract variable name from token (skip the $ prefix)
            var_name = current->args[0] + 1;
            // Special case for $?
            if (ft_strcmp(var_name, "?") == 0)
            {
                expanded_value = ft_itoa(vars->error_code);
            }
            else
            {
                expanded_value = get_env_val(var_name, vars->env);
            }
            if (!expanded_value)
            {
                expanded_value = ft_strdup("");
            } 
            
            // Find the preceding command node
            cmd_node = find_cmd(vars->head, current, FIND_PREV, NULL);
            if (cmd_node)
            {
                append_arg(cmd_node, expanded_value, 0); // Append to command node
            }
            else
            {
                free(expanded_value);
            }
            
            // Clean up the expansion node
            free(current->args[0]);
            free(expanded_value);
            current->type = TYPE_ARGS; // Change type to ARGS
        }
        current = current->next;
    }
}
