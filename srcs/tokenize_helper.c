/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 07:54:40 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 19:35:04 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Process command node arguments from split input
*/
t_node	*build_cmdarg_node(char **args)
{
	t_node	*node;
	int		i;

	if (!args || !args[0])
		return (NULL);
	node = new_cmd_node(args[0]);
	if (!node)
		return (NULL);
	i = 1;
	while (args[i])
	{
		append_arg(node, args[i], 0);
		i++;
	}
	return (node);
}

/*
Update the node pointers in the token chain
*/
void	build_token_linklist(t_vars *vars, t_node *node)
{
	if (!vars->head)
	{
		vars->head = node;
		vars->current = node;
	}
	else
	{
		vars->current->next = node;
		node->prev = vars->current;
		vars->current = node;
	}
}

/*
Process command tokens by splitting input string
with whitespace as delimiter. Uses ft_splitstr()
to handle quotes during splitting.
*/
void	process_cmd_token(char *input, t_vars *vars)
{
	char	*combined;
	char	**args;
	t_node	*node;
	int		i;

	args = ft_splitstr(input, " \t\n\v\f\r");
	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		process_quotes_in_arg(&args[i]);
		if (i > 0 && args[i][0] == '-' && ft_strlen(args[i]) == 1
			&& args[i + 1] && ft_isalpha(args[i + 1][0]))
		{
			combined = ft_strjoin(args[i], args[i + 1]);
			if (combined)
			{
				free(args[i]);
				free(args[i + 1]);
				args[i] = combined;
				int j = i + 1;
				while (args[j + 1])
				{
					args[j] = args[j+1];
					j++;
				}
				args[j] = NULL;
			}
		}
		i++;
	}
	if (args[0])
	{
		node = build_cmdarg_node(args);
		if (node)
			build_token_linklist(vars, node);
	}
	ft_free_2d(args, ft_arrlen(args));
}

/*
Process non-command tokens
*/
void	process_other_token(char *input, t_vars *vars)
{
	char	*token;
	t_node	*node;

	if (vars->pos <= vars->start)
		return ;
	token = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!token)
		return ;
	if (vars->curr_type == TYPE_ARGS)
		node = new_cmd_node(token);
	else
		node = new_other_node(token, vars->curr_type);
	ft_safefree((void **)&token);
	if (!node)
		return ;
	if (vars->curr_type != TYPE_CMD && vars->current)
		add_child(vars->current, node);
	else
		vars->current = node;
}
