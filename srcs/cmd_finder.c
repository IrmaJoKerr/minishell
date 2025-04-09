/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_finder.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 10:40:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/09 09:46:30 by bleow            ###   ########.fr       */
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
// t_node	*mode_action(t_node *current, t_node **last_cmd, t_vars *vars)
// {
// 	if (current->type == TYPE_CMD)
// 	{
// 		if (vars->find_mode == FIND_ALL && vars->cmd_count < 100)
// 		{
// 			vars->cmd_nodes[vars->cmd_count] = current;
// 			vars->cmd_count++;
// 		}
// 		*last_cmd = current;
// 		if (vars->find_mode == FIND_NEXT && vars->find_start != current)
// 			return (current);
// 	}
// 	if (vars->find_mode == FIND_PREV && current == vars->find_tgt)
// 		return (*last_cmd);
// 	if ((vars->find_mode == FIND_PREV) && current->type == TYPE_PIPE)
// 		*last_cmd = NULL;
	
// 	return (NULL);
// }
/*
Handles command node processing based on the current mode.
Returns a node if we found what we're looking for, NULL to continue searching.
*/
t_node *mode_action(t_node *current, t_node **last_cmd, t_vars *vars)
{
    fprintf(stderr, "DEBUG[mode_action]: Processing node %p (type=%d)\n", 
            (void*)current, current->type);
    
    if (current->type == TYPE_CMD) {
        fprintf(stderr, "DEBUG[mode_action]: Found command node: '%s'\n", 
                current->args && current->args[0] ? current->args[0] : "NULL");
        
        if (vars->find_mode == FIND_ALL && vars->cmd_count < 100) {
            vars->cmd_nodes[vars->cmd_count] = current;
            vars->cmd_count++;
            fprintf(stderr, "DEBUG[mode_action]: Added to cmd_nodes[%d]\n", vars->cmd_count-1);
        }
        
        *last_cmd = current;
        fprintf(stderr, "DEBUG[mode_action]: Updated last_cmd to %p\n", (void*)*last_cmd);
        
        if (vars->find_mode == FIND_NEXT && vars->find_start != current) {
            fprintf(stderr, "DEBUG[mode_action]: FIND_NEXT match, returning node\n");
            return current;
        }
    }
    
    if (vars->find_mode == FIND_PREV && current == vars->find_tgt) {
        fprintf(stderr, "DEBUG[mode_action]: FIND_PREV match with target, returning last_cmd=%p\n", 
                (void*)*last_cmd);
        return *last_cmd;
    }
    
    if (vars->find_mode == FIND_PREV && current->type == TYPE_PIPE) {
        fprintf(stderr, "DEBUG[mode_action]: PIPE node in FIND_PREV mode, setting last_cmd=NULL\n");
        *last_cmd = NULL;
    }
    
    fprintf(stderr, "DEBUG[mode_action]: Continuing search (returning NULL)\n");
    return NULL;
}

/*
Sets up find_cmd temporary variables in the vars struct.
Used at the start and end of find_cmd().
*/
void	init_find_cmd(t_vars *vars, t_node *start, t_node *target, int mode)
{
	if (!vars)
		return ;
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
// t_node	*find_cmd(t_node *start, t_node *target, int mode, t_vars *vars)
// {
// 	t_node	*current;
// 	t_node	*last_cmd;
// 	t_node	*result;
	
// 	last_cmd = NULL;
// 	if (!start && mode != FIND_ALL)
// 		return (NULL);
// 	init_find_cmd(vars, start, target, mode);
// 	if (vars->find_mode == FIND_ALL && vars)
// 	{
// 		vars->cmd_count = 0;
// 		current = vars->head;
// 	}
// 	else
// 		current = start;
// 	while (current)
// 	{
// 		result = mode_action(current, &last_cmd, vars);
// 		if (result)
// 			return (result);
// 		current = current->next;
// 	}
// 	init_find_cmd(vars, NULL, NULL, 0);
// 	return (last_cmd);
// }
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
    t_node  *current;
    t_node  *last_cmd;
    t_node  *result;
    
    // Debug prints for input parameters
    fprintf(stderr, "DEBUG[find_cmd]: Called with start=%p, target=%p, mode=%d, vars=%p\n", 
            (void*)start, (void*)target, mode, (void*)vars);
    
    // Check if vars is NULL
    if (!vars) {
        fprintf(stderr, "DEBUG[find_cmd]: vars is NULL! Returning NULL\n");
        return NULL;
    }
    
    last_cmd = NULL;
    if (!start && mode != FIND_ALL) {
        fprintf(stderr, "DEBUG[find_cmd]: start is NULL and mode != FIND_ALL, returning NULL\n");
        return NULL;
    }
    
    fprintf(stderr, "DEBUG[find_cmd]: Calling init_find_cmd()\n");
    init_find_cmd(vars, start, target, mode);
    fprintf(stderr, "DEBUG[find_cmd]: After init_find_cmd: find_start=%p, find_tgt=%p, find_mode=%d\n",
            (void*)vars->find_start, (void*)vars->find_tgt, vars->find_mode);
    
    if (vars->find_mode == FIND_ALL && vars) {
        vars->cmd_count = 0;
        current = vars->head;
        fprintf(stderr, "DEBUG[find_cmd]: FIND_ALL mode, starting from head=%p\n", (void*)vars->head);
    } else {
        current = start;
        fprintf(stderr, "DEBUG[find_cmd]: Using start as current=%p\n", (void*)current);
    }
    
    // Process each node in the list
    while (current) {
        fprintf(stderr, "DEBUG[find_cmd]: Processing node %p type=%d\n", (void*)current, current->type);
        
        result = mode_action(current, &last_cmd, vars);
        
        if (result) {
            fprintf(stderr, "DEBUG[find_cmd]: Found result, returning %p\n", (void*)result);
            init_find_cmd(vars, NULL, NULL, 0);
            return result;
        }
        
        current = current->next;
        fprintf(stderr, "DEBUG[find_cmd]: Moving to next node=%p\n", (void*)current);
    }
    
    fprintf(stderr, "DEBUG[find_cmd]: No more nodes, resetting vars\n");
    init_find_cmd(vars, NULL, NULL, 0);
    fprintf(stderr, "DEBUG[find_cmd]: Returning last_cmd=%p\n", (void*)last_cmd);
    return last_cmd;
}
