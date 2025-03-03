/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 04:36:48 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 13:06:43 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Makes a new node for a command. Works with handle_cmd_token.
*/
t_node	*make_cmdnode(char *token)
{
	t_node	*node;

	node = initnode(TYPE_CMD, token);
	if (!node)
		return (NULL);
	node->args = malloc(sizeof(char *) * 2);
	if (!node->args)
	{
		free(node);
		return (NULL);
	}
	node->args[0] = ft_strdup(token);
	node->args[1] = NULL;
	return (node);
}

/*
Makes a new node for a command token.
*/
t_node	*handle_cmd_token(char *token)
{
	t_node	*node;

	node = make_cmdnode(token);
	if (!node)
	{
		free(token);
		return (NULL);
	}
	return (node);
}

/*
Makes a new node for other types of tokens.
*/
t_node	*handle_other_token(char *token, t_tokentype type)
{
	t_node	*node;

	node = initnode(type, token);
	if (!node)
	{
		free(token);
		return (NULL);
	}
	return (node);
}

/*
Process quoted tokens during lexical analysis.
Updates position pointer to after closing quote.
Creates token from the quoted content.
Example: "hello world" -> processes the token and moves position
*/
void	handle_quote_token(char *str, t_vars *vars, int *pos)
{
	char	quote_char;
	int		start;

	if (!str || !vars || !pos)
		return ;
	quote_char = str[*pos];
	start = *pos;
	(*pos)++;
	while (str[*pos] && str[*pos] != quote_char)
		(*pos)++;
	if (str[*pos] == quote_char)
	{
		(*pos)++;
		vars->start = start;
		maketoken(str, vars);
		vars->start = *pos;
	}
	else
	{
		vars->quote_depth++;
	}
}

/*
Check if token type is a redirection.
Returns 1 if it's a redirection type, 0 otherwise.
Types: TYPE_HEREDOC, TYPE_IN_REDIRECT, TYPE_OUT_REDIRECT, TYPE_APPEND_REDIRECT
*/
int	is_redirection(t_tokentype type)
{
	return (type == TYPE_HEREDOC
		|| type == TYPE_IN_REDIRECT
		|| type == TYPE_OUT_REDIRECT
		|| type == TYPE_APPEND_REDIRECT);
}
