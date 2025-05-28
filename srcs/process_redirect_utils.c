/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redirect_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:21:59 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 02:56:15 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Resets redirection tracking state in the pipes structure.
- Clears command and redirection node pointers.
- Prepares pipes structure for new redirection processing.
Works with proc_redir to clean state before processing.
*/
void	reset_redir_tracking(t_pipe *pipes)
{
	if (!pipes)
		return ;
	fprintf(stderr, "REDIR-DBG: Resetting redirection tracking state\n");
	pipes->last_cmd = NULL;
	pipes->redir_root = NULL;
	pipes->last_in_redir = NULL;
	pipes->last_out_redir = NULL;
	pipes->cmd_redir = NULL;
}

/*
Determines if a redirection node has valid adjacent commands.
- Checks if next node exists and is a command.
Returns:
- 1 if redirection has valid syntax.
- 0 otherwise.
*/
int is_valid_redir_node(t_node *current)
{
	fprintf(stderr, "DEBUG-VALIDATE-REDIR: Checking redirection node type=%s\n", 
			get_token_str(current->type));
	
	if (!current)
	{
		fprintf(stderr, "DEBUG-VALIDATE-REDIR: NULL node\n");
		return (0);
	}
	
	if (!is_redirection(current->type))
	{
		fprintf(stderr, "DEBUG-VALIDATE-REDIR: Not a redirection node\n");
		return (0);
	}
	
	// CRITICAL FIX: Check for two valid cases:
	// 1. Redirection has args already (quoted filename case)
	if (current->args && current->args[0])
	{
		fprintf(stderr, "DEBUG-VALIDATE-REDIR: Valid redirection node %s with embedded filename '%s'\n",
				get_token_str(current->type), current->args[0]);
		return (1);
	}
	
	// 2. Next node is a CMD or ARGS (traditional case)
	if (!current->next || (current->next->type != TYPE_CMD 
			&& current->next->type != TYPE_ARGS))
	{
		fprintf(stderr, "DEBUG-VALIDATE-REDIR: Invalid: missing next node or not CMD/ARGS\n");
		return (0);
	}
	
	fprintf(stderr, "DEBUG-VALIDATE-REDIR: Valid redirection node %s with target ARGS\n",
			get_token_str(current->type));
	return (1);
}

/*
Configures a redirection node with source and target commands.
- Sets left child to source command node.
- Sets right child to target command/filename node.
- Establishes the redirection relationship in the AST.
Works with proc_redir().
*/
// void	set_redir_node(t_node *redir, t_node *cmd, t_node *target)
// {
// 	if (!redir || !cmd || !target)
// 		return ;
// 	redir->left = cmd;
// 	redir->right = target;
// }
void	set_redir_node(t_node *redir, t_node *cmd, t_node *target)
{
	if (!redir || !cmd || !target)
	{
		fprintf(stderr, "REDIR-DBG: Invalid set_redir_node arguments\n");
		return ;
	}
	fprintf(stderr, "REDIR-DBG: Setting redirection: %s -> %s -> %s\n",
			cmd->args ? cmd->args[0] : "NULL",
			get_token_str(redir->type),
			target->args ? target->args[0] : "NULL");
	redir->left = cmd;
	redir->right = target;
}

/*
Finds redirection nodes associated with a specific command node.
- Searches through the redirection list for nodes targeting the command.
- Uses get_redir_target() to determine if a redirection points to the command.
- Handles NULL inputs safely.
Returns:
- The first redirection node that targets the command.
- NULL if no matching redirection found.
Works with swap_cmd_redir() to connect commands and redirections in the AST.
*/
t_node *find_cmd_redir(t_node *redir_root, t_node *cmd_node, t_vars *vars)
{
	t_node *current;

	// Explicitly mark vars as used to avoid compiler warning
	(void)vars;
	if (!redir_root || !cmd_node)
	{
		fprintf(stderr, "REDIR-DBG: find_cmd_redir called with NULL args\n");
		return (NULL);
	}
	fprintf(stderr, "REDIR-DBG: Searching redirections for command '%s'\n",
			cmd_node->args ? cmd_node->args[0] : "NULL");
	
	current = redir_root;
	while (current)
	{
		if (is_redirection(current->type) && current->redir == cmd_node)
		{
			fprintf(stderr, "REDIR-DBG: Found matching redirection %s for '%s'\n",
					get_token_str(current->type),
					cmd_node->args ? cmd_node->args[0] : "NULL");
			return (current);
		}
		current = current->next;
	}
	
	fprintf(stderr, "REDIR-DBG: No matching redirection found for '%s'\n",
			cmd_node->args ? cmd_node->args[0] : "NULL");
	return (NULL);
}