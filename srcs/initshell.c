/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 02:20:54 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 22:51:05 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Initializes the shell environment and variables.
- Sets up signal handlers.
- Initializes environment variables.
- Sets up shell history.
- Prepares the command prompt.
Works with main() as program entry point.
*/
void	init_shell(t_vars *vars, char **envp)
{
	ft_memset(vars, 0, sizeof(t_vars));
	vars->env = dup_env(envp);
	if (!vars->env)
	{
		crit_error(vars);
	}
	get_shell_level(vars);
	incr_shell_level(vars);
	init_vars(vars);
	vars->pipes = init_pipes();
	if (!vars->pipes)
	{
		crit_error(vars);
	}
	setup_terminal_mode(vars);
	vars->error_code = 0;
	load_signals();
	load_history();
}

/*
Initialize the lexer state.
Reset position trackers and node pointers.
*/
void	init_vars(t_vars *vars)
{
	if (!vars)
		return ;
	vars->partial_input = NULL;
	vars->next_flag = 0;
	ft_memset(vars->cmd_nodes, 0, sizeof(vars->cmd_nodes));
	vars->cmd_count = 0;
	ft_memset(vars->quote_ctx, 0, sizeof(vars->quote_ctx));
	vars->quote_depth = 0;
	vars->adj_state[0] = 0;
	vars->adj_state[1] = 0;
	vars->adj_state[2] = -1;
	vars->find_start = NULL;
	vars->find_tgt = NULL;
	vars->find_mode = 0;
	vars->current = NULL;
	vars->head = NULL;
	vars->astroot = NULL;
	vars->curr_type = TYPE_NULL;
	vars->prev_type = TYPE_NULL;
	vars->pos = 0;
	vars->start = 0;
	vars->hd_text_ready = 0;
}

/*
Initialise pipes structure.
Allocates and sets up the pipes state.
*/
t_pipe	*init_pipes(void)
{
	t_pipe	*pipes;

	pipes = (t_pipe *)malloc(sizeof(t_pipe));
	if (!pipes)
		return (NULL);
	ft_memset(pipes, 0, sizeof(t_pipe));
	pipes->saved_stdin = -1;
	pipes->saved_stdout = -1;
	pipes->heredoc_fd = -1;
	pipes->heredoc_delim = NULL;
	pipes->hd_expand = 0;
	pipes->redirection_fd = -1;
	pipes->last_cmd = NULL;
	pipes->last_pipe = NULL;
	pipes->pipe_root = NULL;
	pipes->redir_root = NULL;
	pipes->last_in_redir = NULL;
	pipes->last_out_redir = NULL;
	pipes->cmd_redir = NULL;
	return (pipes);
}

/*
Resets all pipeline and redirection tracking variables.
Works with reset_shell().
*/
void	reset_pipe_vars(t_vars *vars)
{
	if (!vars || !vars->pipes)
		return ;
	vars->pipes->pipe_count = 0;
	vars->pipes->out_mode = OUT_MODE_NONE;
	vars->pipes->current_redirect = NULL;
	vars->pipes->last_cmd = NULL;
	vars->pipes->last_pipe = NULL;
	vars->pipes->pipe_root = NULL;
	vars->pipes->redir_root = NULL;
	vars->pipes->last_in_redir = NULL;
	vars->pipes->last_out_redir = NULL;
	vars->pipes->cmd_redir = NULL;
}

/*
Reset the shell state.
- Cleans up the token list.
- Frees the partial input buffer.
- Cleans up heredoc lines and counts.
- Resets the pipes state.
- Resets vars struct.
*/
void	reset_shell(t_vars *vars)
{
	if (!vars)
		return ;
	cleanup_token_list(vars);
	if (vars->partial_input)
		free(vars->partial_input);
	vars->partial_input = NULL;
	if (vars->pipes)
	{
		reset_redirect_fds(vars);
		reset_pipe_vars(vars);
	}
	init_vars(vars);
}
