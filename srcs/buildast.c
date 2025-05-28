/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 16:44:48 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes the entire token list to build the AST structure.
- Identifies command nodes in the token list.
- Builds pipe structures for commands separated by pipes.
- Processes redirections and integrates with pipe structure.
- Selects the appropriate root node for the final AST.
Example: For input "ls -l | grep a > output.txt":
- Finds "ls" and "grep" commands.
- Creates pipe structure connecting them.
- Attaches ">" redirection to "grep".
- Returns pipe node as root with redirection integrated.

Returns:
- Root node of the constructed AST.
- NULL if invalid syntax or no commands found.
Works with build_and_execute().
*/
t_node	*proc_token_list(t_vars *vars)
{
	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	find_cmd(NULL, NULL, FIND_ALL, vars);
	if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
		|| !vars->cmd_nodes[0]->args)
		return (NULL);
	vars->pipes->pipe_root = NULL;
	vars->pipes->redir_root = NULL;
	vars->pipes->pipe_root = proc_pipes(vars);
	vars->pipes->redir_root = proc_redir(vars);
	verify_command_args(vars);
	
	// NEW: Add AST structure validation debug
	if (vars->pipes->pipe_root)
	{
		fprintf(stderr, "DEBUG-AST-FINAL: === FINAL AST STRUCTURE ===\n");
		fprintf(stderr, "DEBUG-AST-FINAL: Root is PIPE\n");
		fprintf(stderr, "DEBUG-AST-FINAL: Left: %s (%s)\n",
				get_token_str(vars->pipes->pipe_root->left->type),
				vars->pipes->pipe_root->left && vars->pipes->pipe_root->left->args ? 
				vars->pipes->pipe_root->left->args[0] : "NULL");
		fprintf(stderr, "DEBUG-AST-FINAL: Right: %s (%s)\n",
				get_token_str(vars->pipes->pipe_root->right->type),
				vars->pipes->pipe_root->right && vars->pipes->pipe_root->right->args ? 
				vars->pipes->pipe_root->right->args[0] : "NULL");
				
		// Check if left side has redirection structure
		if (is_redirection(vars->pipes->pipe_root->left->type))
		{
			fprintf(stderr, "DEBUG-AST-FINAL: Left redirection->left: %s (%s)\n",
					vars->pipes->pipe_root->left->left ? 
					get_token_str(vars->pipes->pipe_root->left->left->type) : "NULL",
					vars->pipes->pipe_root->left->left && vars->pipes->pipe_root->left->left->args ? 
					vars->pipes->pipe_root->left->left->args[0] : "NULL");
		}
		
		// Check if right side has redirection structure  
		if (is_redirection(vars->pipes->pipe_root->right->type))
		{
			fprintf(stderr, "DEBUG-AST-FINAL: Right redirection->left: %s (%s)\n",
					vars->pipes->pipe_root->right->left ? 
					get_token_str(vars->pipes->pipe_root->right->left->type) : "NULL",
					vars->pipes->pipe_root->right->left && vars->pipes->pipe_root->right->left->args ? 
					vars->pipes->pipe_root->right->left->args[0] : "NULL");
		}
	}
	
	if (vars->pipes->pipe_root)
		return (vars->pipes->pipe_root);
	else if (vars->pipes->redir_root)
		return (vars->pipes->redir_root);
	else if (vars->cmd_count > 0)
		return (vars->cmd_nodes[0]);
	return (NULL);
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
	int i;
	
	fprintf(stderr, "DEBUG-REDIR-BUILD: Building redirection AST (command-first approach)\n");
	
	// First initialize any redirection nodes that have targets
	t_node *current = vars->head;
	while (current) {
		if (is_redirection(current->type)) {
			// CRITICAL FIX: Improved validation logic for both types of redirection targets
			if (!is_valid_redir_node(current)) {
				tok_syntax_error_msg("newline", vars);
				return;
			}
			
			// Initialize redirection target (command will be set later)
			if (!vars->pipes->redir_root)
				vars->pipes->redir_root = current;
		}
		current = current->next;
	}
	
	// Process each command and find its redirections
	for (i = 0; i < vars->cmd_count; i++) {
		if (vars->cmd_nodes[i]) {
			fprintf(stderr, "DEBUG-REDIR-BUILD: Processing command '%s'\n",
					vars->cmd_nodes[i]->args ? vars->cmd_nodes[i]->args[0] : "NULL");
			
			// Link all redirections that belong to this command
			link_cmd_redirs(vars->cmd_nodes[i], vars);
		}
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
// t_node	*proc_pipes(t_vars *vars)
// {
// 	t_node	*pipe_root;

// 	if (!vars || !vars->head || !vars->pipes)
// 		return (NULL);
// 	vars->pipes->pipe_root = NULL;
// 	vars->pipes->last_pipe = NULL;
// 	vars->pipes->last_cmd = NULL;
// 	pipe_root = process_first_pipe(vars);
// 	if (!pipe_root)
// 		return (NULL);
// 	vars->pipes->pipe_root = pipe_root;
// 	process_addon_pipes(vars);
// 	return (pipe_root);
// }
t_node	*proc_pipes(t_vars *vars)
{
	t_node	*pipe_root;

	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	vars->pipes->pipe_root = NULL;
	vars->pipes->last_pipe = NULL;
	vars->pipes->last_cmd = NULL;
	pipe_root = process_first_pipe(vars);
	if (!pipe_root)
		return (NULL);
	vars->pipes->pipe_root = pipe_root;
	if (vars->pipes->pipe_root)
	{
		// CRITICAL ADD: Ensure flag is set when pipe_root exists
		vars->pipes->in_pipe = 1;
		process_addon_pipes(vars);
	}
	return (pipe_root);
}

// void	verify_command_args(t_vars *vars)
// {
// 	t_node	*current;
// 	t_node	*cmd;
// 	t_node	**cmd_ptr;
// 	int		remaining_cmds;

// 	cmd_ptr = vars->cmd_nodes;
// 	remaining_cmds = vars->cmd_count;
// 	while (remaining_cmds > 0)
// 	{
// 		cmd = *cmd_ptr;
// 		if (!cmd || !cmd->args)
// 		{
// 			cmd_ptr++;
// 			remaining_cmds--;
// 			continue ;
// 		}
// 		current = find_node_in_list(vars->head, cmd);
// 		if (!current)
// 		{
// 			cmd_ptr++;
// 			remaining_cmds--;
// 			continue ;
// 		}
// 		current = current->next;
// 		while (current && current->type != TYPE_CMD
// 			&& current->type != TYPE_PIPE)
// 		{
// 			if (current->type == TYPE_ARGS
// 				&& !is_redirection_target(current, vars))
// 			{
// 				if (!is_arg_in_cmd(cmd, current->args[0]))
// 					append_arg(cmd, current->args[0], 0);
// 			}
// 			current = current->next;
// 		}
// 		cmd_ptr++;
// 		remaining_cmds--;
// 	}
// }
void verify_command_args(t_vars *vars)
{
	t_node *current;
	t_node *cmd_node = NULL;
	
	fprintf(stderr, "DEBUG-VERIFY-ARGS: Starting argument verification\n");
	
	if (!vars || !vars->head)
		return;
		
	current = vars->head;
	while (current)
	{
		fprintf(stderr, "DEBUG-VERIFY-ARGS: Processing node type=%s, args[0]='%s'\n",
				get_token_str(current->type),
				current->args ? current->args[0] : "NULL");
				
		if (current->type == TYPE_CMD)
		{
			cmd_node = current;
			fprintf(stderr, "DEBUG-VERIFY-ARGS: Found command '%s'\n", 
					cmd_node->args[0]);
		}
		else if (current->type == TYPE_ARGS && cmd_node && 
				 !is_redirection_target(current, vars))
		{
			fprintf(stderr, "DEBUG-VERIFY-ARGS: Found orphaned argument '%s' for command '%s'\n",
					current->args[0], cmd_node->args[0]);
			
			// Append this argument to the command
			append_arg(cmd_node, current->args[0], 0);
			
			fprintf(stderr, "DEBUG-VERIFY-ARGS: Appended argument '%s' to command '%s'\n",
					current->args[0], cmd_node->args[0]);
		}
		else if (is_redirection(current->type))
		{
			fprintf(stderr, "DEBUG-VERIFY-ARGS: Found redirection, continuing with same command context\n");
			// Don't reset cmd_node - keep it for subsequent arguments
		}
		else if (current->type == TYPE_PIPE)
		{
			fprintf(stderr, "DEBUG-VERIFY-ARGS: Found pipe, resetting command context\n");
			cmd_node = NULL; // Reset for next command in pipeline
		}
		
		current = current->next;
	}
	
	fprintf(stderr, "DEBUG-VERIFY-ARGS: Argument verification completed\n");
}

// Helper function to check if an argument already exists in command's arguments
int	is_arg_in_cmd(t_node *cmd, char *arg)
{
	int	i;

	i = 0;
	if (!cmd || !cmd->args || !arg)
		return (0);
	while (cmd->args[i])
	{
		if (ft_strcmp(cmd->args[i], arg) == 0)
			return (1);
		i++;
	}
	return (0);
}

// Helper function to find a node in the linked list
t_node	*find_node_in_list(t_node *head, t_node *target)
{
	t_node	*current;

	current = head;
	while (current)
	{
		if (current == target)
			return (current);
		current = current->next;
	}
	return (NULL);
}

// Helper function to check if a node is a redirection target
// int	is_redirection_target(t_node *node, t_vars *vars)
// {
// 	t_node	*current;

// 	current = vars->head;
// 	while (current)
// 	{
// 		if (is_redirection(current->type) && current->right == node)
// 			return (1);
// 		current = current->next;
// 	}
// 	return (0);
// }
// int is_redirection_target(t_node *node, t_vars *vars)
// {
// 	t_node *current = vars->head;
	
// 	while (current)
// 	{
// 		if (is_redirection(current->type))
// 		{
// 			// Check if this node immediately follows a redirection
// 			if (current->next == node)
// 			{
// 				fprintf(stderr, "DEBUG-VERIFY-ARGS: Node '%s' is redirection target\n",
// 						node->args ? node->args[0] : "NULL");
// 				return 1;
// 			}
// 		}
// 		current = current->next;
// 	}
// 	return 0;
// }
int is_redirection_target(t_node *node, t_vars *vars)
{
    t_node *current = vars->head;
    
    fprintf(stderr, "DEBUG-REDIR-TARGET: Checking if node '%s' is redirection target\n",
            node->args ? node->args[0] : "NULL");
    
    while (current)
    {
        if (is_redirection(current->type))
        {
            fprintf(stderr, "DEBUG-REDIR-TARGET: Found redirection type=%s with filename='%s'\n",
                    get_token_str(current->type),
                    current->args && current->args[0] ? current->args[0] : "NONE");
            
            // Check if this node immediately follows a redirection
            if (current->next == node)
            {
                // CRITICAL FIX: Check if redirection already has embedded filename
                if (current->args && current->args[0] && strlen(current->args[0]) > 0)
                {
                    fprintf(stderr, "DEBUG-REDIR-TARGET: Redirection already complete with '%s', node '%s' is NOT target\n",
                            current->args[0], node->args ? node->args[0] : "NULL");
                    return 0;
                }
                else
                {
                    fprintf(stderr, "DEBUG-REDIR-TARGET: Redirection needs target, node '%s' IS target\n",
                            node->args ? node->args[0] : "NULL");
                    return 1;
                }
            }
        }
        current = current->next;
    }
    
    fprintf(stderr, "DEBUG-REDIR-TARGET: Node '%s' is NOT a redirection target\n",
            node->args ? node->args[0] : "NULL");
    return 0;
}

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
	if (!vars || !vars->head)
		return (NULL);
	
	reset_redir_tracking(vars->pipes);
	
	// Find all commands first
	find_cmd(NULL, NULL, FIND_ALL, vars);
	
	// Build the redirection AST with new forward command-based approach
	build_redir_ast(vars);
	
	// If we also have pipe structure, integrate redirections with it
	if (vars->pipes->pipe_root)
		link_redirs_pipes(vars);
	
	return (vars->pipes->redir_root);
}


/*
Processes a redirection node during AST construction.
- Sets up connections between command, redirection, and target nodes.
- Updates pipeline tracking structures for later processing.
Works with build_redir_ast().
*/
void process_redir_node(t_node *redir_node, t_vars *vars)
{
	t_node *cmd;
	
	fprintf(stderr, "DEBUG-PROCESS-REDIR: Processing redirection type=%s\n", 
			get_token_str(redir_node->type));
	
	// Debug print the filename based on whether it's embedded or in next node
	if (redir_node->args && redir_node->args[0])
		fprintf(stderr, "DEBUG-PROCESS-REDIR: Filename (embedded): '%s'\n", redir_node->args[0]);
	else if (redir_node->next && redir_node->next->args && redir_node->next->args[0])
		fprintf(stderr, "DEBUG-PROCESS-REDIR: Filename (next node): '%s'\n", redir_node->next->args[0]);
	else
		fprintf(stderr, "DEBUG-PROCESS-REDIR: No filename found\n");
	
	cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
	if (!cmd)
	{
		fprintf(stderr, "DEBUG-PROCESS-REDIR: No command target found\n");
		return;
	}
	
	fprintf(stderr, "DEBUG-PROCESS-REDIR: Target command is '%s'\n",
			cmd->args ? cmd->args[0] : "NULL");
	
	// Directly set the left pointer to the command
	redir_node->left = cmd;
	
	// Mark this redirection as belonging to this specific command
	redir_node->redir = cmd;
	fprintf(stderr, "DEBUG-PROCESS-REDIR: Marked redirection as belonging to command '%s'\n",
			cmd->args ? cmd->args[0] : "NULL");
	
	// CRITICAL FIX: Process the filename for quoted redirections
	if (redir_node->args && redir_node->args[0])
	{
		fprintf(stderr, "DEBUG-PROCESS-REDIR: Processing embedded filename '%s'\n", 
				redir_node->args[0]);
		strip_outer_quotes(&redir_node->args[0], vars);
		fprintf(stderr, "DEBUG-PROCESS-REDIR: After stripping quotes: '%s'\n", 
				redir_node->args[0]);
	}
	
	// Collect any arguments that follow this redirection
	// collect_args_after_redir(redir_node, cmd);
	
	// Set the redirection root if this is the first one
	if (!vars->pipes->redir_root)
	{
		vars->pipes->redir_root = redir_node;
		fprintf(stderr, "DEBUG-PROCESS-REDIR: Set redir_root to this node\n");
	}
	
	// Track redirection type
	track_redirs(redir_node, cmd, vars);
}
