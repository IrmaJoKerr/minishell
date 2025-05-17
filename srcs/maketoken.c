/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   maketoken.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:59:48 by bleow             #+#    #+#             */
/*   Updated: 2025/05/16 04:21:09 by bleow            ###   ########.fr       */
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
// void	maketoken(char *token, t_tokentype type, t_vars *vars) PRE ADDED DEBUG PRINTS
// {
// 	t_node	*node;
// 	t_node	*curr;
// 	int		node_freed;

// 	if (!token || !vars)
// 	{
// 		return ;
// 	}
// 	node = initnode(type, token);
// 	if (!node)
// 	{
// 		return ;
// 	}
// 	node_freed = build_token_linklist(vars, node);
// 	if (!node_freed)
// 	{
// 		free_if_orphan_node(node, vars);
// 	}
// 	curr = vars->head;
// 	while (curr)
// 	{
// 		curr = curr->next;
// 	}
// }
void	maketoken(char *token, t_tokentype type, t_vars *vars)
{
	t_node	*node;
	t_node	*curr;
	int		node_freed;

	fprintf(stderr, "DEBUG-MAKE-TOKEN: Creating token type=%d content='%s'\n", 
		type, token ? token : "NULL");
	if (!token || !vars)
	{
		fprintf(stderr, "DEBUG-MAKE-TOKEN: Invalid token or vars\n");
		return ;
	}
	node = initnode(type, token);
	if (!node)
	{
		fprintf(stderr, "DEBUG-MAKE-TOKEN: Failed to initialize node\n");
		return ;
	}
	node_freed = build_token_linklist(vars, node);
	fprintf(stderr, "DEBUG-MAKE-TOKEN: build_token_linklist result: %d\n", node_freed);
	if (!node_freed)
	{
		free_if_orphan_node(node, vars);
	}
	fprintf(stderr, "DEBUG-MAKE-TOKEN: Traversing final token list\n");
	curr = vars->head;
	while (curr)
	{
		fprintf(stderr, "  Node at %p: type=%d content='%s'\n", 
			(void*)curr, curr->type, 
			(curr->args && curr->args[0]) ? curr->args[0] : "NULL");
		curr = curr->next;
	}
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
// int	build_token_linklist(t_vars *vars, t_node *node)
// {
// 	if (!vars || !node)
// 	{
// 		return (0);
// 	}
// 	if (!vars->head)
// 	{
// 		vars->head = node;
// 		vars->current = node;
// 		return (0);
// 	}
// 	if (vars->current && vars->current->type == TYPE_PIPE
// 		&& node->type == TYPE_ARGS)
// 	{
// 		node->type = TYPE_CMD;
// 	}
// 	if (node->type == TYPE_ARGS && vars->current
// 		&& vars->current->type == TYPE_CMD)
// 	{
// 		return (merge_arg_with_cmd(vars, node));
// 	}
// 	else
// 	{
// 		token_link(node, vars);
// 		return (0);
// 	}
// }
int	build_token_linklist(t_vars *vars, t_node *node)
{
	fprintf(stderr, "DEBUG-LINK-TOKEN: Linking token type=%d to list (current=%d)\n", 
		node->type, vars->current ? vars->current->type : 0);
	if (!vars || !node)
	{
		fprintf(stderr, "DEBUG-LINK-TOKEN: Invalid vars or node\n");
		return (0);
	}
	if (!vars->head)
	{
		fprintf(stderr, "DEBUG-LINK-TOKEN: Setting as head node\n");
		vars->head = node;
		vars->current = node;
		return (0);
	}
	if (vars->current && vars->current->type == TYPE_PIPE && node->type == TYPE_ARGS)
	{
		fprintf(stderr, "DEBUG-LINK-TOKEN: Converting ARGS to CMD after PIPE\n");
		node->type = TYPE_CMD;
	}
	if (node->type == TYPE_ARGS && vars->current && vars->current->type == TYPE_CMD)
	{
		fprintf(stderr, "DEBUG-LINK-TOKEN: Merging ARGS with CMD\n");
		return (merge_arg_with_cmd(vars, node));
	}
	else
	{
		fprintf(stderr, "DEBUG-LINK-TOKEN: Linking as separate node\n");
		token_link(node, vars);
		return (0);
	}
}

/*
Helper function to link a new token node to the current node.
- Sets the next pointer of the current node to the new node.
- Sets the prev pointer of the new node to the current node.
- Updates the current pointer to the new node.
- Works with build_token_linklist().
*/
void	token_link(t_node *node, t_vars *vars)
{
	vars->current->next = node;
	node->prev = vars->current;
	vars->current = node;
}

/*
Merges an argument node into a command node.
- Appends argument text to command's argument list
- Updates linked list connections after merging
- Frees the now-redundant argument node
Returns:
- 1 to indicate node was merged and old arg node freed.
*/
// int	merge_arg_with_cmd(t_vars *vars, t_node *arg_node) PRE ADDED DEBUG PRINTS
// {
// 	t_node	*cmd_node;
// 	t_node	*next_node;

// 	cmd_node = vars->current;
// 	next_node = arg_node->next;
// 	append_arg(cmd_node, arg_node->args[0], 0);
// 	if (next_node)
// 	{
// 		cmd_node->next = next_node;
// 		next_node->prev = cmd_node;
// 	}
// 	else
// 	{
// 		cmd_node->next = NULL;
// 		vars->current = cmd_node;
// 	}
// 	free_token_node(arg_node);
// 	return (1);
// }
int	merge_arg_with_cmd(t_vars *vars, t_node *arg_node)
{
	t_node	*cmd_node;
	t_node	*next_node;

	cmd_node = vars->current;
	next_node = arg_node->next;
	fprintf(stderr, "DEBUG-MERGE-ARG: Merging arg '%s' with command '%s'\n",
		arg_node->args ? arg_node->args[0] : "NULL",
		cmd_node->args ? cmd_node->args[0] : "NULL");
	append_arg(cmd_node, arg_node->args[0], 0);
	if (next_node)
	{
		fprintf(stderr, "DEBUG-MERGE-ARG: Connecting next node in chain\n");
		cmd_node->next = next_node;
		next_node->prev = cmd_node;
	}
	else
	{
		fprintf(stderr, "DEBUG-MERGE-ARG: No next node, setting current to cmd\n");
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
		{
			found_in_list = 1;
		}
		check = check->next;
	}
	if (!found_in_list)
	{
		free_token_node(node);
	}
}
