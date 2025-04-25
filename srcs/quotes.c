/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:04:06 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 23:15:13 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Records a new open quote in the quote context tracking array.
Updates in_quote to the current quote character.
Updates quote context with position and depth information.
Increments quote depth.
*/
void	track_quote_ctx(char quote_char, char *in_quote, int pos, t_vars *vars)
{
	*in_quote = quote_char;
	if (vars->quote_depth < 32)
	{
		vars->quote_ctx[vars->quote_depth].type = quote_char;
		vars->quote_ctx[vars->quote_depth].start_pos = pos;
		vars->quote_ctx[vars->quote_depth].depth = vars->quote_depth + 1;
		vars->quote_depth++;
	}
}

/*
Validates if all quotes in a command are properly closed.
Updates quote_ctx and quote_depth in vars struct.
Tracks single and double quotes.
Returns:
- 1 if all quotes are balanced.
- 0 if unclosed quotes are found.
*/
int	validate_quotes(char *input, t_vars *vars)
{
	int		i;
	char	in_quote;

	i = 0;
	in_quote = 0;
	vars->quote_depth = 0;
	while (input[i])
	{
		if (!in_quote && (input[i] == '\'' || input[i] == '"'))
		{
			track_quote_ctx(input[i], &in_quote, i, vars);
		}
		else if (in_quote && input[i] == in_quote)
		{
			in_quote = 0;
			vars->quote_depth--;
		}
		i++;
	}
	return (in_quote == 0);
}

/*
Prompts for additional input based on the quote type
Returns: 
- Newly allocated string with the input.
- NULL if readline fails.
*/
char	*quote_prompt(char quote_type)
{
	char	*prompt;
	char	*addon_input;

	if (quote_type == '\'')
		prompt = "SQUOTE> ";
	else
		prompt = "DQUOTE> ";
	addon_input = readline(prompt);
	if (!addon_input)
		return (NULL);
	if (*addon_input)
		add_history(addon_input);
	return (addon_input);
}
