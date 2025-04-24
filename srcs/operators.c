/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:13:52 by bleow             #+#    #+#             */
/*   Updated: 2025/04/24 15:54:00 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* 
Checks if a token is an operator (pipe or redirection).
Returns 1 if token is an operator, 0 otherwise.
*/
int is_operator_token(t_tokentype type)
{
	if (type == TYPE_PIPE)
		return (1);
	if (type == TYPE_IN_REDIRECT)
		return (1);
	if (type == TYPE_OUT_REDIRECT)
		return (1);
	if (type == TYPE_APPEND_REDIRECT)
		return (1);
	if (type == TYPE_HEREDOC)
		return (1);
	return (0);
}

/*
Creates string token for text preceding an operator.
- Checks if current position contains operator character.
- Creates a string token from token_start to current position.
- Updates token tracking in vars structure.
Returns:
- Current position (unchanged).
*/
void handle_string(char *input, t_vars *vars)
{
	char		*token;
	int			moves;
	t_tokentype	token_type;
	
	if (vars->pos > vars->start)
	{
		token = ft_substr(input, vars->start, vars->pos - vars->start);
		if (!token)
			return ;
		token_type = get_token_at(token, 0, &moves);
		if (token_type == 0)
		{
			set_token_type(vars, token);
		}
		else
		{
			vars->curr_type = token_type;
		}
		maketoken(token, vars->curr_type, vars);
		free(token);
		vars->start = vars->pos;
	}
}

/* 
Checks if character at position is a single-character token
Returns the token type enum value, 0 if not a special token
Handles: ', ", <, >, $, |
*/
int	is_single_token(char *input, int pos, int *moves)
{
	t_tokentype	token_type;

	token_type = 0;
	*moves = 0;
	if (!input || !input[pos])
		return (token_type);
	if (input[pos] == '\'')
		token_type = TYPE_SINGLE_QUOTE;
	else if (input[pos] == '\"')
		token_type = TYPE_DOUBLE_QUOTE;
	else if (input[pos] == '<')
		token_type = TYPE_IN_REDIRECT;
	else if (input[pos] == '>')
		token_type = TYPE_OUT_REDIRECT;
	else if (input[pos] == '$')
		token_type = TYPE_EXPANSION;
	else if (input[pos] == '|')
		token_type = TYPE_PIPE;
	if (token_type != 0)
	{
		*moves = 1;
	}
	return (token_type);
}

/* 
Checks if characters at position form a double-character token
Returns the token type enum value, 0 if not a double token
Handles: >>, <<, $?
*/
int	is_double_token(char *input, int pos, int *moves)
{
	t_tokentype	token_type;

	token_type = 0;
	*moves = 0;
	if (!input || !input[pos] || !input[pos + 1])
		return (token_type);
	if (input[pos] == '>' && input[pos + 1] == '>')
		token_type = TYPE_APPEND_REDIRECT;
	else if (input[pos] == '<' && input[pos + 1] == '<')
		token_type = TYPE_HEREDOC;
	else if (input[pos] == '$' && input[pos + 1] == '?')
		token_type = TYPE_EXIT_STATUS;
	if (token_type != 0)
	{
		*moves = 2;
	}
	return (token_type);
}

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
		return token_type;
	}
	token_type = is_single_token(input, pos, moves);
	if (token_type != 0)
	{
		return token_type;
	}
	*moves = 1;
	return (0);
}

/*
Processes single-character operators (|, >, <).
Creates token with the provided token type.
Updates position tracking.
Returns:
- Position after operator (i+1).
- Unchanged position if token creation fails.
*/
int	handle_single_operator(char *input, t_vars *vars)
{
	char	*token;
	
	token = ft_substr(input, vars->pos, 1);
	if (!token)
		return (vars->pos);
	maketoken(token, vars->curr_type, vars);
	free(token);
	vars->pos++;
	vars->start = vars->pos;
	vars->prev_type = vars->curr_type;
	return (vars->pos);
}

/*
Processes double-character operators (>>, <<).
Creates token with the provided token type.
Updates position tracking.
Returns:
 - Position after operator (i+moves).
 - Unchanged position if token creation fails.
*/
int	handle_double_operator(char *input, t_vars *vars)
{
	char	*token;
	int		moves;
	
	moves = 2;
	token = ft_substr(input, vars->pos, moves);
	if (!token)
		return (vars->pos);
	maketoken(token, vars->curr_type, vars);
	free(token);
	vars->pos += moves;
	vars->start = vars->pos;
	vars->prev_type = vars->curr_type;
	return (vars->pos);
}
