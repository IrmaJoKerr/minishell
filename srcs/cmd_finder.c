/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_finder.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 10:40:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 10:40:31 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
 * Core command finding function with multiple modes
 * Modes:
 * - FIND_LAST: Find last command in the list (from head to end)
 * - FIND_PREV: Find last command before a specific node
 * - FIND_NEXT: Find next command after a specific node
 * - FIND_ALL: Populate vars->cmd_nodes with all commands
 * 
 * Returns the found command node, or NULL if not found
 * In FIND_ALL mode, returns first command node and populates vars->cmd_nodes
 */
t_node *find_cmd(t_node *start, t_node *target, int mode, t_vars *vars)
{
    t_node *current;
    t_node *last_cmd = NULL;
    int cmd_idx = 0;
    
    if (!start && mode != FIND_ALL)
        return NULL;
    
    if (mode == FIND_ALL && vars)
    {
        // Reset command count
        vars->cmd_count = 0;
        current = vars->head;
    }
    else
    {
        current = start;
    }

    while (current)
    {
        // Check if current node is a command
        if (current->type == TYPE_CMD)
        {
            // In FIND_ALL mode, populate the command array
            if (mode == FIND_ALL && vars && cmd_idx < 100)
            {
                vars->cmd_nodes[cmd_idx++] = current;
                vars->cmd_count = cmd_idx;
            }
            
            last_cmd = current;
            
            // In FIND_NEXT mode, if we already found a command after the starting point, return it
            if (mode == FIND_NEXT && start != current)
                return current;
        }
        
        // In FIND_PREV mode, if we've reached the target node, return the last command
        if (mode == FIND_PREV && current == target)
            return last_cmd;
            
        // In pipe-sensitive modes, reset command context after a pipe
        if ((mode == FIND_PREV) && current->type == TYPE_PIPE)
            last_cmd = NULL;
            
        current = current->next;
    }
    
    // For FIND_LAST, FIND_ALL, or if no command found after start in FIND_NEXT
    return last_cmd;
}
