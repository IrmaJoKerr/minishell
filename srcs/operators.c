/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:13:52 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 23:42:03 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* 
Checks if a token is an operator (pipe or redirection).
Returns 1 if token is an operator, 0 otherwise.
*/
int	is_operator_token(t_tokentype type)
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
void	handle_string(char *input, t_vars *vars)
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
