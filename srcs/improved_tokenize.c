/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   improved_tokenize.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 16:42:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 22:13:45 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Initialize tokenizer state
- Resets position counters
- Clears quote depth
- Resets heredoc state
*/
void	init_tokenizer(t_vars *vars)
{
	vars->pos = 0;
	vars->start = 0;
	vars->quote_depth = 0;
	if (vars->pipes->heredoc_delim)
	{
		free(vars->pipes->heredoc_delim);
		vars->pipes->heredoc_delim = NULL;
	}
	vars->pipes->hd_expand = 0;
}

/*
Extracts raw delimiter string from input
- Sets start position at current position
- Advances position until delimiter end is found
- Validates non-empty delimiter
- Handles memory allocation
Returns:
- Raw delimiter string on success
- NULL on error (with error_code set)
*/
char	*get_delim_str(char *input, t_vars *vars, int *error_code)
{
	int		moves;
	char	*ori_delim_str;

	vars->start = vars->pos;
	while (input[vars->pos] && !ft_isspace(input[vars->pos])
		&& !is_operator_token(get_token_at(input, vars->pos, &moves)))
	{
		vars->pos++;
	}
	if (vars->pos == vars->start)
	{
		tok_syntax_error_msg("newline", vars);
		*error_code = 1;
		return (NULL);
	}
	ori_delim_str = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!ori_delim_str)
	{
		vars->error_code = ERR_DEFAULT;
		*error_code = 1;
		return (NULL);
	}
	return (ori_delim_str);
}

/*
Process heredoc delimiter when expected
- Skips whitespace before delimiter
- Extracts and validates delimiter string
- Creates token for the delimiter
- Updates state after processing
Returns:
- 0 on error
- 1 on success with next_flag set (continue)
- 2 on waiting for more input (whitespace skipping)
*/
int	proc_hd_delim(char *input, t_vars *vars, int *hd_is_delim)
{
	char	*ori_delim_str;
	int		error_code;

	error_code = 0;
	if (ft_isspace(input[vars->pos]))
	{
		vars->pos++;
		vars->start = vars->pos;
		return (2);
	}
	ori_delim_str = get_delim_str(input, vars, &error_code);
	if (error_code || !ori_delim_str)
		return (0);
	if (!is_valid_delim(ori_delim_str, vars))
	{
		free(ori_delim_str);
		return (0);
	}
	maketoken(ori_delim_str, TYPE_ARGS, vars);
	free(ori_delim_str);
	*hd_is_delim = 0;
	vars->start = vars->pos;
	vars->next_flag = 1;
	return (1);
}

/*
Handle quote tokens preserving adjacency state
- Restores saved adjacency state before quote processing
- Processes quoted text through tokenize_quote
Returns:
- 1 on success
*/
int	handle_quotes(char *input, t_vars *vars, int *adj_saved)
{
	ft_memcpy(vars->adj_state, adj_saved, sizeof(int) * 3);
	tokenize_quote(input, vars);
	return (1);
}

/*
Process operators and update token state
- Handles heredoc operators specially
- Processes other operators through process_operator_char
- Updates state for next token processing
Returns:
- 1 on success
- 0 on error
*/
int	proc_opr_token(char *input, t_vars *vars, int *hd_is_delim, t_tokentype token_type)
{
	handle_text(input, vars);
	if (token_type == TYPE_HEREDOC)
	{
		maketoken("<<", TYPE_HEREDOC, vars);
		vars->pos += 2;
		vars->start = vars->pos;
		*hd_is_delim = 1;
	}
	else
	{
		if (!process_operator_char(input, &vars->pos, vars))
			return (0);
	}
	vars->next_flag = 1;
	return (1);
}

/*
Handle token processing based on token type
- Preserve adjacency state across function boundaries
- Route to appropriate specialized handlers
- Process operators, quotes, expansions and whitespace
Returns:
- 1 on success
- 0 on error
*/
int	handle_token(char *input, t_vars *vars, int *hd_is_delim)
{
	t_tokentype	token_type;
	int			moves;
	int			adj_saved[3];

	ft_memcpy(adj_saved, vars->adj_state, sizeof(adj_saved));
	token_type = get_token_at(input, vars->pos, &moves);
	if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
		return (handle_quotes(input, vars, adj_saved));
	if (input[vars->pos] == '$' && !vars->quote_depth)
	{
		ft_memcpy(vars->adj_state, adj_saved, sizeof(adj_saved));
		tokenize_expan(input, vars);
		return (1);
	}
	if (is_operator_token(token_type))
		return (proc_opr_token(input, vars, hd_is_delim, token_type));
	if (ft_isspace(input[vars->pos]))
	{
		tokenize_white(input, vars);
		return (1);
	}
	return (1);
}

/*
Finalize tokenization process
- Check for unterminated heredoc
- Process any remaining text
- Clean up and return status
Returns:
- 1 on success
- 0 on error
*/
int	finish_tokenizing(char *input, t_vars *vars, int hd_is_delim)
{
	if (hd_is_delim)
	{
		tok_syntax_error_msg("newline", vars);
		return (0);
	}
	handle_text(input, vars);
	return (1);
}

/*
Check token position handling
- Increments position counter if not in heredoc mode
Returns:
-1 if next_flag is set (to continue main loop).
- 0 to continue normal processing.
*/
int	chk_move_pos(t_vars *vars, int hd_is_delim)
{
	if (vars->next_flag)
		return (1);
	if (!hd_is_delim)
		vars->pos++;
	return (0);
}

/*
Handle token processing based on token type
- Preserve adjacency state across function boundaries
- Route to appropriate specialized handlers
- Process operators, quotes, expansions and whitespace
Returns:
- 1 on success.
- 0 on error.
*/
int	tokenizer(char *input, t_vars *vars)
{
	int	hd_is_delim;
	int	result;

	init_tokenizer(vars);
	hd_is_delim = 0;
	while (input && input[vars->pos])
	{
		vars->next_flag = 0;
		if (hd_is_delim)
		{
			result = proc_hd_delim(input, vars, &hd_is_delim);
			if (result == 0)
				return (0);
			if (result == 1)
				continue ;
		}
		if (!vars->next_flag && !hd_is_delim)
		{
			if (!handle_token(input, vars, &hd_is_delim))
				return (0);
		}
		if (chk_move_pos(vars, hd_is_delim))
			continue ;
	}
	return (finish_tokenizing(input, vars, hd_is_delim));
}
