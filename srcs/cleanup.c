/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 08:09:44 by bleow             #+#    #+#             */
/*   Updated: 2025/03/04 12:07:26 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Cleanup function to free allocated memory in s_vars struct for proper exit.
Uses :
1) ft_safefree() to prevent double free.
2) free_ast() to free the abstract syntax tree.
3) ft_arrlen() to get the length of the environment array.
4) ft_free_2d() to free the 2d environment variables array.
5) Resets all variables in s_vars struct to NULL or 0.
*/
void	cleanup_vars(t_vars *vars)
{
	if (!vars)
		return ;
	if (vars->env)
		ft_free_2d(vars->env, ft_arrlen(vars->env));
	ft_safefree((void **)&vars->error_msg);
	if (vars->astroot)
		free_ast(vars->astroot);
	if (vars->head && vars->head != vars->astroot)
		free_ast(vars->head);
	vars->astroot = NULL;
	vars->head = NULL;
	vars->current = NULL;
	vars->env = NULL;
	vars->quote_depth = 0;
	vars->error_code = 0;
}

/*
Recursively free the abstract syntax tree (AST) nodes.
This function:
1) Recursively frees left and right child nodes first
2) Frees any argument arrays associated with the node
3) Finally frees the node itself
*/
void	free_ast(t_node *node)
{
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->args)
		ft_free_2d(node->args, ft_arrlen(node->args));
	free(node);
}
