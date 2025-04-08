/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_b.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:50 by bleow             #+#    #+#             */
/*   Updated: 2025/04/08 23:25:17 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Recursively free the AST nodes.
- Traverses the tree in post-order (left, right, current).
- Frees all argument arrays associated with nodes.
- Ensures complete cleanup of complex command structures.
Works with cleanup_vars().

Example: For "ls -la | grep .c > output.txt":
- Recursively frees all nodes including command, pipe, and redirection
- Frees all argument arrays like ["ls", "-la"] and ["grep", ".c"]
*/
void	cleanup_ast(t_node *node)
{
	if (!node)
		return ;
	cleanup_ast(node->left);
	cleanup_ast(node->right);
	if (node->args)
		ft_free_2d(node->args, ft_arrlen(node->args));
	if (node->arg_quote_type)
		free(node->arg_quote_type);
	free(node);
}

/*
Free a single token node and its arguments.
- Takes a node pointer and frees its arguments array.
- Then frees the node itself.
- Used for individual node cleanup without recursion.
Works with cleanup_token_list().
*/
void	free_token_node(t_node *node)
{
    if (!node)
        return;
    if (node->args)
        ft_free_2d(node->args, ft_arrlen(node->args));
    if (node->arg_quote_type)
        free(node->arg_quote_type);
    free(node);
}

/*
Clean up the token list by freeing all nodes.
- Traverses the linked list of tokens.
- Frees each node and its arguments.
- Resets head and current pointers in vars.
- Called when processing a new command line.
Works with cleanup_exit().
*/
void	cleanup_token_list(t_vars *vars)
{
	t_node	*current;
	t_node	*next;
	int		count;

	count = 0;
	if (!vars || !vars->head)
		return ;
	current = vars->head;
	while (current)
	{
		next = current->next;
		free_token_node(current);
		current = next;
		count++;
	}
	vars->head = NULL;
	vars->current = NULL;
}
