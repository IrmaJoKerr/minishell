/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenclass.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:28:07 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 01:51:20 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles exit status token. Returns the exit status. Works with classify.
*/
char	*handle_exit_status(t_vars *vars)
{
	return (ft_itoa(vars->error_code));
}

/*
Handles redirection token classification and checking.
mode: 0 for type check, 1 for classification
Returns: token type or boolean for type check.
Works with classify.
*/
t_tokentype	redirection_type(char *str, int mode)
{
	t_tokentype	type;

	if (mode == 0)
	{
		type = classify(str);
		return (type == TYPE_IN_REDIRECT
			|| type == TYPE_OUT_REDIRECT
			|| type == TYPE_APPEND_REDIRECT
			|| type == TYPE_HEREDOC);
	}
	if (!str[1])
	{
		if (str[0] == '<')
			return (TYPE_IN_REDIRECT);
		return (TYPE_OUT_REDIRECT);
	}
	if (str[0] == '<' && str[1] == '<')
		return (TYPE_HEREDOC);
	if (str[0] == '>' && str[1] == '>')
		return (TYPE_APPEND_REDIRECT);
	if (str[0] == '<')
		return (TYPE_IN_REDIRECT);
	return (TYPE_OUT_REDIRECT);
}

/*
Classify token type. Main controller function for token classification.
*/
t_tokentype	classify(char *str)
{
	if (!str || !*str)
		return (TYPE_STRING);
	if (str[0] == '-' && str[1] && str[1] != '"' && str[1] != '\'')
		return (TYPE_ARGS);
	if (str[0] == '|')
		return (TYPE_PIPE);
	if (str[0] == '<' || str[0] == '>')
		return (redirection_type(str, 1));
	if (str[0] == '$')
	{
		if (str[1] == '?')
			return (TYPE_EXIT_STATUS);
		return (TYPE_EXPANSION);
	}
	if (str[0] == '"')
		return (TYPE_DOUBLE_QUOTE);
	if (str[0] == '\'')
		return (TYPE_SINGLE_QUOTE);
	return (TYPE_STRING);
}
