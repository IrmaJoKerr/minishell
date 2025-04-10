/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:40:50 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 23:21:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets up pipe node connections by linking to command nodes.
- Connects pipe_node's left side to the previous command.
- Connects pipe_node's right side to the next command.
- Updates the pipes structure for tracking.
Works with process_first_pipe and process_addon_pipes.
*/
void	setup_pipe_node(t_node *pipe_node, t_node *left_cmd, t_node *right_cmd)
{
	if (!pipe_node || !left_cmd || !right_cmd)
		return ;
	// Set commands as children of the pipe
	pipe_node->left = left_cmd;
	pipe_node->right = right_cmd;
}

/*
Finds and processes the first pipe node in token stream.
- Searches for first pipe token.
- Identifies commands before and after the pipe.
- Sets up pipe connections.
Returns:
The pipe node if found and valid.
NULL if no valid pipe configuration exists.
Works with proc_pipes.
*/
t_node	*process_first_pipe(t_vars *vars)
{
	t_node	*current;
	t_node	*next_cmd;

	if (!vars || !vars->pipes)
		return (NULL);
	// Initialize tracking
	current = vars->head;
	vars->pipes->last_cmd = NULL;
	// Scan for first pipe and surrounding commands
	while (current)
	{
		// Track commands as we encounter them
		if (current->type == TYPE_CMD)
			vars->pipes->last_cmd = current;
		// When we find a pipe, check if it has commands on both sides
		else if (current->type == TYPE_PIPE)
		{
			// Find command after the pipe
			next_cmd = find_cmd(current->next, NULL, FIND_NEXT, vars);
			// If we have valid commands on both sides
			if (vars->pipes->last_cmd && next_cmd)
			{
				// Set up pipe node connections
				setup_pipe_node(current, vars->pipes->last_cmd, next_cmd);
				vars->pipes->last_pipe = current;
				return (current);
			}
		}
		current = current->next;
	}
	return (NULL);
}

/*
Processes remaining pipe nodes after the first pipe.
- Continues scanning from after the first pipe.
- Sets up additional pipe nodes.
- Links them into the pipe chain.
Returns:
Nothing (void function).
Works with proc_pipes.
*/
void	process_addon_pipes(t_vars *vars)
{
	t_node	*current;
	t_node	*next_cmd;

	if (!vars || !vars->pipes || !vars->pipes->last_pipe)
		return ;
	// Start from after the last processed pipe
	current = vars->pipes->last_pipe->next;
	if (!current)
		return ;
	// Reset command tracking - we'll track as we go
	vars->pipes->last_cmd = NULL;
	// Process remaining tokens
	while (current)
	{
		// Track commands
		if (current->type == TYPE_CMD)
			vars->pipes->last_cmd = current;
		else if (current->type == TYPE_PIPE)
		{
			// Find the next command after this pipe
			next_cmd = find_cmd(current->next, NULL, FIND_NEXT, vars);
			// If we have valid commands to connect
			if (vars->pipes->last_cmd && next_cmd)
			{
				// Set up pipe node
				setup_pipe_node(current, vars->pipes->last_cmd, next_cmd);
				// Link to existing chain
				vars->pipes->last_pipe->right = current;
				// Update tracking
				vars->pipes->last_pipe = current;
			}
		}
		current = current->next;
	}
}

/*
Master function for pipe node processing in token list.
- Initializes pipe tracking structures.
- Identifies and processes the first pipe.
- Processes any additional pipes in sequence.
- Builds a complete pipe chain structure.
Returns:
Root pipe node for AST if pipes found.
NULL if no valid pipes in token stream.
Works with proc_token_list.
*/
t_node	*proc_pipes(t_vars *vars)
{
	t_node	*pipe_root;
	
	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	// Initialize tracking
	vars->pipes->pipe_root = NULL;
	vars->pipes->last_pipe = NULL;
	vars->pipes->last_cmd = NULL;
	// Process first pipe node
	pipe_root = process_first_pipe(vars);
	if (!pipe_root)
		return (NULL);
	// Store root and process additional pipes
	vars->pipes->pipe_root = pipe_root;
	process_addon_pipes(vars);
	return (pipe_root);
}
