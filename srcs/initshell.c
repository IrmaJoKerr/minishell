/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 02:20:54 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 23:33:30 by bleow            ###   ########.fr       */
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
	init_lexer(vars);
	vars->pipes = init_pipes();
    if (!vars->pipes)
	{
        crit_error(vars);
	}
	vars->error_code = 0;
	load_signals();
	load_history();
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
    ft_memset(vars->quote_ctx, 0, sizeof(vars->quote_ctx));
    ft_memset(vars->cmd_nodes, 0, sizeof(vars->cmd_nodes));
    vars->cmd_count = 0;
    vars->astroot = NULL;
    vars->partial_input = NULL;
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
    pipes->redirection_fd = -1;
    return (pipes);
}

/*
Initialize a new AST state structure.
Allocates and sets up the AST building state.
*/
t_ast	*init_ast(void)
{
	t_ast	*ast;

	ast = (t_ast *)malloc(sizeof(t_ast));
	if (!ast)
		return (NULL);
	ft_memset(ast, 0, sizeof(t_ast));
	return (ast);
}

/*
Reset the shell state.
- Cleans up the token list.
- Re-initializes the lexer state.
- Resets the pipes state.
- Frees the partial input buffer.
- Cleans up the AST root node.
*/
void	reset_shell(t_vars *vars)
{
    if (!vars)
        return ;
    cleanup_token_list(vars);
	if (vars->partial_input)
        free(vars->partial_input);
    if (vars->pipes)
    {
        reset_redirect_fds(vars);
        vars->pipes->pipe_count = 0;
        vars->pipes->out_mode = OUT_MODE_NONE;
        vars->pipes->current_redirect = NULL;
        vars->pipes->root_node = NULL;
    }
	init_lexer(vars);
}
