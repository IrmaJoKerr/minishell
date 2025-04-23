/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_a.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:56 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 10:06:17 by bleow            ###   ########.fr       */
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
	if (pipes->heredoc_fd > 0)
		close(pipes->heredoc_fd);
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
	if (isatty(STDIN_FILENO) && vars->ori_term_saved)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &vars->ori_term_settings);
    }
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
	
	if (!vars || !vars->head)
		return ;
	current = vars->head;
	while (current)
	{
		next = current->next;
		free_token_node(current);
		current = next;
	}
	vars->head = NULL;
	vars->current = NULL;
}
