/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 02:04:09 by bleow            ###   ########.fr       */
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
// t_node	*proc_token_list(t_vars *vars)
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
// 	verify_command_args(vars);
// 	if (vars->pipes->pipe_root)
// 		return (vars->pipes->pipe_root);
// 	else if (vars->pipes->redir_root)
// 		return (vars->pipes->redir_root);
// 	else if (vars->cmd_count > 0)
// 		return (vars->cmd_nodes[0]);
// 	return (NULL);
// }
// t_node	*proc_token_list(t_vars *vars)
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
// 	verify_command_args(vars);
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
// void	build_redir_ast(t_vars *vars)
// {
// 	t_node	*current;

// 	current = vars->head;
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 		{
// 			vars->pipes->last_cmd = current;
// 		}
// 		else if (is_redirection(current->type))
// 		{
// 			if (!is_valid_redir_node(current) || !current->next->args
// 				|| !current->next->args[0])
// 			{
// 				tok_syntax_error_msg("newline", vars);
// 				return ;
// 			}
// 			process_redir_node(current, vars);
// 		}
// 		current = current->next;
// 	}
// }
// void	build_redir_ast(t_vars *vars)
// {
// 	t_node	*current;

// 	current = vars->head;
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 		{
// 			vars->pipes->last_cmd = current;
// 		}
// 		else if (is_redirection(current->type))
// 		{
// 			if (!current->args || !current->args[0])
// 			{
// 				tok_syntax_error_msg("newline", vars);
// 				return ;
// 			}
// 			process_redir_node(current, vars);
// 		}
// 		current = current->next;
// 	}
// }
// void build_redir_ast(t_vars *vars)
// {
//     int i;
    
//     fprintf(stderr, "DEBUG-REDIR-BUILD: Building redirection AST (command-first approach)\n");
    
//     // First initialize any redirection nodes that have targets
//     t_node *current = vars->head;
//     while (current) {
//         if (is_redirection(current->type)) {
//             // Make sure redirection node has a valid structure
//             if (!is_valid_redir_node(current) || !current->next || 
//                 !current->next->args || !current->next->args[0]) {
//                 tok_syntax_error_msg("newline", vars);
//                 return;
//             }
            
//             // Initialize redirection target (command will be set later)
//             if (!vars->pipes->redir_root)
//                 vars->pipes->redir_root = current;
//         }
//         current = current->next;
//     }
    
//     // Process each command and find its redirections
//     for (i = 0; i < vars->cmd_count; i++) {
//         if (vars->cmd_nodes[i]) {
//             fprintf(stderr, "DEBUG-REDIR-BUILD: Processing command '%s'\n",
//                     vars->cmd_nodes[i]->args ? vars->cmd_nodes[i]->args[0] : "NULL");
            
//             // Link all redirections that belong to this command
//             link_cmd_redirs(vars->cmd_nodes[i], vars);
//         }
//     }
// }
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

void	verify_command_args(t_vars *vars)
{
	t_node	*current;
	t_node	*cmd;
	t_node	**cmd_ptr;
	int		remaining_cmds;

	cmd_ptr = vars->cmd_nodes;
	remaining_cmds = vars->cmd_count;
	while (remaining_cmds > 0)
	{
		cmd = *cmd_ptr;
		if (!cmd || !cmd->args)
		{
			cmd_ptr++;
			remaining_cmds--;
			continue ;
		}
		current = find_node_in_list(vars->head, cmd);
		if (!current)
		{
			cmd_ptr++;
			remaining_cmds--;
			continue ;
		}
		current = current->next;
		while (current && current->type != TYPE_CMD
			&& current->type != TYPE_PIPE)
		{
			if (current->type == TYPE_ARGS
				&& !is_redirection_target(current, vars))
			{
				if (!is_arg_in_cmd(cmd, current->args[0]))
					append_arg(cmd, current->args[0], 0);
			}
			current = current->next;
		}
		cmd_ptr++;
		remaining_cmds--;
	}
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
// 	verify_command_args(vars);
// 	if (vars->pipes->pipe_root)
// 		link_redirs_pipes(vars);
// 	if (vars->pipes->redir_root)
// 	{
// 		vars->astroot = vars->pipes->redir_root;
// 	}
// 	return (vars->pipes->redir_root);
// }
// t_node *proc_redir(t_vars *vars)
// {
//     if (!vars || !vars->head)
//         return (NULL);
    
//     reset_redir_tracking(vars->pipes);
    
//     // Build the redirection AST with new forward command-based approach
//     build_redir_ast(vars);
    
//     // If we also have pipe structure, integrate redirections with it
//     if (vars->pipes->pipe_root)
//         link_redirs_pipes(vars);
    
//     return (vars->pipes->redir_root);
// }
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

// void	collect_args_after_redir(t_node *redir_node, t_node *cmd_node)
// {
// 	t_node	*arg_node;

// 	if (!redir_node || !cmd_node)
// 		return ;
// 	// Start from the node after the redirection
// 	arg_node = redir_node->next;
// 	// Collect all consecutive ARGS tokens until we hit something else
// 	while (arg_node && arg_node->type == TYPE_ARGS && arg_node->args)
// 	{
// 		// Skip this argument if it's already in the command's args
// 		if (is_arg_in_cmd(cmd_node, arg_node->args[0]))
// 		{
// 			arg_node = arg_node->next;
// 			continue ;
// 		}
// 		// Append this argument to the command
// 		append_arg(cmd_node, arg_node->args[0], 0);
// 		// Move to the next node
// 		arg_node = arg_node->next;
// 	}
// }
// void collect_args_after_redir(t_node *redir_node, t_node *cmd_node)
// {
//     t_node *curr = redir_node;
	
//     fprintf(stderr, "DEBUG-ARG-COLLECT: Collecting arguments after redirections for '%s'\n", 
//             cmd_node->args ? cmd_node->args[0] : "NULL");
	
//     // Find all ARGS nodes that follow and should be part of this command
//     while (curr && curr->next)
//     {
//         curr = curr->next;
		
//         // If we hit another redirection or pipe, stop
//         if (is_redirection(curr->type) || curr->type == TYPE_PIPE)
//             break;
			
//         // If we find an ARGS node, append it to the command
//         if (curr->type == TYPE_ARGS && curr->args && curr->args[0])
//         {
//             fprintf(stderr, "DEBUG-ARG-COLLECT: Adding argument '%s' to command\n", 
//                     curr->args[0]);
//             append_arg(cmd_node, curr->args[0], 0);
//         }
//     }
	
//     fprintf(stderr, "DEBUG-ARG-COLLECT: Command arguments after collection: ");
//     if (cmd_node->args)
//     {
//         int i = 0;
//         while (cmd_node->args[i])
//         {
//             fprintf(stderr, "'%s' ", cmd_node->args[i]);
//             i++;
//         }
//     }
//     fprintf(stderr, "\n");
// }
// void collect_args_after_redir(t_node *redir_node, t_node *cmd_node)
// {
//     t_node *curr = redir_node;
//     int args_collected = 0;
	
//     fprintf(stderr, "DEBUG-ARG-COLLECT: Starting collection for cmd '%s' after redir '%s'\n", 
//             cmd_node->args ? cmd_node->args[0] : "NULL",
//             redir_node->args ? redir_node->args[0] : "NULL");
	
//     // Print current command arguments before collection
//     fprintf(stderr, "DEBUG-ARG-COLLECT: Command args before collection: ");
//     if (cmd_node->args) {
//         int i = 0;
//         while (cmd_node->args[i]) {
//             fprintf(stderr, "'%s' ", cmd_node->args[i]);
//             i++;
//         }
//     }
//     fprintf(stderr, "\n");
	
//     // Find all ARGS nodes that follow and should be part of this command
//     while (curr && curr->next)
//     {
//         curr = curr->next;
		
//         fprintf(stderr, "DEBUG-ARG-COLLECT: Examining node type=%s, content='%s'\n",
//                 get_token_str(curr->type), 
//                 curr->args ? curr->args[0] : "NULL");
		
//         // If we hit another redirection or pipe, stop
//         if (is_redirection(curr->type) || curr->type == TYPE_PIPE) {
//             fprintf(stderr, "DEBUG-ARG-COLLECT: Stopping at %s\n", get_token_str(curr->type));
//             break;
//         }
			
//         // If we find an ARGS node, append it to the command
//         if (curr->type == TYPE_ARGS && curr->args && curr->args[0])
//         {
//             fprintf(stderr, "DEBUG-ARG-COLLECT: Collecting argument #%d: '%s'\n", 
//                     args_collected, curr->args[0]);
//             append_arg(cmd_node, curr->args[0], 0);
//             args_collected++;
//         }
//     }
	
//     // Print final command arguments after collection
//     fprintf(stderr, "DEBUG-ARG-COLLECT: Command args after collection (%d collected): ", args_collected);
//     if (cmd_node->args) {
//         int i = 0;
//         while (cmd_node->args[i]) {
//             fprintf(stderr, "'%s' ", cmd_node->args[i]);
//             i++;
//         }
//     }
//     fprintf(stderr, "\n");
// }
// void collect_args_after_redir(t_node *redir_node, t_node *cmd_node)
// {
// 	t_node *curr = redir_node;
// 	int args_collected = 0;
	
// 	// New debug prints - start
// 	fprintf(stderr, "DEBUG-ARG-COLLECT: Starting collection for cmd '%s' after redir '%s'\n", 
// 			cmd_node->args ? cmd_node->args[0] : "NULL",
// 			redir_node->args ? redir_node->args[0] : "NULL");
	
// 	// Print current command arguments before collection
// 	fprintf(stderr, "DEBUG-ARG-COLLECT: Command args before collection: ");
// 	if (cmd_node->args) {
// 		int i = 0;
// 		while (cmd_node->args[i]) {
// 			fprintf(stderr, "'%s' ", cmd_node->args[i]);
// 			i++;
// 		}
// 	}
// 	fprintf(stderr, "\n");
	
// 	// Find all ARGS nodes that follow and should be part of this command
// 	while (curr && curr->next)
// 	{
// 		curr = curr->next;
		
// 		// New debug print - examining each node
// 		fprintf(stderr, "DEBUG-ARG-COLLECT: Examining node type=%s, content='%s'\n",
// 				get_token_str(curr->type), 
// 				curr->args ? curr->args[0] : "NULL");
		
// 		// If we hit another redirection or pipe, stop
// 		if (is_redirection(curr->type) || curr->type == TYPE_PIPE) {
// 			fprintf(stderr, "DEBUG-ARG-COLLECT: Stopping at %s\n", get_token_str(curr->type));
// 			break;
// 		}
			
// 		// If we find an ARGS node, append it to the command
// 		if (curr->type == TYPE_ARGS && curr->args && curr->args[0])
// 		{
// 			fprintf(stderr, "DEBUG-ARG-COLLECT: Collecting argument #%d: '%s'\n", 
// 					args_collected, curr->args[0]);
// 			append_arg(cmd_node, curr->args[0], 0);
// 			args_collected++;
// 		}
// 	}
	
// 	// Print final command arguments after collection
// 	fprintf(stderr, "DEBUG-ARG-COLLECT: Command args after collection (%d collected): ", args_collected);
// 	if (cmd_node->args) {
// 		int i = 0;
// 		while (cmd_node->args[i]) {
// 			fprintf(stderr, "'%s' ", cmd_node->args[i]);
// 			i++;
// 		}
// 	}
// 	fprintf(stderr, "\n");
// }
// void collect_args_after_redir(t_node *redir_node, t_node *cmd_node)
// {
//     t_node *current;
    
//     fprintf(stderr, "DEBUG-COLLECT-ARGS: Collecting arguments after redirection for cmd '%s'\n",
//             cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     if (!redir_node || !redir_node->next || !cmd_node)
//         return;
    
//     // Start from the node after the redirection's target
//     current = redir_node->next;
    
//     // If the next node is the filename for the redirection, skip it
//     if (current && (current->type == TYPE_ARGS || current->type == TYPE_CMD)) {
//         current = current->next;
//     }
    
//     // Continue collecting arguments until we hit another redirection or pipe
//     while (current && current->type == TYPE_ARGS &&
//            !is_redirection(current->type) && current->type != TYPE_PIPE) {
        
//         fprintf(stderr, "DEBUG-COLLECT-ARGS: Found argument '%s' after redirection\n",
//                 current->args ? current->args[0] : "NULL");
        
//         // Append this argument to the command
//         if (current->args && current->args[0]) {
//             append_arg(cmd_node, current->args[0], 0);
//             fprintf(stderr, "DEBUG-COLLECT-ARGS: Appended '%s' to command\n", 
//                     current->args[0]);
//         }
        
//         current = current->next;
//     }
// }
// void collect_args_after_redir(t_node *redir_node, t_node *cmd_node)
// {
//     t_node *current;
    
//     fprintf(stderr, "DEBUG-COLLECT-ARGS: Starting collection for cmd '%s'\n",
//             cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     if (!redir_node || !redir_node->next || !cmd_node)
//         return;
    
//     // Start from the node after the redirection's target
//     current = redir_node->next;
    
//     // If the next node is the filename for the redirection, skip it
//     if (current && (current->type == TYPE_ARGS || current->type == TYPE_CMD)) {
//         current = current->next;
//     }
    
//     // CRITICAL FIX: Continue collecting ALL arguments until we hit a pipe or redirection
//     while (current) {
//         // Stop at pipe or redirection
//         if (current->type == TYPE_PIPE || is_redirection(current->type))
//             break;
            
//         // Only collect arguments
//         if (current->type == TYPE_ARGS && current->args && current->args[0]) {
//             fprintf(stderr, "DEBUG-COLLECT-ARGS: Found argument '%s' after redirection\n",
//                     current->args[0]);
            
//             append_arg(cmd_node, current->args[0], 0);
//             fprintf(stderr, "DEBUG-COLLECT-ARGS: Appended '%s' to command, now has %d args\n", 
//                     current->args[0], (int)ft_arrlen(cmd_node->args)-1);
//         }
        
//         current = current->next;
//     }
    
//     fprintf(stderr, "DEBUG-COLLECT-ARGS: Finished collecting args, final count: %d\n", 
//             (int)ft_arrlen(cmd_node->args)-1);
// }

/*
Master control function for processing an individual redirection node.
- Finds the target command for the redirection
- Links the redirection node to command and target
- Chains with previous redirections
- Updates type-specific redirection trackers
- Sets the redirection root if this is the first redirection
Works with build_redir_ast() during AST construction.
*/
// void process_redir_node(t_node *redir_node, t_vars *vars)
// {
//     t_node *cmd;

//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Processing redirection type=%s, filename='%s'\n",
//             get_token_str(redir_node->type), redir_node->args ? redir_node->args[0] : "NULL");
//     cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
//     if (!cmd)
//     {
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: No command target found\n");
//         return;
//     }
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Target command is '%s'\n",
//             cmd->args ? cmd->args[0] : "NULL");
//     // Directly set the left pointer to the command
//     redir_node->left = cmd;
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Set left pointer to command '%s'\n",
//             cmd->args ? cmd->args[0] : "NULL");
//     // Link with previous redirections targeting same command
//     link_prev_redirs(redir_node, cmd, vars);
//     // Track redirection type (IN or OUT)
//     track_redirs(redir_node, cmd, vars);
//     // Link input and output redirections if they target the same command
//     link_in_out_redirs(vars);
//     // Collect any arguments that follow this redirection
//     collect_args_after_redir(redir_node, cmd);
//     // Set the redirection root if this is the first one
//     if (!vars->pipes->redir_root)
//     {
//         vars->pipes->redir_root = redir_node;
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: Set redir_root to redirection with filename '%s'\n",
//                 redir_node->args ? redir_node->args[0] : "NULL");
//     }
// }
// void process_redir_node(t_node *redir_node, t_vars *vars)
// {
//     t_node *cmd;
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Processing redirection type=%s, filename='%s'\n", 
//             get_token_str(redir_node->type),
//             redir_node->args ? redir_node->args[0] : "NULL");
//     cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Target command is '%s'\n", 
//             cmd && cmd->args ? cmd->args[0] : "NULL");
//     // Set the redirection's left pointer to the command
//     redir_node->left = cmd;
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Set left pointer to command '%s'\n", 
//             cmd && cmd->args ? cmd->args[0] : "NULL");
//     // Link with previous redirections targeting the same command
//     link_prev_redirs(redir_node, cmd, vars);
//     // Check for arguments after the redirection
//     collect_args_after_redir(redir_node, cmd);
//     // Set the first redirection as the root if not already set
//     if (!vars->pipes->redir_root)
//     {
//         vars->pipes->redir_root = redir_node;
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: Set redir_root to redirection with filename '%s'\n", 
//                 redir_node->args ? redir_node->args[0] : "NULL");
//     }
//     // Update redirection tracking based on type
//     track_redirs(redir_node, cmd, vars);
// }
// void	process_redir_node(t_node *redir_node, t_vars *vars)
// {
// 	t_node	*cmd;

// 	cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
// 	// Set the redirection's left pointer to the command
// 	redir_node->left = cmd;
// 	// Link with previous redirections targeting the same command
// 	link_prev_redirs(redir_node, cmd, vars);
// 	// Check for arguments after the redirection and collect them
// 	collect_args_after_redir(redir_node, cmd);
// 	// Set the first redirection as the root if not already set
// 	if (!vars->pipes->redir_root)
// 	{
// 		vars->pipes->redir_root = redir_node;
// 	}
// 	// Update redirection tracking based on type
// 	track_redirs(redir_node, cmd, vars);
// }
// void process_redir_node(t_node *redir_node, t_vars *vars)
// {
//     t_node *cmd;

//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Processing redirection type=%s, filename='%s'\n",
//             get_token_str(redir_node->type), redir_node->args ? redir_node->args[0] : "NULL");
	
//     cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
//     if (!cmd)
//     {
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: No command target found\n");
//         return;
//     }
	
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Target command is '%s'\n",
//             cmd->args ? cmd->args[0] : "NULL");
	
//     // Directly set the left pointer to the command
//     redir_node->left = cmd;
	
//     // Collect any arguments that follow this redirection and should be part of the command
//     collect_args_after_redir(redir_node, cmd);
	
//     // Set pipes->redir_root if this is the first redirection
//     if (!vars->pipes->redir_root)
//     {
//         vars->pipes->redir_root = redir_node;
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: Set redir_root to this node\n");
//     }
	
//     // Track redirection type
//     track_redirs(redir_node, cmd, vars);
	
//     // Link with previous redirections (if they exist)
//     link_prev_redirs(redir_node, cmd, vars);
// }
// void process_redir_node(t_node *redir_node, t_vars *vars)
// {
// 	t_node *cmd;

// 	fprintf(stderr, "DEBUG-PROCESS-REDIR: Processing redirection type=%s, filename='%s'\n",
// 			get_token_str(redir_node->type), redir_node->args ? redir_node->args[0] : "NULL");
	
// 	cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
// 	if (!cmd)
// 	{
// 		fprintf(stderr, "DEBUG-PROCESS-REDIR: No command target found\n");
// 		return ;
// 	}
	
// 	fprintf(stderr, "DEBUG-PROCESS-REDIR: Target command is '%s'\n",
// 			cmd->args ? cmd->args[0] : "NULL");
	
// 	// Directly set the left pointer to the command
// 	redir_node->left = cmd;
	
// 	// CRITICAL FIX: Mark this redirection as belonging to this specific command
// 	// This helps find_cmd_redir identify which side of a pipe the redirection belongs to
// 	redir_node->redir = cmd;
// 	fprintf(stderr, "DEBUG-PROCESS-REDIR: Marked redirection as belonging to command '%s'\n",
// 			cmd->args ? cmd->args[0] : "NULL");
	
// 	// Collect any arguments that follow this redirection and should be part of the command
// 	collect_args_after_redir(redir_node, cmd);
	
// 	// Set pipes->redir_root if this is the first redirection
// 	if (!vars->pipes->redir_root)
// 	{
// 		vars->pipes->redir_root = redir_node;
// 		fprintf(stderr, "DEBUG-PROCESS-REDIR: Set redir_root to this node\n");
// 	}
	
// 	// Track redirection type
// 	track_redirs(redir_node, cmd, vars);
	
// 	// Link with previous redirections (if they exist)
// 	link_prev_redirs(redir_node, cmd, vars);
// }
// void process_redir_node(t_node *redir_node, t_vars *vars)
// {
//     t_node *cmd;

//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Processing redirection type=%s, filename='%s'\n",
//             get_token_str(redir_node->type), redir_node->args ? redir_node->args[0] : "NULL");
    
//     cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
//     if (!cmd)
//     {
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: No command target found\n");
//         return;
//     }
    
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Target command is '%s'\n",
//             cmd->args ? cmd->args[0] : "NULL");
    
//     // Directly set the left pointer to the command
//     redir_node->left = cmd;
    
//     // Mark this redirection as belonging to this specific command
//     redir_node->redir = cmd;
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Marked redirection as belonging to command '%s'\n",
//             cmd->args ? cmd->args[0] : "NULL");
    
//     // Collect any arguments that follow this redirection and should be part of the command
//     collect_args_after_redir(redir_node, cmd);
    
//     // Set pipes->redir_root if this is the first redirection
//     if (!vars->pipes->redir_root)
//     {
//         vars->pipes->redir_root = redir_node;
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: Set redir_root to this node\n");
//     }
    
//     // Track redirection type
//     track_redirs(redir_node, cmd, vars);
    
//     // We don't call link_prev_redirs anymore
//     // Command-based linking happens separately in link_cmd_redirs
// }
// void process_redir_node(t_node *redir_node, t_vars *vars)
// {
//     t_node *cmd;

//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Processing redirection type=%s, filename='%s'\n",
//             get_token_str(redir_node->type), redir_node->args ? redir_node->args[0] : "NULL");
    
//     cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
//     if (!cmd)
//     {
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: No command target found\n");
//         return;
//     }
    
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Target command is '%s'\n",
//             cmd->args ? cmd->args[0] : "NULL");
    
//     // Directly set the left pointer to the command
//     redir_node->left = cmd;
    
//     // Mark this redirection as belonging to this specific command
//     redir_node->redir = cmd;
//     fprintf(stderr, "DEBUG-PROCESS-REDIR: Marked redirection as belonging to command '%s'\n",
//             cmd->args ? cmd->args[0] : "NULL");
    
//     // Collect any arguments that follow this redirection and should be part of the command
//     collect_args_after_redir(redir_node, cmd);
    
//     // Set pipes->redir_root if this is the first redirection
//     if (!vars->pipes->redir_root)
//     {
//         vars->pipes->redir_root = redir_node;
//         fprintf(stderr, "DEBUG-PROCESS-REDIR: Set redir_root to this node\n");
//     }
    
//     // Track redirection type
//     track_redirs(redir_node, cmd, vars);
    
//     // We don't call link_prev_redirs anymore
//     // Command-based linking happens separately in link_cmd_redirs
// }
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

/*
Processes a redirection node during AST construction.
- Sets up connections between command, redirection, and target nodes.
- Updates pipeline tracking structures for later processing.
Works with build_redir_ast().
*/
// void	process_redir_node(t_node *redir_node, t_vars *vars)
// {
//     t_node	*cmd_node;
//     t_node	*target_node;

//     fprintf(stderr, "REDIR-DBG-PROC: Processing redirection type=%s, filename='%s'\n", 
//             get_token_str(redir_node->type),
//             redir_node->next && redir_node->next->args ? redir_node->next->args[0] : "NULL");
	
//     if (!vars->pipes->redir_root)
//     {
//         fprintf(stderr, "REDIR-DBG-PROC: Set redir_root to this node\n");
//         vars->pipes->redir_root = redir_node;
//     }
	
//     cmd_node = get_redir_target(redir_node, vars->pipes->last_cmd);
//     if (!cmd_node)
//     {
//         fprintf(stderr, "REDIR-DBG-PROC: No command target found\n");
//         return;
//     }
	
//     fprintf(stderr, "REDIR-DBG-PROC: Target command is '%s'\n", 
//             cmd_node->args ? cmd_node->args[0] : "NULL");
	
//     target_node = redir_node->next;
//     if (!target_node)
//     {
//         fprintf(stderr, "REDIR-DBG-PROC: No target node found\n");
//         return;
//     }
	
//     set_redir_node(redir_node, cmd_node, target_node);
	
//     // CRITICAL FIX: Store the command's position in the token stream
//     // This helps identify which side of the pipe it belongs to
//     if (!redir_node->redir)
//     {
//         redir_node->redir = cmd_node;
//         fprintf(stderr, "REDIR-DBG-PROC: Marking redirection as belonging to command '%s'\n", 
//                 cmd_node->args ? cmd_node->args[0] : "NULL");
//     }
	
//     track_redirs(redir_node, cmd_node, vars);
// }
