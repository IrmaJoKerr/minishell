/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 02:20:54 by bleow             #+#    #+#             */
/*   Updated: 2025/03/23 03:02:12 by bleow            ###   ########.fr       */
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

t_exec	*init_exec_context(t_vars *vars)
{
	t_exec	*exec;

	exec = (t_exec *)malloc(sizeof(t_exec));
	if (!exec)
		return (NULL);
	ft_memset(exec, 0, sizeof(t_exec));
	exec->cmd_path = NULL;
	exec->pid = 0;
	exec->status = 0;
	exec->result = 0;
	exec->cmd = NULL;
	if (vars && vars->pipeline)
		exec->append = vars->pipeline->append_mode;
	else
		exec->append = 0;
	return (exec);
}

/*
Initialize resources for input verification.
Allocates memory and initializes AST structure.
Returns:
- AST structure pointer on success
- NULL on failure (with *cmd_ptr set to NULL)
*/
t_ast	*init_verify(char *input, char **cmd_ptr)
{
	t_ast	*ast;

	*cmd_ptr = ft_strdup(input);
	if (!*cmd_ptr)
		return (NULL);
	ast = init_ast_struct();
	if (!ast)
	{
		ft_safefree((void **)cmd_ptr);
		return (NULL);
	}
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

void	init_quote_context(t_vars *vars)
{
	int	i;

	i = 0;
	if (!vars)
		return ;
	vars->quote_depth = 0;
	while (i < 32)
		vars->quote_ctx[i++].type = 0;
}

/*
Initialise pipeline structure.
*/
t_pipe *init_pipeline(void)
{
    t_pipe *pipeline;
    
    pipeline = (t_pipe *)malloc(sizeof(t_pipe));
    if (!pipeline)
        return (NULL);
    // Initialize all fields
    pipeline->pipe_count = 0;
    pipeline->exec_cmds = NULL;
    pipeline->cmd_count = 0;
    pipeline->pipe_fds = NULL;
    pipeline->pids = NULL;
    pipeline->status = NULL;
    pipeline->saved_stdin = -1;
    pipeline->saved_stdout = -1;
    pipeline->heredoc_fd = -1;
    pipeline->redirection_fd = -1;
    pipeline->root_node = NULL;
    pipeline->append_mode = 0;
    pipeline->current_redirect = NULL;
    // Initialize all cmd_nodes to NULL
    ft_memset(pipeline->cmd_nodes, 0, sizeof(pipeline->cmd_nodes));
    return (pipeline);
}
