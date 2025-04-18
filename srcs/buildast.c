/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/04/18 21:18:56 by bleow            ###   ########.fr       */
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
t_node *proc_token_list(t_vars *vars)
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
	if (vars->pipes->pipe_root)
		return (vars->pipes->pipe_root);
	else if (vars->pipes->redir_root)
		return (vars->pipes->redir_root);
	else if (vars->cmd_count > 0)
		return (vars->cmd_nodes[0]);
	return NULL;
}
