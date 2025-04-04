/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   typeconvert.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 16:51:38 by bleow             #+#    #+#             */
/*   Updated: 2025/04/03 12:56:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <string.h>

/*
Returns string representation of basic token types.
Handles the first part of token types.
*/
char	*get_token_str_basic(t_tokentype type)
{
	if (type == TYPE_ARGS)
		return (TOKEN_TYPE_ARGS);
	else if (type == TYPE_CMD)
		return (TOKEN_TYPE_CMD);
	else if (type == TYPE_DOUBLE_QUOTE)
		return (TOKEN_TYPE_DOUBLE_QUOTE);
	else if (type == TYPE_SINGLE_QUOTE)
		return (TOKEN_TYPE_SINGLE_QUOTE);
	else if (type == TYPE_HEREDOC)
		return (TOKEN_TYPE_HEREDOC);
	else if (type == TYPE_PIPE)
		return (TOKEN_TYPE_PIPE);
	else if (type == TYPE_EXPANSION)
		return (TOKEN_TYPE_EXPANSION);
	return (TOKEN_TYPE_NULL);
}

/*
Returns string representation of advanced token types.
Example: TYPE_PIPE -> "|" (String representation)
Handles the second part of token types.
Main control function for getting token strings.
*/
char	*get_token_str(t_tokentype type)
{
	char	*basic_token;

	basic_token = NULL;
	basic_token = get_token_str_basic(type);
	if (basic_token != NULL)
		return (basic_token);
	if (type == TYPE_IN_REDIRECT)
		return (TOKEN_TYPE_IN_REDIRECT);
	else if (type == TYPE_OUT_REDIRECT)
		return (TOKEN_TYPE_OUT_REDIRECT);
	else if (type == TYPE_APPEND_REDIRECT)
		return (TOKEN_TYPE_APPEND_REDIRECT);
	else if (type == TYPE_EXIT_STATUS)
		return (TOKEN_TYPE_EXIT_STATUS);
	else
		return (TOKEN_TYPE_ARGS);
	return (TOKEN_TYPE_ARGS);
}
