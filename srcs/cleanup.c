/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 08:09:44 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 19:33:30 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <readline/readline.h>

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
		cleanup_ast(vars->astroot);
	if (vars->head && vars->head != vars->astroot)
		cleanup_ast(vars->head);
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
void	cleanup_ast(t_node *node)
{
	if (!node)
		return ;
	cleanup_ast(node->left);
	cleanup_ast(node->right);
	if (node->args)
		ft_free_2d(node->args, ft_arrlen(node->args));
	ft_safefree((void **)&node);
}

/*
Clean up the token list by freeing all nodes and resetting head/current pointers.
*/
void	cleanup_token_list(t_vars *vars)
{
	t_node *current;
	t_node *next;
	int	i;

	if (!vars || !vars->head)
		return ;
	current = vars->head;
	while (current)
	{
		next = current->next;
		if (current->args)
		{
			i = 0;
			while (current->args[i])
			{
				ft_safefree((void **)&current->args[i]);
				i++;
			}
			ft_safefree((void **)&current->args);
		}
		ft_safefree((void **)&current);
		current = next;
	}
	vars->head = NULL;
	vars->current = NULL;
	vars->astroot = NULL;
}

/*
Frees a partially allocated environment array up to index n-1.
*/
void	cleanup_env_error(char **env, int n)
{
	while (n > 0)
	{
		n--;
		ft_safefree((void **)&env[n]);
	}
	ft_safefree((void **)&env);
}

/*
Perform complete cleanup before program exit.
Frees all allocated resources in vars struct.
Should be called before any exit() call.
*/
void	cleanup_exit(t_vars *vars)
{
	if (!vars)
		return ;
	cleanup_token_list(vars);
	cleanup_vars(vars);
	ft_safefree((void **)&vars->error_msg);
}
