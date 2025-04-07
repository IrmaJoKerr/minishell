/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/04/07 11:31:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// /*
// Updates pipe structure when commands are redirected.
// - Traverses pipe chain looking for references to the command.
// - Replaces command references with redirection node references.
// - Preserves pipe structure while incorporating redirections.
// - Handles both left and right side command replacements.
// Works with proc_redir_pt2().
// */
// void	upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir)
// {
// 	t_node	*pipe_node;

// 	if (!pipe_root || !cmd || !redir)
// 		return ;
// 	pipe_node = pipe_root;
// 	while (pipe_node)
// 	{
// 		if (pipe_node->left == cmd)
// 			pipe_node->left = redir;
// 		else if (pipe_node->right == cmd)
// 			pipe_node->right = redir;
// 		pipe_node = pipe_node->right;
// 		if (pipe_node && pipe_node->type != TYPE_PIPE)
// 			break ;
// 	}
// }

// /*
// Determines if a redirection node has valid adjacent commands.
// - Checks if next node exists and is a command.
// Returns:
// - 1 if redirection has valid syntax.
// - 0 otherwise.
// Works with proc_redir_pt1().
// */
// int	is_valid_redir_node(t_node *current)
// {
// 	if (!current)
// 		return (0);
// 	if (!is_redirection(current->type))
// 		return (0);
// 	 // Modified to accept ARGS type for redirection targets
// 	if (!current->next || (current->next->type != TYPE_CMD && current->next->type != TYPE_ARGS))
// 		return (0);
// 	return (1);
// }

/*
Gets target command for redirection.
- Uses previous command if it exists and is a command node.
- Otherwise uses last tracked command.
Returns:
- Pointer to target command node.
- NULL if no suitable command found.
Works with proc_redir_pt1().
*/
// t_node	*get_redir_target(t_node *current, t_node *last_cmd)
// {
// 	t_node	*target;

// 	target = NULL;
// 	if (current->prev && current->prev->type == TYPE_CMD)
// 		target = current->prev;
// 	else
// 		target = last_cmd;
// 	return (target);
// }


/*
Processes the first part of redirection nodes identification.
- Traverses token list to find redirection operators.
- Records last command seen for reference.
- Identifies redirection targets.
Returns:
- First valid redirection node if no pipe root exists.
- NULL otherwise or if no valid redirections found.
Works with proc_token_list().
*/
// t_node	*proc_redir_pt1(t_vars *vars, t_node *pipe_root)
// {
// 	t_node	*current;
// 	t_node	*last_cmd;
// 	t_node	*redir_root;
// 	t_node	*target_cmd;

// 	current = vars->head;
// 	last_cmd = NULL;
// 	redir_root = NULL;
// 	(void)pipe_root;
	
// 	DBG_PRINTF(DEBUG_EXEC, "Starting redirection processing (proc_redir_pt1)\n");
	
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 		{
// 			last_cmd = current;
// 			DBG_PRINTF(DEBUG_EXEC, "Found command node: %s\n", 
// 					  current->args ? current->args[0] : "NULL");
// 		}
// 		else if (is_valid_redir_node(current))
// 		{
// 			DBG_PRINTF(DEBUG_EXEC, "Found valid redirection node type=%d (%s)\n", 
// 					  current->type, get_token_str(current->type));
			
// 			target_cmd = get_redir_target(current, last_cmd);
			
// 			DBG_PRINTF(DEBUG_EXEC, "Target command for redirection: %p\n", (void*)target_cmd);
// 			if (target_cmd && target_cmd->args)
// 				DBG_PRINTF(DEBUG_EXEC, "Target command content: %s\n", target_cmd->args[0]);
			
// 			if (target_cmd)
// 			{
// 				DBG_PRINTF(DEBUG_EXEC, "Setting up redirection AST: %s %s %s\n",
// 						  target_cmd->args ? target_cmd->args[0] : "NULL",
// 						  get_token_str(current->type),
// 						  current->next && current->next->args ? current->next->args[0] : "NULL");
				
// 				set_redir_node(current, target_cmd, current->next);
// 				if (!redir_root)
// 				{
// 					redir_root = current;
// 					DBG_PRINTF(DEBUG_EXEC, "Set redir_root to node %p\n", (void*)redir_root);
// 				}
// 			}
// 		}
// 		current = current->next;
// 	}
	
// 	DBG_PRINTF(DEBUG_EXEC, "Completed redirection processing, returning root: %p\n", (void*)redir_root);
	
// 	return (redir_root);
// }
/*
Processes the first part of redirection nodes identification.
- Traverses token list to find redirection operators.
- Records last command seen in vars->pipes structure.
- Identifies redirection targets.
Returns:
- First valid redirection node if found.
- NULL if no valid redirections found or on error.
Works with proc_token_list().
*/
// t_node *proc_redir_pt1(t_vars *vars)
// {
//     t_node *current;

//     if (!vars || !vars->head || !vars->pipes)
//         return (NULL);
//     current = vars->head;
//     vars->pipes->last_cmd = NULL;
//     vars->pipes->redir_root = NULL;
//     DBG_PRINTF(DEBUG_EXEC, "Starting redirection processing (proc_redir_pt1)\n");
//     while (current)
//     {
//         if (current->type == TYPE_CMD)
//         {
//             vars->pipes->last_cmd = current;
//             DBG_PRINTF(DEBUG_EXEC, "Found command node: %s\n", 
//                       current->args ? current->args[0] : "NULL");
//         }
//         else if (is_valid_redir_node(current))
//         {
//             DBG_PRINTF(DEBUG_EXEC, "Found valid redirection node type=%d (%s)\n", 
//                       current->type, get_token_str(current->type));
                      
//             vars->pipes->cmd_redir = get_redir_target(current, vars->pipes->last_cmd);
//             DBG_PRINTF(DEBUG_EXEC, "Target command for redirection: %p\n", (void*)vars->pipes->cmd_redir);
//             if (vars->pipes->cmd_redir && vars->pipes->cmd_redir->args)
//                 DBG_PRINTF(DEBUG_EXEC, "Target command content: %s\n", vars->pipes->cmd_redir->args[0]);
//             if (vars->pipes->cmd_redir)
//             {
//                 DBG_PRINTF(DEBUG_EXEC, "Setting up redirection AST: %s %s %s\n",
//                           vars->pipes->cmd_redir->args ? vars->pipes->cmd_redir->args[0] : "NULL",
//                           get_token_str(current->type),
//                           current->next && current->next->args ? current->next->args[0] : "NULL");
                
//                 set_redir_node(current, vars->pipes->cmd_redir, current->next);
//                 if (!vars->pipes->redir_root)
//                 {
//                     vars->pipes->redir_root = current;
//                     DBG_PRINTF(DEBUG_EXEC, "Set redir_root to node %p\n", (void*)vars->pipes->redir_root);
//                 }
//             }
//         }
//         current = current->next;
//     }
//     DBG_PRINTF(DEBUG_EXEC, "Completed redirection processing, returning root: %p\n", 
//               (void*)vars->pipes->redir_root);
    
//     return (vars->pipes->redir_root);
// }

/*
Processes the second part of redirection nodes handling.
- Updates pipe structure with redirection references.
- Links redirections into existing pipe hierarchy.
- Only modifies pipe structure if redirection affects piped commands.
Works with proc_token_list().
*/
// void	proc_redir_pt2(t_vars *vars, t_node *pipe_root)
// {
// 	t_node	*current;
// 	t_node	*target_cmd;
// 	t_node	*last_cmd;

// 	current = vars->head;
// 	last_cmd = NULL;
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 			last_cmd = current;
// 		else if (is_valid_redir_node(current))
// 		{
// 			target_cmd = get_redir_target(current, last_cmd);
// 			if (target_cmd && pipe_root)
// 				upd_pipe_redir(pipe_root, target_cmd, current);
// 		}
// 		current = current->next;
// 	}
// }
/*
Processes the second part of redirection nodes handling.
- Updates pipe structure with redirection references.
- Links redirections into existing pipe hierarchy.
- Only modifies pipe structure if redirection affects piped commands.
Works with proc_token_list().
*/
// void proc_redir_pt2(t_vars *vars)
// {
//     t_node *current;
//     t_node *target_cmd;  // Need to keep this as local temporary variable

//     if (!vars || !vars->head || !vars->pipes || !vars->pipes->pipe_root)
//         return;
        
//     current = vars->head;
//     vars->pipes->last_cmd = NULL;
    
//     while (current)
//     {
//         if (current->type == TYPE_CMD)
//             vars->pipes->last_cmd = current;
//         else if (is_valid_redir_node(current))
//         {
//             // Find target command for this redirection
//             target_cmd = get_redir_target(current, vars->pipes->last_cmd);
            
//             // Update pipe structure to include this redirection
//             if (target_cmd && vars->pipes->pipe_root)
//                 upd_pipe_redir(vars->pipes->pipe_root, target_cmd, current);
//         }
//         current = current->next;
//     }
// }

/*
Processes the entire token list to build the AST structure.
- Identifies command nodes in the token list.
- Builds pipe structures for commands separated by pipes.
- Processes redirections and integrates with pipe structure.
- Selects the appropriate root node for the final AST.

Example: For input "ls -l | grep a > output.txt":
- First identifies command nodes "ls" and "grep"
- Processes pipe to connect them
- Processes redirection to output.txt
- Returns pipe node as root, with redirection integrated

Returns:
- Root node of the constructed AST.
- NULL if invalid syntax or no commands found.
Works with build_and_execute.
*/
t_node *proc_token_list(t_vars *vars)
{
    // Basic validation
    if (!vars || !vars->head || !vars->pipes)
        return (NULL);
    // Find all command nodes
    find_cmd(NULL, NULL, FIND_ALL, vars);
    // Verify at least one valid command exists
    if (vars->cmd_count == 0 || !vars->cmd_nodes[0] || !vars->cmd_nodes[0]->args)
        return (NULL);
    // Initialize AST root tracking
    vars->pipes->pipe_root = NULL;
    vars->pipes->redir_root = NULL;
    // Process all pipe nodes
    vars->pipes->pipe_root = proc_pipes(vars);
    // Process redirections with our new unified function
    //DBG_PRINTF(DEBUG_EXEC, "Processing redirections: pipe_root=%p\n", 
    //          (void*)vars->pipes->pipe_root);  
    vars->pipes->redir_root = proc_redir(vars);
    //DBG_PRINTF(DEBUG_EXEC, "After redirection processing: redir_root=%p\n", 
    //          (void*)vars->pipes->redir_root);
    // Select appropriate root node for the AST
    //DBG_PRINTF(DEBUG_EXEC, "Final AST root selection: pipe_root=%p, redir_root=%p\n", 
    //          (void*)vars->pipes->pipe_root, (void*)vars->pipes->redir_root);
    if (vars->pipes->pipe_root)
        return vars->pipes->pipe_root;
    else if (vars->pipes->redir_root)
        return vars->pipes->redir_root;
    else if (vars->cmd_count > 0)
        return vars->cmd_nodes[0];
    return NULL;
}

/*
Converts string tokens after pipes to command tokens.
*/
void	convert_strs_to_cmds(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current && current->next)
	{
		if (current->type == TYPE_PIPE && current->next->type == TYPE_ARGS)
			current->next->type = TYPE_CMD;
		current = current->next;
	}
}

// /*
// Attaches string tokens as arguments to their commands.
// */
// void	link_strargs_to_cmds(t_vars *vars)
// {
// 	t_node	*current;
// 	t_node	*cmd_node;

// 	current = vars->head;
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 			cmd_node = current;
// 		else if (cmd_node && current->type == TYPE_ARGS)
// 		{
// 			// Append the argument to the command node
// 			if (cmd_node->args && current->args && current->args[0])
// 			{
// 				cmd_node->args[0] = ft_strdup(current->args[0]);
// 			}
// 		}
// 		current = current->next;
// 	}
// }

// /*
// Connects an additional pipe in the pipe chain.
// */
// void	link_addon_pipe(t_node *last_pipe, t_node *new_pipe, t_node *right_cmd)
// {
// 	t_node	*prev_right;

// 	prev_right = last_pipe->right;
// 	last_pipe->right = new_pipe;
// 	new_pipe->left = prev_right;
// 	new_pipe->right = right_cmd;
// }

// /*
// Creates a tree structure for pipe commands.
// */
// void	build_pipe_ast(t_vars *vars)
// {
// 	t_node	*current;
// 	t_node	*last_pipe;
// 	t_node	*cmd_before;
// 	t_node	*cmd_after;

// 	last_pipe = NULL;
// 	cmd_before = NULL;
// 	current = vars->head;
// 	vars->astroot = NULL;
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 			cmd_before = current;
// 		else if (current->type == TYPE_PIPE)
// 		{
// 			cmd_after = find_cmd(current->next, NULL, FIND_NEXT, NULL);
// 			if (!vars->astroot)
// 			{
// 				vars->astroot = current;
// 				if (cmd_before)
// 					vars->astroot->left = cmd_before;
// 				if (cmd_after)
// 					vars->astroot->right = cmd_after;
// 				last_pipe = vars->astroot;
// 			}
// 			else if (last_pipe)
// 			{
// 				link_addon_pipe(last_pipe, current, cmd_after);
// 				last_pipe = current;
// 			}
// 		}
// 		current = current->next;
// 	}
// 	if (!vars->astroot)
// 	{
// 		if (vars->head && vars->head->type == TYPE_CMD)
// 			vars->astroot = vars->head;
// 	}
// }
