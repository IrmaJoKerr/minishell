/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/05/04 21:23:09 by bleow            ###   ########.fr       */
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
// 	// if (DEBUG_AST) //DEBUG PRINT
// 	//	fprintf(stderr, "[DEBUG] Starting proc_token_list with error_code=%d\n", vars->error_code); //DEBUG PRINT
	
// 	if (!vars || !vars->head || !vars->pipes)
// 		return (NULL);
	
// 	find_cmd(NULL, NULL, FIND_ALL, vars);
	
// 	// if (DEBUG_AST) //DEBUG PRINT
// 	// { //DEBUG PRINT
// 	//	fprintf(stderr, "[DEBUG] Commands found: %d\n", vars->cmd_count); //DEBUG PRINT
// 	//	for (int i = 0; i < vars->cmd_count; i++) //DEBUG PRINT
// 	//	{ //DEBUG PRINT
// 	//		if (vars->cmd_nodes[i]->args) //DEBUG PRINT
// 	//			fprintf(stderr, "  Cmd %d: %s\n", i, vars->cmd_nodes[i]->args[0]); //DEBUG PRINT
// 	//	} //DEBUG PRINT
// 	// } //DEBUG PRINT
	
// 	if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
// 		|| !vars->cmd_nodes[0]->args)
// 		return (NULL);
// 	vars->pipes->pipe_root = NULL;
// 	vars->pipes->redir_root = NULL;
	
// 	// if (DEBUG_AST) //DEBUG PRINT
// 	//	fprintf(stderr, "[DEBUG] Processing pipes with error_code=%d\n", vars->error_code); //DEBUG PRINT
// 	vars->pipes->pipe_root = proc_pipes(vars);
	
// 	// if (DEBUG_AST) //DEBUG PRINT
// 	//	fprintf(stderr, "[DEBUG] Processing redirections with error_code=%d\n", vars->error_code); //DEBUG PRINT
// 	vars->pipes->redir_root = proc_redir(vars);
	
// 	// if (DEBUG_ERROR) //DEBUG PRINT
// 	//	fprintf(stderr, "[DEBUG] After redirection processing, error_code=%d\n", vars->error_code); //DEBUG PRINT
	
// 	// if (DEBUG_AST) //DEBUG PRINT
// 	// { //DEBUG PRINT
// 	//	fprintf(stderr, "[DEBUG] AST roots - Pipe: %p, Redir: %p\n",  //DEBUG PRINT
// 	//		   (void*)vars->pipes->pipe_root, (void*)vars->pipes->redir_root); //DEBUG PRINT
// 	// } //DEBUG PRINT
	
// 	if (vars->pipes->pipe_root)
// 		return (vars->pipes->pipe_root);
// 	else if (vars->pipes->redir_root)
// 		return (vars->pipes->redir_root);
// 	else if (vars->cmd_count > 0)
// 		return (vars->cmd_nodes[0]);
// 	// Debug print the entire AST
// 	 // Debug print the AST - change root to either NULL or something meaningful
// 	fprintf(stderr, "\n[DEBUG-AST] No valid AST could be constructed\n");
// 	return (NULL);
// 	return (NULL);
// }
// t_node  *proc_token_list(t_vars *vars)
// {
//     t_node *ast_root = NULL;
    
//     // Existing code...
    
//     vars->pipes->pipe_root = proc_pipes(vars);
//     vars->pipes->redir_root = proc_redir(vars);
    
//     // Attach any orphaned arguments to their commands
//     fprintf(stderr, "[DEBUG-AST] Attaching orphaned arguments\n");
//     attach_orphaned_args(vars);
    
//     // Determine which root to use
//     if (vars->pipes->pipe_root)
//         ast_root = vars->pipes->pipe_root;
//     else if (vars->pipes->redir_root)
//         ast_root = vars->pipes->redir_root;
//     else if (vars->cmd_count > 0)
//         ast_root = vars->cmd_nodes[0];
    
//     // Debug print the constructed AST
//     if (ast_root) {
//         fprintf(stderr, "\n[DEBUG-AST] Final Abstract Syntax Tree after orphan attachment:\n");
//         print_ast(ast_root, NULL);
//     } else {
//         fprintf(stderr, "\n[DEBUG-AST] No valid AST could be constructed\n");
//     }
    
//     return (ast_root);
// }
// t_node  *proc_token_list(t_vars *vars)
// {
//     t_node *ast_root = NULL;
    
//     if (!vars || !vars->head || !vars->pipes)
//         return (NULL);
    
//     // Debug print the token list before processing
//     fprintf(stderr, "\n[DEBUG-AST] Token list before AST construction:\n");
//     print_token_list(vars->head, NULL);
    
//     find_cmd(NULL, NULL, FIND_ALL, vars);
    
//     if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
//         || !vars->cmd_nodes[0]->args)
//         return (NULL);
//     vars->pipes->pipe_root = NULL;
//     vars->pipes->redir_root = NULL;
    
//     vars->pipes->pipe_root = proc_pipes(vars);
//     vars->pipes->redir_root = proc_redir(vars);
    
//     // Attach any orphaned arguments to their commands
//     attach_orphaned_args(vars);
    
//     // Determine which root to use
//     if (vars->pipes->pipe_root)
//         ast_root = vars->pipes->pipe_root;
//     else if (vars->pipes->redir_root)
//         ast_root = vars->pipes->redir_root;
//     else if (vars->cmd_count > 0)
//         ast_root = vars->cmd_nodes[0];
    
//     // Debug print the constructed AST
//     if (ast_root) {
//         fprintf(stderr, "\n[DEBUG-AST] Final Abstract Syntax Tree after orphan attachment:\n");
//         print_ast(ast_root, NULL);
//     } else {
//         fprintf(stderr, "\n[DEBUG-AST] No valid AST could be constructed\n");
//     }
    
//     return (ast_root);
// }
// t_node *proc_token_list(t_vars *vars)
// {
//     t_node *ast_root = NULL;
    
//     if (!vars || !vars->head || !vars->pipes)
//         return (NULL);
    
//     // Debug print the token list before processing
//     fprintf(stderr, "\n[DEBUG-AST] Token list before AST construction:\n");
//     print_token_list(vars->head, NULL);
    
//     find_cmd(NULL, NULL, FIND_ALL, vars);
    
//     if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
//         || !vars->cmd_nodes[0]->args)
//         return (NULL);
//     vars->pipes->pipe_root = NULL;
//     vars->pipes->redir_root = NULL;
    
//     vars->pipes->pipe_root = proc_pipes(vars);
//     vars->pipes->redir_root = proc_redir(vars);
    
//     // Attach any orphaned arguments to their commands
//     attach_orphaned_args(vars);
    
//     // Determine which root to use
//     if (vars->pipes->pipe_root)
//         ast_root = vars->pipes->pipe_root;
//     else if (vars->pipes->redir_root)
//         ast_root = vars->pipes->redir_root;
//     else if (vars->cmd_count > 0)
//         ast_root = vars->cmd_nodes[0];
    
//     // Debug print the constructed AST
//     if (ast_root) {
//         fprintf(stderr, "\n[DEBUG-AST] Final Abstract Syntax Tree after orphan attachment:\n");
//         print_ast(ast_root, NULL);
//     } else {
//         fprintf(stderr, "\n[DEBUG-AST] No valid AST could be constructed\n");
//     }
    
//     return (ast_root);
// }
// t_node *proc_token_list(t_vars *vars)
// {
//     t_node *ast_root = NULL;
    
//     if (!vars || !vars->head || !vars->pipes)
//         return (NULL);
    
//     // Debug print the token list before processing
//     fprintf(stderr, "\n[DEBUG-AST] Token list before AST construction:\n");
//     print_token_list(vars->head, NULL);
    
//     find_cmd(NULL, NULL, FIND_ALL, vars);
    
//     if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
//         || !vars->cmd_nodes[0]->args)
//         return (NULL);
//     vars->pipes->pipe_root = NULL;
//     vars->pipes->redir_root = NULL;
    
//     vars->pipes->pipe_root = proc_pipes(vars);
//     vars->pipes->redir_root = proc_redir(vars);
    
//     // Attach any orphaned arguments to their commands
//     attach_orphaned_args(vars);
    
//     // Determine which root to use
//     if (vars->pipes->pipe_root)
//         ast_root = vars->pipes->pipe_root;
//     else if (vars->pipes->redir_root)
//         ast_root = vars->pipes->redir_root;
//     else if (vars->cmd_count > 0)
//         ast_root = vars->cmd_nodes[0];
    
//     // Debug print the constructed AST
//     if (ast_root) {
//         fprintf(stderr, "\n[DEBUG-AST] Final Abstract Syntax Tree after orphan attachment:\n");
//         print_ast(ast_root, NULL);
//     } else {
//         fprintf(stderr, "\n[DEBUG-AST] No valid AST could be constructed\n");
//     }
    
//     return (ast_root);
// }
t_node *proc_token_list(t_vars *vars)
{
    t_node *ast_root = NULL;
    
    if (!vars || !vars->head || !vars->pipes)
        return (NULL);
    
    // Debug print the token list before processing
    fprintf(stderr, "\n[DEBUG-AST] Token list before AST construction:\n");
    print_token_list(vars->head, NULL);
    
    find_cmd(NULL, NULL, FIND_ALL, vars);
    
    if (vars->cmd_count == 0 || !vars->cmd_nodes[0]
        || !vars->cmd_nodes[0]->args)
        return (NULL);
    vars->pipes->pipe_root = NULL;
    vars->pipes->redir_root = NULL;
    
    vars->pipes->pipe_root = proc_pipes(vars);
    vars->pipes->redir_root = proc_redir(vars);
    
    // Attach any orphaned arguments to their commands
    attach_orphaned_args(vars);
    fprintf(stderr, "[DEBUG-AST] Command nodes after orphan attachment:\n");
	for (int i = 0; i < vars->cmd_count; i++) 
	{
    	fprintf(stderr, "  Cmd %d: ", i);
    	print_node_content(stderr, vars->cmd_nodes[i]);
    	fprintf(stderr, "\n");
	}
    // Determine which root to use
    if (vars->pipes->pipe_root)
        ast_root = vars->pipes->pipe_root;
    else if (vars->pipes->redir_root)
        ast_root = vars->pipes->redir_root;
    else if (vars->cmd_count > 0)
        ast_root = vars->cmd_nodes[0];
    
    // Debug print the constructed AST
    if (ast_root) {
        fprintf(stderr, "\n[DEBUG-AST] Final Abstract Syntax Tree after orphan attachment:\n");
        print_ast(ast_root, NULL);
    } else {
        fprintf(stderr, "\n[DEBUG-AST] No valid AST could be constructed\n");
    }
    
    return (ast_root);
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

	// if (DEBUG_REDIR) //DEBUG PRINT
	//	fprintf(stderr, "[DEBUG] Starting build_redir_ast with error_code=%d\n", vars->error_code); //DEBUG PRINT

	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_CMD)
		{
			// if (DEBUG_REDIR && current->args) //DEBUG PRINT
			//	fprintf(stderr, "[DEBUG] Found command node: %s\n", current->args[0]); //DEBUG PRINT
			vars->pipes->last_cmd = current;
		}
		else if (is_redirection(current->type))
		{
			// if (DEBUG_REDIR) //DEBUG PRINT
			// { //DEBUG PRINT
			//	fprintf(stderr, "[DEBUG] Found redirection node type: %s\n", get_token_str(current->type)); //DEBUG PRINT
			//	if (current->next && current->next->args) //DEBUG PRINT
			//		fprintf(stderr, "[DEBUG] Redirection target: %s\n", current->next->args[0]); //DEBUG PRINT
			// } //DEBUG PRINT
			
			if (!is_valid_redir_node(current) || !current->next->args
				|| !current->next->args[0])
			{
				// if (DEBUG_ERROR) //DEBUG PRINT
				//	fprintf(stderr, "[DEBUG] Invalid redirection, setting syntax error\n"); //DEBUG PRINT
				tok_syntax_error_msg("newline", vars);
				return ;
			}
			process_redir_node(current, vars);
		}
		current = current->next;
	}
	// if (DEBUG_REDIR) //DEBUG PRINT
	//	fprintf(stderr, "[DEBUG] Completed build_redir_ast with error_code=%d\n", vars->error_code); //DEBUG PRINT
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
/*
 * Ensures redirection nodes are properly processed and linked in the AST
 * Returns the root redirection node or NULL if no redirections found
 */
t_node *proc_redir(t_vars *vars)
{
    t_node *current;
    t_node *first_redir;
    
    if (!vars || !vars->head)
        return (NULL);
    
    reset_redir_tracking(vars->pipes);
    current = vars->head;
    first_redir = NULL;
    
    fprintf(stderr, "[DEBUG-REDIR] Processing redirections in token list\n");
    
    while (current)
    {
        if (is_redirection(current->type))
        {
            fprintf(stderr, "[DEBUG-REDIR] Found redirection: %s\n", get_token_str(current->type));
            
            if (!chk_redir_nodes(vars, current))
            {
                current = current->next;
                continue;
            }
            
            process_redir_node(current, vars);
            
            if (!first_redir)
                first_redir = current;
        }
        current = current->next;
    }
    
    if (first_redir)
    {
        fprintf(stderr, "[DEBUG-REDIR] Setting redirection root: %s\n", get_token_str(first_redir->type));
        vars->pipes->redir_root = first_redir;
    }
    else
    {
        fprintf(stderr, "[DEBUG-REDIR] No redirections found in token list\n");
    }
    
    if (vars->pipes->pipe_root)
        link_redirs_pipes(vars);
        
    return (vars->pipes->redir_root);
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
void	process_redir_node(t_node *redir_node, t_vars *vars)
{
	t_node	*cmd;

	if (DEBUG_REDIR) //DEBUG PRINT
	{ //DEBUG PRINT
		fprintf(stderr, "In process_redir_node().Processing redirection: %s -> ", //DEBUG PRINT
			get_token_str(redir_node->type)); //DEBUG PRINT
		if (redir_node->next && redir_node->next->args) //DEBUG PRINT
			fprintf(stderr, "target: %s\n", redir_node->next->args[0]); //DEBUG PRINT
		else //DEBUG PRINT
			fprintf(stderr, "target: NULL\n"); //DEBUG PRINT
	} //DEBUG PRINT
	
	cmd = get_redir_target(redir_node, vars->pipes->last_cmd);
	
	if (DEBUG_REDIR) //DEBUG PRINT
	{ //DEBUG PRINT
		fprintf(stderr, "[DEBUG] Found target command for redirection: "); //DEBUG PRINT
		if (cmd && cmd->args) //DEBUG PRINT
			fprintf(stderr, "%s\n", cmd->args[0]); //DEBUG PRINT
		else //DEBUG PRINT
			fprintf(stderr, "NULL\n"); //DEBUG PRINT
	} //DEBUG PRINT
	
	if (cmd && redir_node->next)
	{
		if (DEBUG_REDIR) //DEBUG PRINT
			fprintf(stderr, "[DEBUG] Setting up redirection links\n"); //DEBUG PRINT
		set_redir_node(redir_node, cmd, redir_node->next);
		link_prev_redirs(redir_node, cmd, vars);
		track_redirs(redir_node, cmd, vars);
		link_in_out_redirs(vars);
		if (!vars->pipes->redir_root)
		{
			if (DEBUG_REDIR) //DEBUG PRINT
				fprintf(stderr, "[DEBUG] Setting first redirection as root\n"); //DEBUG PRINT
			vars->pipes->redir_root = redir_node;
		}
	}
	
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] After process_redir_node, error_code=%d\n", vars->error_code); //DEBUG PRINT
}

/*
Checks if a node is being used as a redirection target.
Returns:
- 1 if node is a redirection target
- 0 if not
*/
// int is_redir_target(t_node *head, t_node *node)
// {
//     t_node *current = head;
    
//     while (current) {
//         if (is_redirection(current->type) && current->right == node)
//             return (1);
//         current = current->next;
//     }
//     return (0);
// }
// int is_redir_target(t_node *head, t_node *node)
// {
//     t_node *current;
    
//     if (!head || !node)
//         return (0);
        
//     current = head;
//     while (current) {
//         if (is_redirection(current->type) && current->right == node)
//             return (1);
//         current = current->next;
//     }
//     return (0);
// }
// int	is_redir_target(t_node *head, t_node *node)
// {
//     t_node *current;
    
//     if (!head || !node)
//         return (0);
        
//     current = head;
//     while (current) {
//         if (is_redirection(current->type) && current->right == node)
//             return (1);
//         current = current->next;
//     }
//     return (0);
// }
// /*
// Checks if a node is being used as a redirection target.
// Returns:
// - 1 if node is a redirection target
// - 0 if not
// */
// int is_redir_target(t_node *head, t_node *node)
// {
//     t_node *current;
    
//     if (!head || !node)
//         return (0);
        
//     current = head;
//     while (current) {
//         if (is_redirection(current->type) && current->right == node)
//             return (1);
//         current = current->next;
//     }
//     return (0);
// }
/*
Checks if a node is being used as a redirection target.
Returns:
- 1 if node is a redirection target
- 0 if not
*/
int is_redir_target(t_node *head, t_node *node)
{
    t_node *current;
    
    if (!head || !node)
        return (0);
        
    current = head;
    while (current) {
        if (is_redirection(current->type) && current->right == node) {
            fprintf(stderr, "[DEBUG-ORPHAN] Node is a redirection target for %s\n", 
                   get_token_str(current->type));
            return (1);
        }
        current = current->next;
    }
    return (0);
}

/*
Attaches "orphaned" arguments that appear after redirections to their command nodes.
- Scans the token list for arguments that aren't connected in the AST
- Uses the linked list structure to determine argument ownership
- Links orphaned arguments to appropriate command nodes
- Skips arguments that are being used as redirection targets
*/
// void attach_orphaned_args(t_vars *vars)
// {
//     t_node  *current;
//     t_node  *cmd_node;
//     int     i;
    
//     if (!vars || !vars->head)
//         return;
    
//     fprintf(stderr, "[DEBUG-AST] Attaching orphaned arguments\n");
    
//     current = vars->head;
//     while (current)
//     {
//         // Only process unattached argument nodes that aren't redirection targets
//         if (current->type == TYPE_ARGS && !current->left && !current->right && 
//             !is_redir_target(vars->head, current))
//         {
//             // Find the command this argument belongs to
//             cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
            
//             if (cmd_node && cmd_node->type == TYPE_CMD)
//             {
//                 fprintf(stderr, "[DEBUG-ORPHAN] Found orphaned arg: %s\n", 
//                        current->args[0]);
//                 fprintf(stderr, "[DEBUG-ORPHAN] Attaching to command: %s\n", 
//                        cmd_node->args[0]);
                
//                 // Append the argument to the command's args array
//                 if (current->args && current->args[0])
//                 {
//                     // Call append_arg with the node, not its args array
//                     // Use 0 for the quote_type since we don't know the type
//                     append_arg(cmd_node, current->args[0], 0);
                    
//                     fprintf(stderr, "[DEBUG-ORPHAN] Command args after attachment:\n");
//                     i = 0;
//                     while (cmd_node->args && cmd_node->args[i])
//                     {
//                         fprintf(stderr, "  arg[%d]: %s\n", i, cmd_node->args[i]);
//                         i++;
//                     }
//                 }
//             }
//             else
//             {
//                 fprintf(stderr, "[DEBUG-ORPHAN] Could not find command for arg: %s\n",
//                        current->args[0]);
//             }
//         }
//         current = current->next;
//     }
// }
// void attach_orphaned_args(t_vars *vars)
// {
//     t_node  *current;
//     t_node  *cmd_node;
//     int     i;
    
//     if (!vars || !vars->head)
//         return;
    
//     fprintf(stderr, "[DEBUG-AST] Attaching orphaned arguments\n");
    
//     current = vars->head;
//     while (current)
//     {
//         // Only process unattached argument nodes that aren't redirection targets
//         if (current->type == TYPE_ARGS && !current->left && !current->right && 
//             !is_redir_target(vars->head, current))
//         {
//             // Find the command this argument belongs to
//             cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
            
//             if (cmd_node && cmd_node->type == TYPE_CMD)
//             {
//                 fprintf(stderr, "[DEBUG-ORPHAN] Found orphaned arg: %s\n", 
//                        current->args[0]);
//                 fprintf(stderr, "[DEBUG-ORPHAN] Attaching to command: %s\n", 
//                        cmd_node->args[0]);
                
//                 // Append the argument to the command's args array
//                 if (current->args && current->args[0])
//                 {
//                     append_arg(cmd_node, current->args[0], 0);
                    
//                     fprintf(stderr, "[DEBUG-ORPHAN] Command args after attachment:\n");
//                     i = 0;
//                     while (cmd_node->args && cmd_node->args[i])
//                     {
//                         fprintf(stderr, "  arg[%d]: %s\n", i, cmd_node->args[i]);
//                         i++;
//                     }
//                 }
//             }
//             else
//             {
//                 fprintf(stderr, "[DEBUG-ORPHAN] Could not find command for arg: %s\n",
//                        current->args[0]);
//             }
//         }
//         current = current->next;
//     }
// }
// void attach_orphaned_args(t_vars *vars)
// {
//     t_node  *current;
//     t_node  *cmd_node;
//     int     i;
    
//     if (!vars || !vars->head)
//         return;
    
//     fprintf(stderr, "[DEBUG-AST] Attaching orphaned arguments\n");
    
//     current = vars->head;
//     while (current)
//     {
//         // Only process unattached argument nodes that aren't redirection targets
//         if (current->type == TYPE_ARGS && !current->left && !current->right && 
//             !is_redir_target(vars->head, current))
//         {
//             // Find the command this argument belongs to
//             cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
            
//             if (cmd_node && cmd_node->type == TYPE_CMD)
//             {
//                 fprintf(stderr, "[DEBUG-ORPHAN] Found orphaned arg: %s\n", 
//                        current->args[0]);
//                 fprintf(stderr, "[DEBUG-ORPHAN] Attaching to command: %s\n", 
//                        cmd_node->args[0]);
                
//                 // Append the argument to the command's args array
//                 if (current->args && current->args[0])
//                 {
//                     append_arg(cmd_node, current->args[0], 0);
                    
//                     fprintf(stderr, "[DEBUG-ORPHAN] Command args after attachment:\n");
//                     i = 0;
//                     while (cmd_node->args && cmd_node->args[i])
//                     {
//                         fprintf(stderr, "  arg[%d]: %s\n", i, cmd_node->args[i]);
//                         i++;
//                     }
//                 }
//             }
//             else
//             {
//                 fprintf(stderr, "[DEBUG-ORPHAN] Could not find command for arg: %s\n",
//                        current->args[0]);
//             }
//         }
//         current = current->next;
//     }
// }
// /*
// Attaches "orphaned" arguments that appear after redirections to their command nodes.
// - Scans the token list for arguments that aren't connected in the AST
// - Uses the linked list structure to determine argument ownership
// - Links orphaned arguments to appropriate command nodes
// - Skips arguments that are being used as redirection targets
// */
// void attach_orphaned_args(t_vars *vars)
// {
//     t_node  *current;
//     t_node  *cmd_node;
//     int     i;
    
//     if (!vars || !vars->head)
//         return;
    
//     fprintf(stderr, "[DEBUG-AST] Attaching orphaned arguments\n");
    
//     current = vars->head;
//     while (current)
//     {
//         // Only process unattached argument nodes that aren't redirection targets
//         if (current->type == TYPE_ARGS && !current->left && !current->right && 
//             !is_redir_target(vars->head, current))
//         {
//             // Find the command this argument belongs to
//             cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
            
//             if (cmd_node && cmd_node->type == TYPE_CMD)
//             {
//                 fprintf(stderr, "[DEBUG-ORPHAN] Found orphaned arg: %s\n", 
//                        current->args[0]);
//                 fprintf(stderr, "[DEBUG-ORPHAN] Attaching to command: %s\n", 
//                        cmd_node->args[0]);
                
//                 // Append the argument to the command's args array
//                 if (current->args && current->args[0])
//                 {
//                     append_arg(cmd_node, current->args[0], 0);
                    
//                     fprintf(stderr, "[DEBUG-ORPHAN] Command args after attachment:\n");
//                     i = 0;
//                     while (cmd_node->args && cmd_node->args[i])
//                     {
//                         fprintf(stderr, "  arg[%d]: %s\n", i, cmd_node->args[i]);
//                         i++;
//                     }
//                 }
//             }
//             else
//             {
//                 fprintf(stderr, "[DEBUG-ORPHAN] Could not find command for arg: %s\n",
//                        current->args[0]);
//             }
//         }
//         current = current->next;
//     }
// }
/*
Attaches "orphaned" arguments that appear after redirections to their command nodes.
- Scans the token list for arguments that aren't connected in the AST
- Uses the linked list structure to determine argument ownership
- Links orphaned arguments to appropriate command nodes
- Skips arguments that are being used as redirection targets
*/
void attach_orphaned_args(t_vars *vars)
{
    t_node  *current;
    t_node  *cmd_node;
    int     i;
    
    if (!vars || !vars->head)
        return;
    
    fprintf(stderr, "[DEBUG-AST] Attaching orphaned arguments\n");
    
    current = vars->head;
    while (current)
    {
        // Only process unattached argument nodes that aren't redirection targets
        if (current->type == TYPE_ARGS && !current->left && !current->right && 
            !is_redir_target(vars->head, current))
        {
            // Find the command this argument belongs to
            cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
            
            if (cmd_node && cmd_node->type == TYPE_CMD)
            {
                fprintf(stderr, "[DEBUG-ORPHAN] Found orphaned arg: %s\n", 
                       current->args[0]);
                fprintf(stderr, "[DEBUG-ORPHAN] Attaching to command: %s\n", 
                       cmd_node->args[0]);
                
                // Append the argument to the command's args array
                if (current->args && current->args[0])
                {
                    append_arg(cmd_node, current->args[0], 0);
                    
                    fprintf(stderr, "[DEBUG-ORPHAN] Command args after attachment:\n");
                    i = 0;
                    while (cmd_node->args && cmd_node->args[i])
                    {
                        fprintf(stderr, "  arg[%d]: %s\n", i, cmd_node->args[i]);
                        i++;
                    }
                }
            }
            else
            {
                fprintf(stderr, "[DEBUG-ORPHAN] Could not find command for arg: %s\n",
                       current->args[0]);
            }
        }
        current = current->next;
    }
}
