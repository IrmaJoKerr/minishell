/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:40:50 by bleow             #+#    #+#             */
/*   Updated: 2025/05/25 22:00:23 by bleow            ###   ########.fr       */
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
// void	setup_pipe_node(t_node *pipe_node, t_node *left_cmd, t_node *right_cmd)
// {
// 	if (!pipe_node || !left_cmd || !right_cmd)
// 		return ;
// 	pipe_node->left = left_cmd;
// 	pipe_node->right = right_cmd;
// }
void setup_pipe_node(t_node *pipe_node, t_node *left_cmd, t_node *right_cmd)
{
    fprintf(stderr, "DEBUG-PIPE-SETUP-DETAILED: ===== SETTING UP PIPE NODE =====\n");
    
    fprintf(stderr, "DEBUG-PIPE-SETUP-DETAILED: Initial pipe node:\n");
    print_node_debug(pipe_node, "PIPE", "setup_pipe_node");
    
    fprintf(stderr, "DEBUG-PIPE-SETUP-DETAILED: Left command:\n");
    print_node_debug(left_cmd, "LEFT", "setup_pipe_node");
    
    fprintf(stderr, "DEBUG-PIPE-SETUP-DETAILED: Right command:\n");
    print_node_debug(right_cmd, "RIGHT", "setup_pipe_node");
    
    if (!pipe_node || !left_cmd || !right_cmd) {
        fprintf(stderr, "DEBUG-PIPE-SETUP-DETAILED: Missing required nodes, setup aborted\n");
        return;
    }
    
    pipe_node->left = left_cmd;
    pipe_node->right = right_cmd;
    
    fprintf(stderr, "DEBUG-PIPE-SETUP-DETAILED: Final pipe node after setup:\n");
    print_node_debug(pipe_node, "PIPE-FINAL", "setup_pipe_node");
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
	current = vars->head;
	vars->pipes->last_cmd = NULL;
	while (current)
	{
		if (current->type == TYPE_CMD)
			vars->pipes->last_cmd = current;
		else if (current->type == TYPE_PIPE)
		{
			next_cmd = find_cmd(current->next, NULL, FIND_NEXT, vars);
			if (vars->pipes->last_cmd && next_cmd)
			{
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
	current = vars->pipes->last_pipe->next;
	if (!current)
		return ;
	vars->pipes->last_cmd = NULL;
	while (current)
	{
		if (current->type == TYPE_CMD)
			vars->pipes->last_cmd = current;
		else if (current->type == TYPE_PIPE)
		{
			next_cmd = find_cmd(current->next, NULL, FIND_NEXT, vars);
			if (vars->pipes->last_cmd && next_cmd)
			{
				setup_pipe_node(current, vars->pipes->last_cmd, next_cmd);
				vars->pipes->last_pipe->right = current;
				vars->pipes->last_pipe = current;
			}
		}
		current = current->next;
	}
}
