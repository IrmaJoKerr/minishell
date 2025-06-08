/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   maketoken.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:59:48 by bleow             #+#    #+#             */
/*   Updated: 2025/06/07 02:54:02 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates a token node and adds it to the token linked list.
- Creates a node with the given token string and type.
- Links the node into the shell's token linked list.
- Handles orphaned nodes to prevent memory leaks.
- Updates current token pointer as needed.
Example: When tokenizing "echo hello"
- Creates separate nodes for "echo" and "hello"
- Links them in sequence with proper type assignment.
*/
void	maketoken(char *token, t_tokentype type, t_vars *vars)
{
	t_node		*node;
	int			node_freed;
	static int	token_position = 0;

	if (!token || !vars)
		return ;
	node = initnode(type, token);
	if (!node)
		return ;
	if (type == TYPE_PIPE)
		vars->pipes->in_pipe = 1;
	node_freed = build_token_linklist(vars, node);
	if (!node_freed)
		free_if_orphan_node(node, vars);
	vars->prev_type = type;
	token_position++;
}

/*
Updates the token list with a new node.
- Handles first token as head.
- Otherwise adds to end of list.
- Updates current pointer.

Example: When adding command node
- If first token, sets as head
- Otherwise links to previous token
- Updates current pointer
*/
int	build_token_linklist(t_vars *vars, t_node *node)
{
	if (!vars || !node)
		return (0);
	if (!vars->head)
	{
		vars->head = node;
		vars->current = node;
		return (0);
	}
	if (vars->current && vars->current->type == TYPE_PIPE
		&& node->type == TYPE_ARGS)
	{
		node->type = TYPE_CMD;
	}
	if (node->type == TYPE_ARGS && vars->current
		&& vars->current->type == TYPE_CMD)
	{
		return (merge_arg_with_cmd(vars, node));
	}
	else
	{
		vars->current->next = node;
		node->prev = vars->current;
		vars->current = node;
		return (0);
	}
}

/*
Merges an argument node into a command node.
- Appends argument text to command's argument list
- Updates linked list connections after merging
- Frees the now-redundant argument node
Returns:
- 1 to indicate node was merged and old arg node freed.
*/
int	merge_arg_with_cmd(t_vars *vars, t_node *arg_node)
{
	t_node	*cmd_node;
	t_node	*next_node;

	cmd_node = vars->current;
	next_node = arg_node->next;
	append_arg(cmd_node, arg_node->args[0], 0);
	if (next_node)
	{
		cmd_node->next = next_node;
		next_node->prev = cmd_node;
	}
	else
	{
		cmd_node->next = NULL;
		vars->current = cmd_node;
	}
	free_token_node(arg_node);
	return (1);
}

/*
Checks if a node is orphaned (not properly linked in the token list)
and frees it if necessary to prevent memory leaks.
This function is called when a node is created and added to the token list.
*/
void	free_if_orphan_node(t_node *node, t_vars *vars)
{
	t_node	*check;
	int		found_in_list;

	found_in_list = 0;
	if (node == vars->head || node == vars->current)
	{
		return ;
	}
	check = vars->head;
	while (check && !found_in_list)
	{
		if (check == node)
			found_in_list = 1;
		check = check->next;
	}
	if (!found_in_list)
		ft_safefree((void **)&node);
}
