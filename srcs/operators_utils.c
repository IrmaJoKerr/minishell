/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:18:19 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 23:54:36 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
