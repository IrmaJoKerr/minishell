/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:04:06 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 01:51:02 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Tracks opening and closing of single and double quotes.
1) Identifies single (') and double (") quotes in the input
2) Tracks a stack of quote contexts in vars->quote_ctx
3) Increments quote_depth when opening a new quote
4) Decrements quote_depth when matching closing quote is found
5) Advances the position pointer past the quote character
This tracking handles nested quotes and detection of unclosed quotes.
Works with tokenize().
*/
void	handle_quotes(char *input, int *pos, t_vars *vars)
{
	char	quote;

	if (!input || !pos || !vars)
		return ;
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
Handles unclosed quotes. Prompts the user to close the quotes with new input.
1) Checks which type of quote is unclosed (single or double)
2) Displays matching prompt (SQUOTE> or DQUOTE>)
3) Reads new input from user with readline()
4) Joins the new input with the existing input string using ft_strjoin()
5) Retokenizes the combined input to check if quotes are now closed
6) Repeats until all quotes are closed (quote_depth == 0)
Returns the completed input string with balanced quotes, or NULL on error.
Works with tokenize() and lexerlist().
*/
char	*handle_unclosed_quotes(char *input, t_vars *vars)
{
	char	*line;
	char	*temp;
	char	*prompt;
	char	*result;

	prompt = "DQUOTE> ";
	if (vars->quote_depth > 0
		&& vars->quote_ctx[vars->quote_depth - 1].type == '\'')
		prompt = "SQUOTE> ";
	while (vars->quote_depth > 0)
	{
		line = readline(prompt);
		if (!line)
			return (NULL);
		temp = input;
		result = ft_strjoin(temp, "\n");
		ft_safefree((void **)&temp);
		temp = result;
		result = ft_strjoin(temp, line);
		ft_safefree((void **)&temp);
		ft_safefree((void **)&line);
		input = result;
		tokenize(input, vars);
	}
	return (input);
}

/*
Extracts the string between matching quote characters.
1) Starts from the position +1 after the opening quote
2) Advances the position pointer until closing quote or end of string
3) Creates a substring with just the quoted content (without quotes)
4) Advances position +1 past the closing quote
5) Returns a new string with the quoted content, or NULL if unclosed.
Note: Caller is responsible for freeing the returned string after use.
Works with handle_quote_token().
*/
char	*read_quoted_content(char *input, int *pos, char quote)
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

/*
Helper function to remove quotes from an argument
*/
void	process_quotes_in_arg(char **arg)
{
	char	*str;
	char	*new_str;
	size_t	len;
	
	str = *arg;
	if (!str)
		return ;
	len = ft_strlen(str);
	if (len < 2)
		return ;
	if (str[0] == '"' && str[len-1] == '"')
	{
		new_str = ft_substr(str, 1, len-2);
		if (new_str)
		{
			ft_safefree((void **)&str);
			*arg = new_str;
		}
		return ;
	}
	if (str[0] == '\'' && str[len-1] == '\'')
	{
		new_str = ft_substr(str, 1, len-2);
		if (new_str)
		{
			ft_safefree((void **)&str);
			*arg = new_str;
		}
	}
}
