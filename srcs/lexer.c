/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:08 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Skips whitespace characters. Works with lexerlist.
*/
void	skip_whitespace(char *str, t_vars *vars)
{
	while (str[vars->pos] && (str[vars->pos] == ' ' || 
		str[vars->pos] == '\t' || str[vars->pos] == '\n'))
		vars->pos++;
}

/*
Checks for quoted strings. Handles quoted tokens. Works with lexerlist.
*/
void	handle_token(char *str, t_vars *vars)
{
	int	token_start;

	token_start = vars->pos;
	if (is_quote(str[vars->pos]))
		handle_quote_token(str, vars, &vars->pos);
	else if (str[vars->pos] == '$')
		handle_expansion(str, &vars->pos, vars);
	else
		vars->pos = operators(str, vars->pos, token_start, vars);
}

/*
Controller function for the lexer. Works with tokenize.
*/
void	lexerlist(char *str, t_vars *vars)
{
	vars->pos = 0;
	vars->current = NULL;
	vars->current = init_head_node(vars);
	if (!vars->current)
		ft_error(vars);

	while (str[vars->pos])
	{
		skip_whitespace(str, vars);
		if (str[vars->pos])
			handle_token(str, vars);
	}
	if (vars->squoflag || vars->dquoflag)
		str = handle_unclosed_quotes(str, vars);
}
