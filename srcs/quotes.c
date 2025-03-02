/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:04:06 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:28 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Handles single and double quotes. Handles nested quotes.
*/
void	handle_quotes(char *input, int *pos, t_vars *vars)
{
	char	quote;
	
	if (!input || !pos || !vars)
		return;
	quote = input[*pos];
	if (quote == '\'' || quote == '\"')
	{
		if (vars->quote_depth == 0)
		{
			vars->quote_ctx[0].type = quote;
			vars->quote_ctx[0].start_pos = *pos;
			vars->quote_depth = 1;
			(*pos)++;
		}
		else if (quote == vars->quote_ctx[vars->quote_depth - 1].type)
		{
			vars->quote_depth--;
			(*pos)++;
		}
	}
}

/*
Handles unclosed quotes. Prompts the user to close the quotes.
*/
char *handle_unclosed_quotes(char *input, t_vars *vars)
{
	char	*line;
	char	*temp;
	char	*prompt;
	char	*result;

	prompt = "DQUOTE> ";
	if (vars->quote_depth > 0 && 
		vars->quote_ctx[vars->quote_depth - 1].type == '\'')
		prompt = "SQUOTE> ";
	while (vars->quote_depth > 0)
	{
		line = readline(prompt);
		if (!line)
			return (NULL);
		temp = input;
		result = ft_strjoin(temp, "\n");
		free(temp);
		temp = result;
		result = ft_strjoin(temp, line);
		free(temp);
		free(line);
		input = result;
		tokenize(input, vars);
	}
	return (input);
}

/*
Extracts the content of a quoted string.
*/
char *read_quoted_content(char *input, int *pos, char quote)
{
	int		start;
	char	*content;

	start = *pos + 1;
	*pos = start;
	while (input[*pos] && input[*pos] != quote)
		(*pos)++;
	if (!input[*pos])
		return (NULL);
	content = ft_substr(input, start, *pos - start);
	(*pos)++;
	return (content);
}
