/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_envops.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 16:32:37 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 13:31:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Create a new t_envop node from an argument string and flag.
- Duplicates the argument string.
- Extracts the export key and its length.
Returns:
- Pointer to new node.
- NULL on failure.
*/
t_envop	*make_envop_node(const char *arg, int flag)
{
	t_envop	*node;

	node = malloc(sizeof(t_envop));
	if (!node)
		return (NULL);
	node->arg_str = ft_strdup(arg);
	node->export_key = NULL;
	node->flag = flag;
	node->arg_len = 0;
	node->matched_idx = -1;
	node->next = NULL;
	if (node->arg_str)
		node->export_key = get_env_key(node->arg_str, &node->arg_len);
	return (node);
}

/*
Parse argument list into a linked list of t_envop nodes.
- Skips invalid export arguments.
- Frees all on allocation failure.
Returns:
- Head of the list.
- NULL on error.
*/
t_envop	*parse_envop_list(char **args, int op_type)
{
	t_envop	*head;
	t_envop	*node;
	int		i;

	head = NULL;
	i = 1;
	while (args[i])
	{
		if (!valid_export(args[i]))
			err_invalid_export_arg(args[i]);
		else
		{
			node = make_envop_node(args[i], op_type);
			if (!node)
			{
				free_envop_list(head);
				return (NULL);
			}
			add_envop_node(&head, node);
		}
		i++;
	}
	return (head);
}

/*
Append a t_envop node to the end of a linked list.
- If head is NULL, sets head to node.
*/
void	add_envop_node(t_envop **head, t_envop *node)
{
	t_envop	*cur;

	if (!*head)
		*head = node;
	else
	{
		cur = *head;
		while (cur->next)
			cur = cur->next;
		cur->next = node;
	}
}

/*
Match each t_envop node to an environment variable.
- Sets matched_idx if found.
- Updates flag if assignment is present.
*/
void	match_envline_to_env(t_envop *envop_list, char **env)
{
	t_envop	*node;
	int		i;

	node = envop_list;
	while (node)
	{
		node->matched_idx = -1;
		i = 0;
		while (env && env[i])
		{
			if ((ft_memcmp(env[i], node->export_key, node->arg_len) == 0)
				&& (env[i][node->arg_len] == '='
				|| env[i][node->arg_len] == '\0'))
			{
				node->matched_idx = i;
				if (node->flag == 1 && ft_strchr(node->arg_str, '='))
					node->flag = 0;
				break ;
			}
			i++;
		}
		node = node->next;
	}
}

/*
Process envop list and environment to produce a new environment array.
- Applies export operations.
- Adds new variables if needed.
Returns:
- A new environment array.
- NULL on failure.
*/
char	**proc_envop_list(t_envop *envop_list, char **env)
{
	int		env_arr_len;
	int		env_arr_idx;
	char	**new_env;
	t_envop	*node;

	env_arr_len = calc_new_env_len(envop_list, ft_arrlen(env));
	new_env = malloc(sizeof(char *) * (env_arr_len + 1));
	if (!new_env)
		return (NULL);
	env_arr_idx = copy_env_with_ops(env, envop_list, new_env);
	node = envop_list;
	while (node)
	{
		if (node->flag == 1 && node->matched_idx == -1)
		{
			if (env_arr_idx < env_arr_len)
				new_env[env_arr_idx++] = ft_strdup(node->arg_str);
		}
		node = node->next;
	}
	new_env[env_arr_idx] = NULL;
	return (new_env);
}
