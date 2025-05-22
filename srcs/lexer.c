/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 17:30:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* 
Master function to get token type at current position
Checks double tokens first, then single tokens
Returns token type and updates position via moves parameter
*/
t_tokentype	get_token_at(char *input, int pos, int *moves)
{
	t_tokentype	token_type;

	token_type = 0;
	*moves = 0;
	token_type = is_double_token(input, pos, moves);
	if (token_type != 0)
	{
		return (token_type);
	}
	token_type = is_single_token(input, pos, moves);
	if (token_type != 0)
	{
		return (token_type);
	}
	*moves = 1;
	return (0);
}

/*
Master function for lexing analysis.
Handle token processing based on token type.
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

	if (!input || !*input)
		return (0);
	init_tokenizer(vars);
	hd_is_delim = 0;
	while (input && input[vars->pos])
	{
		vars->next_flag = 0;
		if (hd_is_delim)
		{
			result = proc_hd_delim(input, vars, &hd_is_delim);
			if (result == 0)
			{
				return (0);
			}
			if (result == 1)
			{
				continue;
			}
		}
		if (!vars->next_flag && !hd_is_delim)
		{
			if (!handle_token(input, vars, &hd_is_delim))
			{
				return (0);
			}
		}
		if (chk_move_pos(vars, hd_is_delim))
		{
			continue;
		}
	}
	return (finish_tokenizing(input, vars, hd_is_delim));
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
Extracts quoted content from input.
Sets the quote_type to TYPE_SINGLE_QUOTE or TYPE_DOUBLE_QUOTE.
Returns:
- The quoted string.
- NULL on error.
*/
char	*get_quoted_str(char *input, t_vars *vars, int *quote_type)
{
	int		end;
	char	quote_char;
	char	*content;

	quote_char = input[vars->pos];
	*quote_type = (quote_char == '\"') ? TYPE_DOUBLE_QUOTE : TYPE_SINGLE_QUOTE;
	vars->pos++;
	end = vars->pos;
	while (input[end] && input[end] != quote_char)
		end++;
	if (!input[end])
	{
		return (handle_quote_completion(input, vars));
	}
	content = ft_substr(input, vars->pos, end - vars->pos);
	vars->pos = end + 1;
	return (content);
}

/*
Processes operator characters in the input string.
- Identifies redirection operators and pipe.
- Creates tokens for these operators.
- Updates position past the operator.
Returns:
- 1 if operator was processed.
- 0 if otherwise.

Example: For input "cmd > file"
- Processes the '>' operator.
- Creates redirect token.
- Returns 1 to indicate operator was handled.
*/
int	process_operator_char(char *input, int *i, t_vars *vars)
{
	int			moves;
	t_tokentype	token_type;

	token_type = get_token_at(input, *i, &moves);
	if (token_type == 0)
	{
		return (0);
	}
	vars->curr_type = token_type;
	if (moves == 2)
	{
		handle_double_operator(input, vars);
	}
	else
	{
		handle_single_operator(input, vars);
	}
	return (1);
}
