/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:43:35 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:34:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Function to initialize the head node for the AST.
*/
t_node	*init_head_node(t_vars *vars)
{
	t_node	*head;

	if (!vars)
		return (NULL);
	head = malloc(sizeof(t_node));
	if (!head)
	{
		ft_error(vars);
		return (NULL);
	}
	head->type = TYPE_HEAD;
	head->args = NULL;
	head->next = NULL;
	head->prev = NULL;
	head->left = NULL;
	head->right = NULL;
	return (head);
}

/*
Function to build AST from tokens.
Builds pipe chains and redirections based on type using 2 while loops.
*/
t_node *build_ast(t_vars *vars)
{
	t_node	*current;
	t_node	*root;

	if (!vars || !vars->head)
		return (NULL);
	current = vars->head;
	root = NULL;
	while (current)
	{
		if (current->type == TYPE_PIPE)
			handle_pipe_node(&root, current);
		current = current->next;
	}
	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
			redirection_node(root, current);
		current = current->next;
	}
	return (root);
}

/*
Function to initialize a new node.
*/
t_node	*initnode(t_tokentype type, char *token)
{
	t_node	*node;

	if (!token)
		return (NULL);
	node = malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->args = malloc(sizeof(char *) * 2);
	if (!node->args)
	{
		free(node);
		return (NULL);
	}
	node->args[0] = ft_strdup(token);
	node->args[1] = NULL;
	node->next = NULL;
	node->prev = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}
