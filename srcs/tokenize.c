/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:49 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

/*
Creates a new token from the input string. Works with handle_quote_content.
*/
void	maketoken(char *input, t_vars *vars)
{
	char	*token;
	t_node	*node;

	if (!input || !vars || vars->pos <= vars->start)
		return;
	token = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!token)
		return;
	if (vars->curr_type == TYPE_CMD)
		node = handle_cmd_token(token);
	else
		node = handle_other_token(token, vars->curr_type);
	if (!node)
	{
		free(token);
		return;
	}
	if (vars->curr_type == TYPE_CMD)
		vars->current = node;
	else if (vars->current)
		add_child(vars->current, node);
	free(token);
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
void process_char(char *input, int *i, t_vars *vars)
{
	if (!input || !i || !vars)
		return;
	if (input[*i] == '\'' || input[*i] == '\"')
	{
		handle_quotes(input, i, vars);
		if (vars->quote_depth > 0)
		{
			char *content = read_quoted_content(input, i, 
				vars->quote_ctx[vars->quote_depth - 1].type);
			if (content)
			{
				maketoken(content, vars);
				free(content);
			}
		}
		return;
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
