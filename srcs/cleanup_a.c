/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_a.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:56 by bleow             #+#    #+#             */
/*   Updated: 2025/03/23 02:56:45 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Frees a partially allocated environment array up to index n-1.
- Takes the environment array and number of elements to free.
- Safely frees each string element, then frees the array itself.
- Prevents memory leaks during initialization errors.
Works with ft_free_2d() and dup_env().
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
Cleanup AST struct variables.
- Takes a pointer to an AST structure.
- Safely frees the structure itself.
- Prevents memory leaks after AST processing.
Works with init_ast_struct().
*/
void	cleanup_ast_struct(t_ast *ast)
{
	if (!ast)
		return ;
	ft_safefree((void **)&ast);
}

/*
Cleanup pipeline struct variables.
- Takes a pointer to a pipeline structure.
- Safely frees the structure itself.
- Prevents memory leaks after pipeline processing.
*/
void	cleanup_pipeline(t_pipe *pipeline)
{
	if (!pipeline)
		return ;
	ft_safefree((void **)&pipeline->exec_cmds);
	ft_safefree((void **)&pipeline->pipe_fds);
	ft_safefree((void **)&pipeline->pids);
	ft_safefree((void **)&pipeline->status);
	if (pipeline->heredoc_fd > 2)
		close(pipeline->heredoc_fd);
	if (pipeline->redirection_fd > 2)
        close(pipeline->redirection_fd);
	ft_safefree((void **)&pipeline);
}

void	cleanup_vars(t_vars *vars)
{
	int	env_count;

	if (!vars)
		return;
	if (vars->env)
	{
		env_count = ft_arrlen(vars->env);
		ft_free_2d(vars->env, env_count);
		vars->env = NULL;
	}
    if (vars->error_msg != NULL)
		ft_safefree((void **)&vars->error_msg);
	if (vars->astroot)
		cleanup_ast(vars->astroot);
	if (vars->head && vars->head != vars->astroot)
		cleanup_ast(vars->head);
	vars->astroot = NULL;
	vars->head = NULL;
	vars->current = NULL;
	vars->quote_depth = 0;
	vars->error_code = 0;
}

void	cleanup_exit(t_vars *vars)
{
	if (!vars)
		return ;
	save_history();
	cleanup_token_list(vars);
	cleanup_vars(vars);
	if (vars->pipeline)
		cleanup_pipeline(vars->pipeline);
	vars->pipeline = NULL;
	rl_clear_history();
}
