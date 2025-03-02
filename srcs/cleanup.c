/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 08:09:44 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:34:37 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Cleanup function to free allocated memory for proper exit.
*/
void	cleanup_vars(t_vars *vars)
{
	if (!vars)
		return;
	if (vars->env)
		ft_free_2d(vars->env, ft_arrlen(vars->env));
	if (vars->error_msg)
		free(vars->error_msg);
	if (vars->astroot)
		free_ast(vars->astroot);
	if (vars->head && vars->head != vars->astroot)
		free_ast(vars->head);
	vars->quote_depth = 0;
	vars->error_code = 0;
	vars->current = NULL;
}

/*
Free the abstract syntax tree(AST).
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
