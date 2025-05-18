/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/05/18 07:46:44 by bleow            ###   ########.fr       */
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
// t_node	*proc_token_list(t_vars *vars) PRE ADDED DEBUG PRINTS
// {
// 	if (!vars || !vars->head || !vars->pipes)
// 		return (NULL);
// 	find_cmd(NULL, NULL, FIND_ALL, vars);
// 	if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
// 		|| !vars->cmd_nodes[0]->args)
// 		return (NULL);
// 	vars->pipes->pipe_root = NULL;
// 	vars->pipes->redir_root = NULL;
// 	vars->pipes->pipe_root = proc_pipes(vars);
// 	vars->pipes->redir_root = proc_redir(vars);
// 	if (vars->pipes->pipe_root)
// 		return (vars->pipes->pipe_root);
// 	else if (vars->pipes->redir_root)
// 		return (vars->pipes->redir_root);
// 	else if (vars->cmd_count > 0)
// 		return (vars->cmd_nodes[0]);
// 	return (NULL);
// }
t_node	*proc_token_list(t_vars *vars)
{
	fprintf(stderr, "DEBUG-AST: Building AST from token list\n");
	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	find_cmd(NULL, NULL, FIND_ALL, vars);
	fprintf(stderr, "DEBUG-AST: Found %d command nodes\n", vars->cmd_count);
	if (vars->cmd_count == 0 || !vars->cmd_nodes[0] || !vars->cmd_nodes[0]->args) {
		fprintf(stderr, "DEBUG-AST: No valid commands found\n");
		return (NULL);
	}
	vars->pipes->pipe_root = NULL;
	vars->pipes->redir_root = NULL;
	vars->pipes->pipe_root = proc_pipes(vars);
	fprintf(stderr, "DEBUG-AST: Pipe processing complete, root: %p\n", (void*)vars->pipes->pipe_root);
	vars->pipes->redir_root = proc_redir(vars);
	fprintf(stderr, "DEBUG-AST: Redirection processing complete, root: %p\n", (void*)vars->pipes->redir_root);
	fprintf(stderr, "DEBUG-FINAL-AST: Root node type=%d\n", 
		vars->pipes->pipe_root ? vars->pipes->pipe_root->type :
		(vars->pipes->redir_root ? vars->pipes->redir_root->type :
			(vars->cmd_count > 0 ? vars->cmd_nodes[0]->type : 0)));
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
void	build_redir_ast(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_CMD)
		{
			vars->pipes->last_cmd = current;
		}
		else if (is_redirection(current->type))
		{
			if (!is_valid_redir_node(current) || !current->next->args
				|| !current->next->args[0])
			{
				tok_syntax_error_msg("newline", vars);
				return ;
			}
			process_redir_node(current, vars);
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
	vars->pipes->pipe_root = NULL;
	vars->pipes->last_pipe = NULL;
	vars->pipes->last_cmd = NULL;
	pipe_root = process_first_pipe(vars);
	if (!pipe_root)
		return (NULL);
	vars->pipes->pipe_root = pipe_root;
	process_addon_pipes(vars);
	return (pipe_root);
}

// void	verify_command_args(t_vars *vars)
// {
// 	t_node	*current;
// 	t_node	*cmd;
// 	int		i;

// 	fprintf(stderr, "DEBUG-VERIFY: Checking for missed arguments\n");
// 	// For each command, ensure we didn't miss any arguments
// 	for (i = 0; i < vars->cmd_count; i++)
// 	{
// 		cmd = vars->cmd_nodes[i];
// 		if (!cmd || !cmd->args)
// 			continue ;
// 		fprintf(stderr, "DEBUG-VERIFY: Checking command %s\n", 
// 				cmd->args[0] ? cmd->args[0] : "NULL");
// 		// Verify all arguments between this command and next command/pipe
// 		current = find_node_in_list(vars->head, cmd);
// 		if (!current)
// 			continue ;
// 		current = current->next;
// 		while (current && current->type != TYPE_CMD && current->type != TYPE_PIPE)
// 		{
// 			// If this is an argument that's not already part of a redirection
// 			if (current->type == TYPE_ARGS && !is_redirection_target(current, vars))
// 			{
// 				fprintf(stderr, "DEBUG-VERIFY: Found missed argument '%s'\n", 
// 						current->args ? current->args[0] : "NULL");
// 				append_arg(cmd, current->args[0], 0);
// 			}
// 			current = current->next;
// 		}
// 	}
// }
// void verify_command_args(t_vars *vars)
// {
// 	t_node	*current;
// 	t_node	*cmd
// 	int		i;
	
// 	fprintf(stderr, "DEBUG-VERIFY: Checking for missed arguments\n");
	
// 	for (i = 0; i < vars->cmd_count; i++)
// 	{
// 		cmd = vars->cmd_nodes[i];
// 		if (!cmd || !cmd->args)
// 			continue ;
			
// 		fprintf(stderr, "DEBUG-VERIFY: Checking command %s\n", 
// 				cmd->args[0] ? cmd->args[0] : "NULL");
		
// 		current = find_node_in_list(vars->head, cmd);
// 		if (!current)
// 			continue ;
			
// 		current = current->next;
// 		while (current && current->type != TYPE_CMD && current->type != TYPE_PIPE)
// 		{
// 			if (current->type == TYPE_ARGS && !is_redirection_target(current, vars))
// 			{
// 				// Only add if not already present
// 				if (!is_arg_in_cmd(cmd, current->args[0]))
// 				{
// 					fprintf(stderr, "DEBUG-VERIFY: Found missed argument '%s'\n", 
// 							current->args[0] ? current->args[0] : "NULL");
// 					append_arg(cmd, current->args[0], 0);
// 				}
// 				else
// 				{
// 					fprintf(stderr, "DEBUG-VERIFY: Skipping duplicate argument '%s'\n", 
// 							current->args[0] ? current->args[0] : "NULL");
// 				}
// 			}
// 			current = current->next;
// 		}
// 	}
// }
void verify_command_args(t_vars *vars)
{
	t_node *current;
	int i;
	
	fprintf(stderr, "DEBUG-VERIFY: Checking for missed arguments\n");
	
	for (i = 0; i < vars->cmd_count; i++)
	{
		t_node *cmd = vars->cmd_nodes[i];
		if (!cmd || !cmd->args)
			continue;
			
		fprintf(stderr, "DEBUG-VERIFY: Checking command %s\n", 
				cmd->args[0] ? cmd->args[0] : "NULL");
				
		fprintf(stderr, "DEBUG-VERIFY: Current arguments for command '%s':\n", 
				cmd->args[0] ? cmd->args[0] : "NULL");
		for (int a = 0; cmd->args[a]; a++) {
			fprintf(stderr, "  Arg[%d]: '%s'\n", a, cmd->args[a]);
		}
		
		current = find_node_in_list(vars->head, cmd);
		if (!current)
			continue;
			
		current = current->next;
		while (current && current->type != TYPE_CMD && current->type != TYPE_PIPE)
		{
			if (current->type == TYPE_ARGS && !is_redirection_target(current, vars))
			{
				// Only add if not already present
				if (!is_arg_in_cmd(cmd, current->args[0]))
				{
					fprintf(stderr, "DEBUG-VERIFY: Found missed argument '%s'\n", 
							current->args[0] ? current->args[0] : "NULL");
					append_arg(cmd, current->args[0], 0);
				}
				else
				{
					fprintf(stderr, "DEBUG-VERIFY: Skipping duplicate argument '%s'\n", 
							current->args[0] ? current->args[0] : "NULL");
				}
			}
			current = current->next;
		}
		
		fprintf(stderr, "DEBUG-VERIFY: Final arguments for command '%s':\n", 
				cmd->args[0] ? cmd->args[0] : "NULL");
		for (int a = 0; cmd->args[a]; a++) {
			fprintf(stderr, "  Arg[%d]: '%s'\n", a, cmd->args[a]);
		}
	}
}

// Helper function to check if an argument already exists in command's arguments
int	is_arg_in_cmd(t_node *cmd, char *arg)
{
	int	i;
	
	i = 0;
	if (!cmd || !cmd->args || !arg)
		return 0;
	while (cmd->args[i])
	{
		if (ft_strcmp(cmd->args[i], arg) == 0)
			return 1;
		i++;
	}
	return 0;
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
int	is_redirection_target(t_node *node, t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type) && current->right == node)
			return (1);
		current = current->next;
	}
	return (0);
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
// t_node	*proc_redir(t_vars *vars)
// {
// 	if (!vars || !vars->head || !vars->pipes)
// 		return (NULL);
// 	reset_redir_tracking(vars->pipes);
// 	build_redir_ast(vars);
// 	if (vars->error_code == 2)
// 	{
// 		return (NULL);
// 	}
// 	if (vars->pipes->pipe_root)
// 		link_redirs_pipes(vars);
// 	if (vars->pipes->redir_root)
// 	{
// 		vars->astroot = vars->pipes->redir_root;
// 	}
// 	return (vars->pipes->redir_root);
// }
t_node	*proc_redir(t_vars *vars)
{
	if (!vars || !vars->head || !vars->pipes)
		return (NULL);
	reset_redir_tracking(vars->pipes);
	build_redir_ast(vars);
	if (vars->error_code == 2)
	{
		return (NULL);
	}
	// NEW: Verify all commands have their arguments properly collected
	// This catches any arguments that might have been missed during initial processing
	fprintf(stderr, "DEBUG-REDIR: Verifying all commands have proper arguments\n");
	verify_command_args(vars);
	if (vars->pipes->pipe_root)
		link_redirs_pipes(vars);
	if (vars->pipes->redir_root)
	{
		vars->astroot = vars->pipes->redir_root;
	}
	return (vars->pipes->redir_root);
}

void	collect_args_after_redir(t_node *redir_node, t_node *cmd_node)
{
	t_node	*target_node;
	t_node	*arg_node;
	
	fprintf(stderr, "DEBUG-COLLECT-ARGS: Starting collection after redirection\n");
	if (!redir_node || !cmd_node || !redir_node->right)
		return ;
	target_node = redir_node->right;
	arg_node = target_node->next;
	while (arg_node && arg_node->type == TYPE_ARGS)
	{
		fprintf(stderr, "DEBUG-COLLECT-ARGS: Found argument '%s' after redirection\n", 
				arg_node->args ? arg_node->args[0] : "NULL");
		append_arg(cmd_node, arg_node->args[0], 0);
		arg_node = arg_node->next;
	}
}

/*
Master control function for processing an individual redirection node.
- Finds the target command for the redirection
- Links the redirection node to command and target
- Chains with previous redirections
- Updates type-specific redirection trackers
- Sets the redirection root if this is the first redirection
Works with build_redir_ast() during AST construction.
*/
// void	process_redir_node(t_node *redir_node, t_vars *vars) PRE ADDED DEBUG PRINTS
// {
// 	t_node	*cmd;

// 	cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
// 	if (cmd && redir_node->next)
// 	{
// 		set_redir_node(redir_node, cmd, redir_node->next);
// 		link_prev_redirs(redir_node, cmd, vars);
// 		track_redirs(redir_node, cmd, vars);
// 		link_in_out_redirs(vars);
// 		if (!vars->pipes->redir_root)
// 		{
// 			vars->pipes->redir_root = redir_node;
// 		}
// 	}
// }
void	process_redir_node(t_node *redir_node, t_vars *vars)
{
	t_node	*cmd;

	cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
	fprintf(stderr, "DEBUG-REDIR: Redirection node type=%d, target cmd=%p, cmd type=%d\n", 
		redir_node->type, (void*)cmd, cmd ? cmd->type : 0);
	if (cmd && cmd->args)
		fprintf(stderr, "DEBUG-REDIR: Target command: %s\n", cmd->args[0] ? cmd->args[0] : "NULL");
	if (cmd && redir_node->next)
	{
		fprintf(stderr, "DEBUG-REDIR: Redirection target file: %s\n", 
			redir_node->next->args ? redir_node->next->args[0] : "NULL");
		set_redir_node(redir_node, cmd, redir_node->next);
		fprintf(stderr, "DEBUG-SET-REDIR: Linking redir %p to cmd %p and target %p\n", 
			(void*)redir_node, (void*)cmd, (void*)redir_node->next);
		link_prev_redirs(redir_node, cmd, vars);
		track_redirs(redir_node, cmd, vars);
		link_in_out_redirs(vars);
		collect_args_after_redir(redir_node, cmd);
		if (!vars->pipes->redir_root)
		{
			vars->pipes->redir_root = redir_node;
			fprintf(stderr, "DEBUG-REDIR: Setting redirection root to node %p\n", (void*)redir_node);
		}
	}
}
