/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 01:51:28 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Main token creation function. Creates a new token from the input string.
Uses functions process_cmd_token and process_other_token to create the token.
Works with handle_quote_content.
*/
void maketoken(char *input, t_vars *vars)
{
	if (!input || !vars)
		return ;
	if (vars->curr_type == TYPE_CMD)
		process_cmd_token(input, vars);
	else
		process_other_token(input, vars);
}

/*
Decides whether to expand the character based on the quote depth.
Works with process_char.
*/
int	handle_expand(t_vars *vars)
{
	if (vars->quote_depth == 0)
		return (1);
	if (vars->quote_ctx[vars->quote_depth - 1].type == '"')
		return (1);
	return (0);
}

/*
Function to selectively decide how to process the character.
Works with tokenize.
*/
void	process_char(char *input, int *i, t_vars *vars)
{
	char	*content;

	if (!input || !i || !vars)
		return ;
	if (*i == 0 || ft_isspace(input[*i - 1]))
		vars->start = *i;
	if (input[*i] == '\'' || input[*i] == '\"')
	{
		handle_quotes(input, i, vars);
		if (vars->quote_depth > 0)
		{
			content = read_quoted_content(input, i,
					vars->quote_ctx[vars->quote_depth - 1].type);
			if (content)
			{
				maketoken(content, vars);
				ft_safefree((void **)&content);
			}
		}
		return ;
	}
	(*i)++;
}

/*
Control function. Tokenizes the input string.
*/
void	tokenize(char *input, t_vars *vars)
{
	int	i;

	i = 0;
	vars->start = 0;
	while (input[i])
		process_char(input, &i, vars);
}
