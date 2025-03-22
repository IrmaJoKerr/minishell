/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arguments.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:36:41 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 18:53:18 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	create_args_array(t_node *node, char *token)
{
	char	**args;
	int		*quote_types;

	args = malloc(sizeof(char *) * 2);
	if (!args)
		return ;
	args[0] = ft_strdup(token);
	if (!args[0])
	{
		ft_safefree((void **)&args);
		return ;
	}
	args[1] = NULL;
	node->args = args;
	quote_types = malloc(sizeof(int) * 2);
	if (!quote_types)
		return ;
	quote_types[0] = 0;
	quote_types[1] = 0;
	node->arg_quote_type = quote_types;
}

void	append_arg(t_node *node, char *new_arg, int quote_type)
{
	char	**new_args;
	int		*quo_types;
	size_t	len;
	size_t	i;

	if (!node || !new_arg || !node->args)
		return ;
	len = ft_arrlen(node->args);
	new_args = malloc(sizeof(char *) * (len + 2));
	if (!new_args)
		return ;
	i = -1;
	while (++i < len)
	{
		new_args[i] = node->args[i];
		if (!new_args[i])
		{
			ft_safefree((void **)&new_args);
			return ;
		}
	}
	new_args[len] = ft_strdup(new_arg);
	new_args[len + 1] = NULL;
	process_quotes_in_arg(&new_args[len]);
	quo_types = malloc(sizeof(int) * (len + 2));
	if (node->arg_quote_type)
	{
		i = 0;
		while (i < len)
		{
			quo_types[i] = node->arg_quote_type[i];
			i++;
		}
		ft_safefree((void **)&node->arg_quote_type);
	}
	else
	{
		i = 0;
		while (i < len)
		{
			quo_types[i] = 0;
			i++;
		}
	}
	quo_types[len] = quote_type;
	quo_types[len + 1] = 0;
	node->arg_quote_type = quo_types;
	ft_safefree((void **)&node->args);
	node->args = new_args;
}
