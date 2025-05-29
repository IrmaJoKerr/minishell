/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   typeconvert.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 16:51:38 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 02:59:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Returns string representation of token types.
Example: TYPE_PIPE -> "|" (String representation)
*/
char	*get_token_str(t_tokentype type)
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
	else if (type == TYPE_IN_REDIR)
		return (TOKEN_TYPE_IN_REDIR);
	else if (type == TYPE_OUT_REDIR)
		return (TOKEN_TYPE_OUT_REDIR);
	else if (type == TYPE_APPD_REDIR)
		return (TOKEN_TYPE_APPD_REDIR);
	else if (type == TYPE_EXIT_STATUS)
		return (TOKEN_TYPE_EXIT_STATUS);
	else
		return (TOKEN_TYPE_ARGS);
}
