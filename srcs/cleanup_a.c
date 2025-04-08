/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_a.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:56 by bleow             #+#    #+#             */
/*   Updated: 2025/04/08 23:20:39 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Frees a partially allocated environment array up to index n-1.
- Takes the environment array and number of elements to free.
- Prevents memory leaks during initialization errors.
Works with make_sorted_env() and builtin_export().
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
Works with cleanup_exit().
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

/*
Cleans up the vars structure.
- Frees environment variables array and its contents
- Cleans up the AST from root
- Handles separate cleanup if head is different from AST root
- Resets all pointers to NULL and state variables to 0
- Preserves any status codes for later use
Works with cleanup_exit() and reset_shell().
*/
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

/*
Performs complete program cleanup before exit.
- Saves command history to persistent storage
- Cleans up token list and AST structures
- Frees all variable and environment resources
- Releases pipeline and file descriptor resources
- Clears readline history from memory
Works with builtin_exit().
*/
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

/*
Clean up file descriptors after command execution.
- Closes input and output file descriptors.
- Avoids closing standard descriptors (0, 1, 2).
- Prevents descriptor leaks after redirections.
Works with exec_redirect_cmd().
*/
void	cleanup_fds(int fd_in, int fd_out)
{
	if (fd_in > 2 && fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out > 2 && fd_out != STDOUT_FILENO)
		close(fd_out);
}
