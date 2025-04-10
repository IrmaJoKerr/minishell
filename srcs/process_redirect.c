/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redirect.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:40:07 by bleow             #+#    #+#             */
/*   Updated: 2025/04/11 01:05:36 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Master redirection processing function.
- Controls the overall redirection handling workflow.
- Identifies commands and redirection operators.
- Builds redirection nodes and links them properly.
- Integrates redirections with pipe structures if present.
Returns:
- Root redirection node for the AST if found.
- NULL if no valid redirections exist in the token stream.
Works with proc_token_list.
*/
t_node *proc_redir(t_vars *vars)
{
	// Debug print for tracing
	fprintf(stderr, "DEBUG: proc_redir function called\n");
	
	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	
	// Reset redirection tracking state
	reset_redir_tracking(vars->pipes);
	
	// Build the redirection AST by connecting commands to redirection operators
	build_redir_ast(vars);
	
	// Integrate redirections with pipe structure if needed
	if (vars->pipes->pipe_root)
		integrate_redirections_with_pipes(vars);
	
	return vars->pipes->redir_root;
}

/*
Resets redirection tracking state in the pipes structure.
- Clears command and redirection node pointers.
- Prepares pipes structure for new redirection processing.
Works with proc_redir to clean state before processing.
*/
void reset_redir_tracking(t_pipe *pipes)
{
	fprintf(stderr, "DEBUG: Resetting redirection tracking\n");
	
	if (!pipes)
		return;
		
	pipes->last_cmd = NULL;
	pipes->redir_root = NULL;
	pipes->last_in_redir = NULL;
	pipes->last_out_redir = NULL;
	pipes->cmd_redir = NULL;
}

/*
Builds the redirection AST by connecting commands to redirection operators.
- Traverses token list once, tracking commands and redirections.
- Links redirection nodes to their target commands and targets.
- Sets pipes->redir_root to the first valid redirection.
Works with proc_redir for redirection structure building.
*/
void build_redir_ast(t_vars *vars)
{
	t_node *current = vars->head;
	
	fprintf(stderr, "DEBUG: Building redirection AST\n");
	
	while (current)
	{
		// Track commands as we go
		if (current->type == TYPE_CMD)
		{
			vars->pipes->last_cmd = current;
			fprintf(stderr, "DEBUG: Found command: %s\n", 
				  current->args ? current->args[0] : "NULL");
		}
		// Process redirections
		else if (is_redirection(current->type) && current->next)
		{
			// Keep track of last redirection of each type
			if (current->type == TYPE_IN_REDIRECT)
			{
				vars->pipes->last_in_redir = current;
				fprintf(stderr, "DEBUG: Found input redirection: %s\n",
					   current->next->args ? current->next->args[0] : "NULL");
			}
			else if (current->type == TYPE_OUT_REDIRECT || 
					 current->type == TYPE_APPEND_REDIRECT)
			{
				vars->pipes->last_out_redir = current;
				fprintf(stderr, "DEBUG: Found output redirection: %s\n",
					   current->next->args ? current->next->args[0] : "NULL");
			}
			
			// Process the redirection node
			process_redir_node(current, vars);
		}
		current = current->next;
	}
}

/*
Processes an individual redirection node.
- Finds the target command for the redirection.
- Links the redirection node to command and target.
- Updates the redirection root if this is the first redirection.
- Records debug information about the redirection.
Works with build_redir_ast during AST construction.
*/
void process_redir_node(t_node *redir_node, t_vars *vars)
{
	fprintf(stderr, "DEBUG: Processing redirection node, type: %d\n", 
			redir_node->type);
			
	// Find the target command
	t_node *cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
	
	if (cmd && redir_node->next)
	{
		// Set up the redirection relationship
		set_redir_node(redir_node, cmd, redir_node->next);
		
		// Update root if this is the first redirection
		if (!vars->pipes->redir_root)
			vars->pipes->redir_root = redir_node;
			
		fprintf(stderr, "DEBUG: Set up redirection from cmd '%s' to '%s'\n",
				cmd->args ? cmd->args[0] : "NULL",
				redir_node->next->args ? redir_node->next->args[0] : "NULL");
	}
	else
	{
		fprintf(stderr, "DEBUG: Invalid redirection setup\n");
	}
}

/*
Integrates redirection nodes with the pipe structure.
- Updates pipe node references to point to redirection nodes.
- Ensures pipes use redirection nodes instead of direct commands.
- Makes pipe commands output to redirections correctly.
Works with proc_redir when pipe nodes exist.
*/
void integrate_redirections_with_pipes(t_vars *vars)
{
	t_node *current;
	t_node *target_cmd;
	
	if (!vars || !vars->pipes || !vars->pipes->pipe_root || !vars->head)
		return;
		
	fprintf(stderr, "DEBUG: Integrating redirections with pipes\n");
	
	current = vars->head;
	vars->pipes->last_cmd = NULL;
	
	while (current)
	{
		if (current->type == TYPE_CMD)
		{
			vars->pipes->last_cmd = current;
			fprintf(stderr, "DEBUG: Found command in pipe integration: %s\n", 
				   current->args ? current->args[0] : "NULL");
		}
		else if (is_redirection(current->type) && current->next)
		{
			// For test 77: When dealing with missing files in pipes, we should
			// still complete the AST and let the execution phase handle the error
			target_cmd = get_redir_target(current, vars->pipes->last_cmd);
			
			if (target_cmd && vars->pipes->pipe_root)
			{
				fprintf(stderr, "DEBUG: Updating pipe for cmd '%s' to use redirection type %d\n", 
					   target_cmd->args ? target_cmd->args[0] : "NULL", current->type);
				upd_pipe_redir(vars->pipes->pipe_root, target_cmd, current);
			}
		}
		current = current->next;
	}
	
	fprintf(stderr, "DEBUG: Completed pipe structure integration\n");
}

/*
Configures a redirection node with source and target commands.
- Sets left child to source command node.
- Sets right child to target command/filename node.
- Establishes the redirection relationship in the AST.
Works with proc_redir().
*/
void	set_redir_node(t_node *redir, t_node *cmd, t_node *target)
{
	if (!redir || !cmd || !target)
		return ;
	redir->left = cmd;
	redir->right = target;
}

/*
Retrieves the target command for a redirection node.
- Checks if the previous node is a command.
- If not, uses the last command seen in the pipe structure.
- Ensures the target command is valid.
Returns:
- Pointer to the target command node.
- NULL if no valid target found.
*/
t_node	*get_redir_target(t_node *current, t_node *last_cmd)
{
	t_node	*target;

	target = NULL;
	if (current->prev && current->prev->type == TYPE_CMD)
		target = current->prev;
	else
		target = last_cmd;
		
	// Make sure we have a valid target
	if (!target)
		DBG_PRINTF(DEBUG_EXEC, "No valid redirection target found\n");
	
	return (target);
}

/*
Updates pipe structure when commands are redirected.
- Traverses pipe chain looking for references to the command.
- Replaces command references with redirection node references.
- Preserves pipe structure while incorporating redirections.
- Handles both left and right side command replacements.
*/
void upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir)
{
	t_node *pipe_node;

	if (!pipe_root || !cmd || !redir)
		return;
	
	pipe_node = pipe_root;
	
	// Traverse the pipe chain
	while (pipe_node)
	{
		// Check if this pipe node references the command we want to redirect
		if (pipe_node->left == cmd)
		{
			fprintf(stderr, "DEBUG: Updating left branch of pipe to use redirection\n");
			pipe_node->left = redir;
		}
		else if (pipe_node->right == cmd)
		{
			fprintf(stderr, "DEBUG: Updating right branch of pipe to use redirection\n");
			pipe_node->right = redir;
		}
		
		// Move to the next pipe in the chain (if it exists and is a pipe)
		if (pipe_node->right && pipe_node->right->type == TYPE_PIPE)
			pipe_node = pipe_node->right;
		else
			break;
	}
}

/*
Determines if a redirection node has valid adjacent commands.
- Checks if next node exists and is a command.
Returns:
- 1 if redirection has valid syntax.
- 0 otherwise.
Works with proc_redir.
*/
int	is_valid_redir_node(t_node *current) //Possible to reuse
{
	if (!current)
		return (0);
	if (!is_redirection(current->type))
		return (0);
	 // Modified to accept ARGS type for redirection targets
	if (!current->next || (current->next->type != TYPE_CMD && current->next->type != TYPE_ARGS))
		return (0);
	return (1);
}
