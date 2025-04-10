/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 23:38:16 by bleow            ###   ########.fr       */
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
		return (vars->pipes->pipe_root);
	else if (vars->pipes->redir_root)
		return (vars->pipes->redir_root);
	else if (vars->cmd_count > 0)
		return (vars->cmd_nodes[0]);
	return NULL;
}
