/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 02:20:54 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 06:10:19 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Initialize a new AST state structure.
Allocates and sets up the AST building state.
*/
t_ast	*init_ast_struct(void)
{
	t_ast	*ast;

	ast = (t_ast *)malloc(sizeof(t_ast));
	if (!ast)
		return (NULL);
	ft_memset(ast, 0, sizeof(t_ast));
	ast->current = NULL;
	ast->last_cmd = NULL;
	ast->last_heredoc = NULL;
	ast->cmd_redir = NULL;
	ast->pipe_root = NULL;
	ast->root = NULL;
	ast->cmd_idx = 0;
	ast->syntax_error = 0;
	ast->serial_pipes = 0;
	ast->pipe_at_front = 0;
	ast->pipe_at_end = 0;
	ast->fd_write = 0;
	ast->expand_vars = 0;
	return (ast);
}

/*
Initialize the lexer state.
Reset position trackers and node pointers.
*/
void	init_lexer(t_vars *vars)
{
	if (!vars)
		return ;
	vars->head = NULL;
	vars->current = NULL;
	vars->curr_type = TYPE_NULL;
	vars->prev_type = TYPE_NULL;
	vars->pos = 0;
	vars->start = 0;
	vars->quote_depth = 0;
}

/*
Initialise pipes structure.
*/
t_pipe *init_pipes(void)
{
    t_pipe *pipes;
    
    pipes = (t_pipe *)malloc(sizeof(t_pipe));
    if (!pipes)
        return (NULL);
    // Initialize all fields
    pipes->pipe_count = 0;
    pipes->exec_cmds = NULL;
    pipes->pipe_fds = NULL;
    pipes->pids = NULL;
    pipes->status = NULL;
    pipes->saved_stdin = -1;
    pipes->saved_stdout = -1;
    pipes->heredoc_fd = -1;
    pipes->redirection_fd = -1;
    pipes->root_node = NULL;
    pipes->append_mode = 0;
    pipes->current_redirect = NULL;
    return (pipes);
}
