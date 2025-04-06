/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_a.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:56 by bleow             #+#    #+#             */
/*   Updated: 2025/04/06 09:21:37 by bleow            ###   ########.fr       */
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
		free(env[n]);
	}
	free(env);
}



/*
Cleanup pipes struct variables.
- Takes a pointer to a pipes structure.
- Safely frees the structure itself.
- Prevents memory leaks after pipeline processing.
*/
void	cleanup_pipes(t_pipe *pipes)
{
	if (!pipes)
		return ;
	free(pipes->pipe_fds);
	free(pipes->pids);
	free(pipes->status);
	if (pipes->heredoc_fd > 2)
		close(pipes->heredoc_fd);
	if (pipes->redirection_fd > 2)
        close(pipes->redirection_fd);
	free(pipes);
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
	if (vars->pipes)
		cleanup_pipes(vars->pipes);
	vars->pipes = NULL;
	rl_clear_history();
}
