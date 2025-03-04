/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/04 11:45:08 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Skips whitespace characters specifically space, \t, and \n.
Advances vars->pos to the next non-whitespace character.
For skipping whitespace between tokens.
Works with lexerlist().
*/
void	skip_whitespace(char *str, t_vars *vars)
{
	while (str[vars->pos] && (str[vars->pos] == ' '
			|| str[vars->pos] == '\t' || str[vars->pos] == '\n'))
		vars->pos++;
}

/*
Identifies and processes different token types in the input string.
Handles these specific cases:
1. Quoted content (single or double quotes)
2. Expansion variables (starting with $)
3. Operators (like pipes and redirections)
Advances vars->pos past the processed token.
If no special token is found, advances to next character.
Works with lexerlist().
*/
void	handle_token(char *str, t_vars *vars)
{
	int	token_start;

	token_start = vars->pos;
	if (ft_isquote(str[vars->pos]))
		handle_quote_token(str, vars, &vars->pos);
	else if (str[vars->pos] == '$')
		handle_expansion(str, &vars->pos, vars);
	else
		vars->pos = operators(str, vars->pos, token_start, vars);
	if (vars->pos == token_start && str[vars->pos])
		vars->pos++;
}

/*
Main controller function for lex analysis.
Takes an input string and converts it into a linked list of tokens.
Steps:
1. Initializes position tracking and creates a head node
2. Processes tokens until end of input is reached, while skipping any
   whitespaces between tokens using skip_whitespace().
3. Calls handle_token() to process each token.
4. Handles any unclosed quotes at the end of input with
   handle_unclosed_quotes().
Result: vars->current populated with the token list.
Works with tokenize().
*/
void	lexerlist(char *str, t_vars *vars)
{
	vars->pos = 0;
	vars->current = NULL;
	vars->current = init_head_node(vars);
	if (!vars->current)
	{
		ft_error(vars);
		return ;
	}
	while (str && str[vars->pos])
	{
		skip_whitespace(str, vars);
		if (str[vars->pos])
			handle_token(str, vars);
	}
	if (vars->quote_depth > 0 && str)
		str = handle_unclosed_quotes(str, vars);
}
