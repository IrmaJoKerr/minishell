/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   typeconvert.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 16:51:38 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 00:27:44 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Returns string representation of token type.
Central function for getting token strings.
*/
const char *get_token_str(t_tokentype type)
{
    if (type == TYPE_CMD)
        return (TOKEN_TYPE_CMD);
    else if (type == TYPE_ARGS)
        return (TOKEN_TYPE_ARGS);
    else if (type == TYPE_STRING)
        return (TOKEN_TYPE_STRING);
    else if (type == TYPE_DOUBLE_QUOTE)
        return (TOKEN_TYPE_DOUBLE_QUOTE);
    else if (type == TYPE_SINGLE_QUOTE)
        return (TOKEN_TYPE_SINGLE_QUOTE);
    else if (type == TYPE_HEREDOC)
        return (TOKEN_TYPE_HEREDOC);
    else if (type == TYPE_IN_REDIRECT)
        return (TOKEN_TYPE_IN_REDIRECT);
    else if (type == TYPE_OUT_REDIRECT)
        return (TOKEN_TYPE_OUT_REDIRECT);
    else if (type == TYPE_APPEND_REDIRECT)
        return (TOKEN_TYPE_APPEND_REDIRECT);
    else if (type == TYPE_EXPANSION)
        return (TOKEN_TYPE_EXPANSION);
    else if (type == TYPE_PIPE)
        return (TOKEN_TYPE_PIPE);
    else if (type == TYPE_EXIT_STATUS)
        return (TOKEN_TYPE_EXIT_STATUS);
    else
        return (TOKEN_TYPE_UNKNOWN);
}

/*
Converts string to token type.
Central function for classifying tokens.
*/
t_tokentype get_token_type(const char *str)
{
    if (!str || !*str)
        return (TYPE_STRING);
        
    if (ft_strcmp(str, TOKEN_TYPE_CMD) == 0)
        return (TYPE_CMD);
    else if (ft_strcmp(str, TOKEN_TYPE_ARGS) == 0)
        return (TYPE_ARGS);
    else if (ft_strcmp(str, TOKEN_TYPE_STRING) == 0)
        return (TYPE_STRING);
    else if (ft_strcmp(str, TOKEN_TYPE_DOUBLE_QUOTE) == 0)
        return (TYPE_DOUBLE_QUOTE);
    else if (ft_strcmp(str, TOKEN_TYPE_SINGLE_QUOTE) == 0)
        return (TYPE_SINGLE_QUOTE);
    else if (ft_strcmp(str, TOKEN_TYPE_HEREDOC) == 0)
        return (TYPE_HEREDOC);
    else if (ft_strcmp(str, TOKEN_TYPE_IN_REDIRECT) == 0)
        return (TYPE_IN_REDIRECT);
    else if (ft_strcmp(str, TOKEN_TYPE_OUT_REDIRECT) == 0)
        return (TYPE_OUT_REDIRECT);
    else if (ft_strcmp(str, TOKEN_TYPE_APPEND_REDIRECT) == 0)
        return (TYPE_APPEND_REDIRECT);
    else if (ft_strcmp(str, TOKEN_TYPE_EXPANSION) == 0)
        return (TYPE_EXPANSION);
    else if (ft_strcmp(str, TOKEN_TYPE_PIPE) == 0)
        return (TYPE_PIPE);
    else if (ft_strcmp(str, TOKEN_TYPE_EXIT_STATUS) == 0)
        return (TYPE_EXIT_STATUS);
    else
        return (TYPE_UNKNOWN);
}
