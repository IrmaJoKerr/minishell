/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   improved_tokenize.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 16:42:44 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 21:23:15 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Initialize tokenizer state
- Resets position counters
- Clears quote depth
- Resets heredoc state
*/
static void	initialize_tokenizer(t_vars *vars)
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
static int	process_heredoc_delimiter(char *input, t_vars *vars, int *hd_is_delim)
{
	char	*raw_delimiter_str;
	int		moves;

	if (ft_isspace(input[vars->pos]))
	{
		vars->pos++;
		vars->start = vars->pos;
		return (2);
	}
	vars->start = vars->pos;
	while (input[vars->pos] && !ft_isspace(input[vars->pos])
		&& !is_operator_token(get_token_at(input, vars->pos, &moves)))
	{
		vars->pos++;
	}
	if (vars->pos == vars->start)
	{
		tok_syntax_error_msg("newline", vars);
		return (0);
	}
	raw_delimiter_str = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!raw_delimiter_str)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	if (!is_valid_delim(raw_delimiter_str, vars))
	{
		free(raw_delimiter_str);
		return (0);
	}
	debug_token_creation("process_heredoc_delimiter:raw_delimiter", raw_delimiter_str, TYPE_ARGS, vars);
	maketoken(raw_delimiter_str, TYPE_ARGS, vars);
	free(raw_delimiter_str);
	*hd_is_delim = 0;
	vars->start = vars->pos;
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
static int	handle_token(char *input, t_vars *vars,
		int *hd_is_delim)
{
	t_tokentype	token_type;
	int			moves;
	int			adj_saved[3];

	ft_memcpy(adj_saved, vars->adj_state, sizeof(adj_saved));
	token_type = get_token_at(input, vars->pos, &moves);
	if (token_type == TYPE_SINGLE_QUOTE || token_type == TYPE_DOUBLE_QUOTE)
	{
		ft_memcpy(vars->adj_state, adj_saved, sizeof(adj_saved));
		tokenize_quote(input, vars);
		return (1);
	}
	if (input[vars->pos] == '$' && !vars->quote_depth)
	{
		ft_memcpy(vars->adj_state, adj_saved, sizeof(adj_saved));
		tokenize_expan(input, vars);
		return (1);
	}
	if (is_operator_token(token_type))
	{
		handle_text(input, vars);
		if (token_type == TYPE_HEREDOC)
		{
			debug_token_creation("handle_token:heredoc", "<<", TYPE_HEREDOC, vars);
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
static int	finalize_tokenization(char *input, t_vars *vars, int hd_is_delim)
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
Tokenizes input string. Calls delimiter validation when << is found.
Returns 1 on success, 0 on failure (syntax error or malloc error).
*/
int	improved_tokenize(char *input, t_vars *vars)
{
	int	hd_is_delim;
	int	result;

	initialize_tokenizer(vars);
	hd_is_delim = 0;
	while (input && input[vars->pos])
	{
		vars->next_flag = 0;
		if (hd_is_delim)
		{
			result = process_heredoc_delimiter(input, vars, &hd_is_delim);
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
		if (vars->next_flag)
			continue ;
		if (!hd_is_delim)
			vars->pos++;
	}
	return (finalize_tokenization(input, vars, hd_is_delim));
}
