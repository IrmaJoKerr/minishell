/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 08:13:36 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 10:31:54 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates and links a new node in a linked list of tokens.
Uses the s_vars struct and token data.
1) Initializes a new node with the current token type
2) Links the new node to the existing list if there is one
3) Updates vars->current to point to the new node
Returns 1 on success, 0 on allocation failure.
Works with lexerlist().
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
Adds a child node to a parent node in the AST.
Follows a left-to-right priority when adding children:
1) If the left child position is empty, adds the child there
2) Otherwise, if the right child position is empty, adds it there
3) Does nothing if both positions are already filled
Both arguments must be non-NULL for operation to proceed.
Works with build_ast().
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
Handles pipe node insertion during AST construction.
Creates a new pipe node and restructures the tree accordingly.
Two cases:
1) If root is NULL: Create new pipe node as root with
   previous node as left child and next node as right child
2) If root exists: Create new pipe node with current root as left child,
   pipe_node->next as right child, and make this new node the root
This implements the shell's pipe operator (cmd1 | cmd2).
Works with build_ast().
*/
void handle_pipe_node(t_node **root, t_node *pipe_node)
{
	t_node *new_pipe;

	new_pipe = initnode(TYPE_PIPE, "|");
	if (!new_pipe)
		return;
		
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
Handles redirection node insertion during AST construction.
Finds the appropriate command node to attach the redirection to by:
1) Starting from root and traversing right through pipe nodes
2) When a non-pipe node is found, attach the redirection:
   - If cmd_node->right is empty, put redirection there
   - Otherwise, push existing right child down and insert redirection
This ensures redirections are properly associated with their commands.
Works with build_ast().
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
