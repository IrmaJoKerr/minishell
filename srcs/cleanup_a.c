/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_a.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:56 by bleow             #+#    #+#             */
/*   Updated: 2025/04/17 21:24:32 by bleow            ###   ########.fr       */
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
void cleanup_pipes(t_pipe *pipes)
{
	if (!pipes)
		return ;
	if (pipes->pipe_fds)
		free(pipes->pipe_fds);
	if (pipes->pids)
		free(pipes->pids);
	if (pipes->status)
		free(pipes->status);
	if (pipes->saved_stdin > 2)
		close(pipes->saved_stdin);
	if (pipes->saved_stdout > 2)
		close(pipes->saved_stdout);
	// Close heredoc pipe if open
    // if (pipes->heredoc_fd > 0)
	// {
    //     fprintf(stderr, "[DBG_HEREDOC] Closing heredoc pipe fd=%d during cleanup\n",
    //             pipes->heredoc_fd);
    //     close(pipes->heredoc_fd);
    // }
	if (pipes->heredoc_fd > 0)
    {
        fprintf(stderr, "[DBG_HEREDOC] Closing heredoc pipe fd=%d during cleanup\n",
                pipes->heredoc_fd);
        close(pipes->heredoc_fd);
    }
    
    // Close heredoc pipe write end if open
    if (pipes->hd_fd_write > 0 && pipes->hd_fd_write != pipes->heredoc_fd)
    {
        fprintf(stderr, "[DBG_HEREDOC] Closing heredoc write fd=%d during cleanup\n",
                pipes->hd_fd_write);
        close(pipes->hd_fd_write);
    }
    
    // Free heredoc delimiter if allocated
    if (pipes->heredoc_delim)
    {
        free(pipes->heredoc_delim);
        pipes->heredoc_delim = NULL;
    }
	if (pipes->redirection_fd > 2)
		close(pipes->redirection_fd);
	free(pipes);
}

/*
Cleans up the vars structure.
- If exists:
	- Frees pipes structure and it's contents
	- Frees env array
Works with cleanup_exit().
*/
void	cleanup_vars(t_vars *vars)
{
	int	env_count;

	if (!vars)
	{
		return ;
	}
	if (vars->pipes)
		cleanup_pipes(vars->pipes);
	vars->pipes = NULL;
	if (vars->env)
	{
		env_count = ft_arrlen(vars->env);
		ft_free_2d(vars->env, env_count);
		vars->env = NULL;
	}
}

/*
Performs complete program cleanup before exit.
- Saves command history to persistent storage
- Cleans up token list and AST structures
- Frees all vars,pipes, and env memory
- Clears readline history from memory
Works with builtin_exit().
*/
void cleanup_exit(t_vars *vars)
{
	if (!vars)
		return ;
	save_history();
	if (vars->head)
	{
		cleanup_token_list(vars);
	}
	cleanup_vars(vars);
	if (vars->partial_input)
	{
		free(vars->partial_input);
		vars->partial_input = NULL;
	}
	vars->head = NULL;
	vars->astroot = NULL;
	vars->current = NULL;
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
