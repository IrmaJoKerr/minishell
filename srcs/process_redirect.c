/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redirect.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:40:07 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 15:06:15 by bleow            ###   ########.fr       */
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
	if (vars->pipes->pipe_root)
		link_redirs_pipes(vars);
	if (vars->pipes->redir_root)
	{
		vars->astroot = vars->pipes->redir_root;
	}
	return (vars->pipes->redir_root);
}

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
void	build_redir_ast(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_CMD)
			vars->pipes->last_cmd = current;
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
Analyses a redirection node during pipe integration.
- Checks if the node is the head of its redirection chain
- Finds the target command for the redirection
- Updates pipe structure to use redirection nodes
Returns:
1 if current node should be processed further
0 if we should skip to the next node
*/
int	chk_redir_nodes(t_vars *vars, t_node *current)
{
	t_node	*chain_head;
	t_node	*target_cmd;

	chain_head = find_redir_chain_head(current, vars->pipes->last_cmd);
	if (chain_head != current)
	{
		return (0);
	}
	target_cmd = get_redir_target(current, vars->pipes->last_cmd);
	if (target_cmd && vars->pipes->pipe_root)
	{
		upd_pipe_redir(vars->pipes->pipe_root, target_cmd, current);
	}
	return (1);
}

/*
Finds the head (first) redirection node in a chain of redirections.
- Scans backward from current node to find first redirection
- All redirections in the chain must target the same command
Returns:
- The first (earliest) redirection node in the chain
- The original node if no earlier nodes found
Works with link_redirs_pipes().
*/
t_node	*find_redir_chain_head(t_node *current, t_node *last_cmd)
{
	t_node	*chain_head;
	t_node	*prev_redir;
	t_node	*current_target;
	t_node	*prev_target;
	int		same_target;

	chain_head = current;
	prev_redir = current->prev;
	current_target = get_redir_target(current, last_cmd);
	while (prev_redir)
	{
		prev_target = get_redir_target(prev_redir, last_cmd);
		same_target = (prev_target == current_target);
		if (!is_redirection(prev_redir->type) || !same_target)
			break ;
		chain_head = prev_redir;
		prev_redir = prev_redir->prev;
	}
	return (chain_head);
}

/*
Integrates redirection nodes with the pipe structure.
- Updates pipe node references to point to redirection nodes.
- Ensures pipes use redirection nodes instead of direct commands.
- Makes pipe commands output to redirections correctly.
Works with proc_redir() when pipe nodes exist.
*/
void	link_redirs_pipes(t_vars *vars)
{
	t_node	*current;

	if (!vars || !vars->pipes || !vars->pipes->pipe_root || !vars->head)
		return ;
	vars->pipes->last_cmd = NULL;
	while (current)
	{
		if (current->type == TYPE_CMD)
		{
			vars->pipes->last_cmd = current;
		}
		else if (is_redirection(current->type) && current->next)
		{
			if (!chk_redir_nodes(vars, current))
			{
				current = current->next;
				continue ;
			}
		}
		current = current->next;
	}
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
	return (target);
}

/*
Updates pipe structure when commands are redirected.
- Traverses pipe chain looking for references to the command.
- Replaces command references with redirection node references.
- Preserves pipe structure while incorporating redirections.
- Handles both left and right side command replacements.
*/
void	upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir)
{
	t_node	*pipe_node;

	if (!pipe_root || !cmd || !redir)
		return ;
	pipe_node = pipe_root;
	while (pipe_node)
	{
		if (pipe_node->left == cmd)
		{
			pipe_node->left = redir;
		}
		else if (pipe_node->right == cmd)
		{
			pipe_node->right = redir;
		}
		if (pipe_node->right && pipe_node->right->type == TYPE_PIPE)
			pipe_node = pipe_node->right;
		else
			break ;
	}
}

/*
Determines if a redirection node has valid adjacent commands.
- Checks if next node exists and is a command.
Returns:
- 1 if redirection has valid syntax.
- 0 otherwise.
*/
int	is_valid_redir_node(t_node *current)
{
	if (!current)
		return (0);
	if (!is_redirection(current->type))
		return (0);
	if (!current->next || (current->next->type != TYPE_CMD
			&& current->next->type != TYPE_ARGS))
		return (0);
	return (1);
}
