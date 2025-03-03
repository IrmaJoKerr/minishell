/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 08:13:36 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 12:57:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates and links a new node in a linked list.
*/
int	makenode(t_vars *vars, char *data)
{
	t_node	*newnode;

	newnode = initnode(vars->curr_type, data);
	if (!newnode)
		return (0);
	if (vars->current)
	{
		vars->current->next = newnode;
		newnode->prev = vars->current;
	}
	vars->current = newnode;
	return (1);
}

/*
Adds a child node to a parent node, if the parent node has an empty child node.
*/
void	add_child(t_node *parent, t_node *child)
{
	if (!parent || !child)
		return ;
	if (!parent->left)
	{
		parent->left = child;
		child->prev = parent;
	}
	else if (!parent->right)
	{
		parent->right = child;
		child->prev = parent;
	}
}

/*
Handles pipe node in AST construction
*/
void	handle_pipe_node(t_node **root, t_node *pipe_node)
{
	t_node	*new_pipe;

	new_pipe = initnode(TYPE_PIPE, NULL);
	if (!*root)
	{
		*root = new_pipe;
		(*root)->left = pipe_node->prev;
		(*root)->right = pipe_node->next;
	}
	else
	{
		new_pipe->left = *root;
		new_pipe->right = pipe_node->next;
		*root = new_pipe;
	}
}

/*
Handles redirection node in AST construction
*/
void	redirection_node(t_node *root, t_node *redir_node)
{
	t_node	*cmd_node;

	cmd_node = root;
	while (cmd_node && cmd_node->type == TYPE_PIPE)
		cmd_node = cmd_node->right;
	if (cmd_node)
	{
		if (!cmd_node->right)
			cmd_node->right = redir_node;
		else
		{
			redir_node->left = cmd_node->right;
			cmd_node->right = redir_node;
		}
	}
}
