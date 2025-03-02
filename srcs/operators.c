/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:13:52 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:18 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Handles operators. Used for handling operators and strings. Works with 
*/
int	operators(char *input, int i, int token_start, t_vars *vars)
{
	int next_pos;

	handle_string(input, i, token_start, vars);
	next_pos = handle_double_operator(input, i, vars);
	if (next_pos != i)
		return (next_pos);
	return (handle_single_operator(input, i, vars));
}

/*
Handles string tokens. 
*/
int	handle_string(char *input, int i, int token_start, t_vars *vars)
{
	if ((input[i] == '>' || input[i] == '<' || input[i] == '|')
		&& i > token_start)
	{
		vars->start = token_start;
		vars->pos = i;
		vars->curr_type = TYPE_STRING;
		maketoken(input, vars);
	}
	return (i);
}

/*
Handles single operators. Used for handling '|', '>', and '<'.
*/
int	handle_single_operator(char *input, int i, t_vars *vars)
{
	if (input[i] == '|' || input[i] == '>' || input[i] == '<')
	{
		vars->start = i;
		vars->pos = i + 1;
		vars->curr_type = get_operator_type(input[i]);
		maketoken(input, vars);
		return (i + 1);
	}
	return (i);
}

/*
Handles double operators. Used for handling '>>' and '<<'.
*/
int	handle_double_operator(char *input, int i, t_vars *vars)
{
	if (input[i] == '>' && input[i + 1] == '>')
	{
		vars->start = i;
		vars->pos = i + 2;
		vars->curr_type = TYPE_APPEND_REDIRECT;
		maketoken(input, vars);
		return (i + 2);
	}
	if (input[i] == '<' && input[i + 1] == '<')
	{
		vars->start = i;
		vars->pos = i + 2;
		vars->curr_type = TYPE_HEREDOC;
		maketoken(input, vars);
		return (i + 2);
	}
	return (i);
}
