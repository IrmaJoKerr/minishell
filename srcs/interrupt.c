/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interrupt.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 00:47:11 by bleow             #+#    #+#             */
/*   Updated: 2025/06/12 23:41:26 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles comprehensive cleanup when heredoc is interrupted by signal.
- Cleans up token list and AST structures
- Resets shell state variables  
- Cleans up partial input buffers
- Resets pipe and redirection state
- Preserves error code for signal propagation
Called from process_hd_parent() when heredoc is interrupted.
*/
void	hd_abort_cleanup(t_vars *vars)
{
	int	preserved_error;

	preserved_error = vars->error_code;
	if (!vars)
		return ;
	if (vars->head)
	{
		free_null_token_stop(vars);
		cleanup_token_list(vars);
	}
	if (vars->partial_input)
	{
		ft_safefree((void **)&vars->partial_input);
		vars->partial_input = NULL;
	}
	reset_shell_state_for_interrupt(vars);
	vars->error_code = preserved_error;
	hd_abort_state_reset(vars);
}

/*
Cleans up heredoc-specific state after interruption.
- Resets heredoc readiness flag
- Cleans up delimiter if set
- Ensures temporary files are removed
*/
void	hd_abort_state_reset(t_vars *vars)
{
	if (!vars || !vars->pipes)
		return ;
	vars->hd_text_ready = 0;
	if (vars->pipes->heredoc_delim)
	{
		ft_safefree((void **)&vars->pipes->heredoc_delim);
		vars->pipes->heredoc_delim = NULL;
	}
	unlink(TMP_BUF);
	if (vars->pipes->hd_fd > 0)
	{
		close(vars->pipes->hd_fd);
		vars->pipes->hd_fd = -1;
	}
}

/*
Resets shell state variables specifically for interrupt scenarios.
- Resets lexer position and state tracking
- Clears quote and adjacency state
- Resets AST and node pointers
- Preserves environment and pipes structure
Works with hd_abort_cleanup() to handle signal interruption cleanup.
*/
void	reset_shell_state_for_interrupt(t_vars *vars)
{
	if (!vars)
		return ;
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
