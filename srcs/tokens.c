/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 04:36:48 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 18:05:14 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Makes a new node for a command. Works with new_cmd_node.
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
		ft_safefree((void **)&node);
		return (NULL);
	}
	node->args[0] = ft_strdup(token);
	if (!node->args[0])
    {
        ft_safefree((void **)&node->args);
        ft_safefree((void **)&node);
        return (NULL);
    }
	node->args[1] = NULL;
	return (node);
}

/*
Makes a new node for a command token.
*/
t_node	*new_cmd_node(char *token)
{
	t_node	*node;

	node = make_cmdnode(token);
	if (!node)
	{
		ft_safefree((void **)&token);
		return (NULL);
	}
	return (node);
}

/*
Makes a new node for other types of tokens.
*/
t_node	*new_other_node(char *token, t_tokentype type)
{
	t_node	*node;

	node = initnode(type, token);
	if (!node)
	{
		ft_safefree((void **)&token);
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
/*
Process quoted tokens during lexical analysis.
Updates position pointer to after closing quote.
Creates token from the quoted content.
Example: "hello world" -> processes the token and moves position
*/
/*
void	handle_quote_token(char *str, t_vars *vars, int *pos)
{
    char		quote_char;
    int			start;
    t_tokentype	quote_type;

    if (!str || !vars || !pos)
        return ;
    
    quote_char = str[*pos];
    if (quote_char == '"')
        quote_type = TYPE_DOUBLE_QUOTE;
    else
        quote_type = TYPE_SINGLE_QUOTE;
    printf("DEBUG: Processing %s token\n", get_token_str(quote_type));
    
    start = *pos;
    (*pos)++;
    
    while (str[*pos])
    {
        if (str[*pos] == quote_char)
            break;
        (*pos)++;
    }
    
    if (str[*pos] == quote_char)
    {
        (*pos)++;
        vars->start = start;
        vars->curr_type = quote_type;
        maketoken(str, vars);
        vars->start = *pos;
    }
    else
    {
        vars->quote_depth++;
        vars->quote_ctx[vars->quote_depth - 1].type = quote_char;
    }
}
*/
void handle_quote_token(char *str, t_vars *vars, int *pos)
{
    char quote_char;
    int start;
    t_tokentype quote_type;

    if (!str || !vars || !pos)
        return;
    
    quote_char = str[*pos];
    if (quote_char == '"')
        quote_type = TYPE_DOUBLE_QUOTE;
    else
        quote_type = TYPE_SINGLE_QUOTE;
    
    printf("DEBUG: Processing %s token\n", get_token_str(quote_type));
    
    start = *pos;
    (*pos)++;
    
    while (str[*pos])
    {
        if (str[*pos] == quote_char)
            break;
        (*pos)++;
    }
    
    if (str[*pos] == quote_char)
    {
        (*pos)++;
        vars->start = start;
        vars->curr_type = quote_type;
        maketoken(str, vars);
        vars->start = *pos;
    }
    else
    {
        vars->quote_depth++;
        vars->quote_ctx[vars->quote_depth - 1].type = quote_char;
    }
}
