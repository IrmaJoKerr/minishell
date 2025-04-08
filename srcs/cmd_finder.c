/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_finder.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 10:40:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/09 00:12:28 by bleow            ###   ########.fr       */
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
// t_node *find_cmd(t_node *start, t_node *target, int mode, t_vars *vars)
// {
//     t_node	*current;
//     t_node	*last_cmd;
//     int		cmd_index;
    
// 	last_cmd = NULL;
// 	cmd_index = 0;
//     if (!start && mode != FIND_ALL)
//         return NULL;
//     if (mode == FIND_ALL && vars)
//     {
//         vars->cmd_count = 0;
//         current = vars->head;
//     }
//     else
//     {
//         current = start;
//     }
//     while (current)
//     {
//         if (current->type == TYPE_CMD)
//         {
//             if (mode == FIND_ALL && vars && cmd_index < 100)
//             {
//                 vars->cmd_nodes[cmd_index++] = current;
//                 vars->cmd_count = cmd_index;
//             }
//             last_cmd = current;
//             if (mode == FIND_NEXT && start != current)
//                 return (current);
//         }
//         if (mode == FIND_PREV && current == target)
//             return (last_cmd);
//         if ((mode == FIND_PREV) && current->type == TYPE_PIPE)
//             last_cmd = NULL;
//         current = current->next;
//     }
//     return (last_cmd);
// }

/*
Handles command node processing based on the current mode.
Returns a node if we found what we're looking for, NULL to continue searching.
*/
t_node *mode_action(t_node *current, t_node **last_cmd, int *cmd_index, t_vars *vars)
{
    if (current->type == TYPE_CMD)
    {
        if (vars->find_mode == FIND_ALL && *cmd_index < 100)
        {
            vars->cmd_nodes[*cmd_index] = current;
            (*cmd_index)++;
            vars->cmd_count = *cmd_index;
        }
        *last_cmd = current;
        if (vars->find_mode == FIND_NEXT && vars->find_start != current)
            return (current);
    }
    if (vars->find_mode == FIND_PREV && current == vars->find_tgt)
        return (*last_cmd);
    if ((vars->find_mode == FIND_PREV) && current->type == TYPE_PIPE)
        *last_cmd = NULL;
    
    return (NULL);
}

/*
Sets up find_cmd temporary variables in the vars struct.
Used at the start and end of find_cmd().
*/
void init_find_cmd(t_vars *vars, t_node *start, t_node *target, int mode)
{
    if (!vars)
        return;
    vars->find_start = start;
    vars->find_tgt = target;
    vars->find_mode = mode;
}

/*
Core command finding function with multiple modes.
Modes:
- FIND_LAST: Find last command in the list (from head to end)
- FIND_PREV: Find last command before a specific node
- FIND_NEXT: Find next command after a specific node
- FIND_ALL: Returns first command node and populates vars->cmd_nodes 
   with all commands
Returns:
- The found command node.
- NULL if not found
*/
t_node *find_cmd(t_node *start, t_node *target, int mode, t_vars *vars)
{
    t_node	*current;
    t_node	*last_cmd;
    t_node	*result;
    int		cmd_index;
    
    last_cmd = NULL;
    cmd_index = 0;
    if (!start && mode != FIND_ALL)
        return (NULL);
    init_find_cmd(vars, start, target, mode);
    if (vars->find_mode == FIND_ALL && vars)
    {
        vars->cmd_count = 0;
        current = vars->head;
    }
    else
        current = start;
    while (current)
    {
        result = mode_action(current, &last_cmd, &cmd_index, vars);
        if (result)
            return (result);
        current = current->next;
    }
    init_find_cmd(vars, NULL, NULL, 0);
    return (last_cmd);
}
