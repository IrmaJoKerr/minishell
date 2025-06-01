/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_envops.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 16:32:37 by bleow             #+#    #+#             */
/*   Updated: 2025/06/01 20:05:16 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

char	*get_env_key(const char *arg_str, int *key_len)
{
	char	*equal_ptr;

	equal_ptr = ft_strchr(arg_str, '=');
	if (equal_ptr)
	{
		*key_len = equal_ptr - arg_str;
		return (ft_substr(arg_str, 0, *key_len));
	}
	else
	{
		*key_len = ft_strlen(arg_str);
		return (ft_strdup(arg_str));
	}
}

void	free_envop_list(t_envop *head)
{
	t_envop	*tmp;

	while (head)
	{
		tmp = head->next;
		free(head->arg_str);
		free(head->export_key);
		free(head);
		head = tmp;
	}
}

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

void	err_invalid_export_arg(char *arg)
{
	ft_putstr_fd("export: '", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

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

int	chk_copy_or_write(int idx, t_envop *envop_list, t_envop **overwrite_node)
{
	t_envop	*curr;

	curr = envop_list;
	while (curr)
	{
		if (curr->matched_idx == idx)
		{
			if (curr->flag == -1)
				return (-1);
			if (curr->flag == 0)
			{
				if (overwrite_node)
					*overwrite_node = curr;
				return (0);
			}
		}
		curr = curr->next;
	}
	return (1);
}

int	copy_env_with_envop_list(char **env, t_envop *envop_list, char **new_env)
{
	int		i;
	int		j;
	int		action;
	t_envop	*overwrite_node;

	i = 0;
	j = 0;
	while (env && env[i])
	{
		overwrite_node = NULL;
		action = chk_copy_or_write(i, envop_list, &overwrite_node);
		if (action == 0)
			new_env[j++] = ft_strdup(overwrite_node->arg_str);
		else if (action == 1)
			new_env[j++] = ft_strdup(env[i]);
		i++;
	}
	return (j);
}

// char	**proc_envop_list(t_envop *envop_list, char **env)
// {
// 	t_envop	*node;
// 	t_envop	*n;
// 	int		old_len;
// 	int		delta;
// 	int		new_len;
// 	char	**new_env;
// 	int		i;
// 	int		j;
// 	int		skip;

// 	delta = 0;
	// old_len = ft_arrlen(env);
	// node = envop_list;
	// while (node)
	// {
	// 	if (node->flag == -1 && node->matched_idx != -1)
	// 		delta--;
	// 	else if (node->flag == 1 && node->matched_idx == -1)
	// 		delta++;
	// 	node = node->next;
	// }
// 	new_len = old_len + delta;
// 	new_env = malloc(sizeof(char *) * (new_len + 1));
// 	if (!new_env)
// 		return (NULL);
// 	i = 0;
// 	j = 0;
// 	while (env && env[i])
// 	{
// 		skip = 0;
// 		n = envop_list;
// 		while (n)
// 		{
// 			if (n->matched_idx == i)
// 			{
// 				if (n->flag == -1)
// 				{
// 					skip = 1;
// 					break ;
// 				}
// 				if (n->flag == 0)
// 				{
// 					new_env[j++] = ft_strdup(n->arg_str);
// 					skip = 1;
// 					break ;
// 				}
// 			}
// 			n = n->next;
// 		}
// 		if (!skip)
// 			new_env[j++] = ft_strdup(env[i]);
// 		i++;
// 	}
// 	node = envop_list;
// 	while (node)
// 	{
// 		if (node->flag == 1 && node->matched_idx == -1)
// 			new_env[j++] = ft_strdup(node->arg_str);
// 		node = node->next;
// 	}
// 	new_env[j] = NULL;
// 	return (new_env);
// }

int	decide_action_at_index(int idx, t_envop *env_list, t_envop **overwrite_node)
{
	t_envop	*curr;

	curr = env_list;
	while (curr)
	{
		if (curr->matched_idx == idx)
		{
			if (curr->flag == -1)
				return (-1);
			if (curr->flag == 0)
			{
				if (overwrite_node)
					*overwrite_node = curr;
				return (0);
			}
		}
		curr = curr->next;
	}
	return (1);
}

int	copy_env_with_ops(char **env, t_envop *env_list, char **new_env)
{
	int		i;
	int		j;
	int		action;
	t_envop	*overwrite_node;

	i = 0;
	j = 0;
	while (env && env[i])
	{
		overwrite_node = NULL;
		action = decide_action_at_index(i, env_list, &overwrite_node);
		if (action == 0)
			new_env[j++] = ft_strdup(overwrite_node->arg_str);
		else
			new_env[j++] = ft_strdup(env[i]);
		i++;
	}
	return (j);
}

// int	copy_env_with_ops(char **env, t_envop *env_list, char **new_env)
// {
// 	int		i;
// 	int		j;
// 	int		skip;
// 	t_envop	*n;

// 	i = 0;
// 	j = 0;
// 	while (env && env[i])
// 	{
// 		skip = 0;
// 		n = env_list;
// 		while (n)
// 		{
// 			if (n->matched_idx == i)
// 			{
// 				if (n->flag == -1)
// 				{
// 					skip = 1;
// 					break ;
// 				}
// 				if (n->flag == 0)
// 				{
// 					new_env[j++] = ft_strdup(n->arg_str);
// 					skip = 1;
// 					break ;
// 				}
// 			}
// 			n = n->next;
// 		}
// 		if (!skip)
// 			new_env[j++] = ft_strdup(env[i]);
// 		i++;
// 	}
// 	return (j);
// }

int	calc_new_env_len(t_envop *envop_list, int old_len)
{
	t_envop	*node;
	int		env_arr_len;

	node = envop_list;
	env_arr_len = old_len;
	while (node)
	{
		if (node->flag == -1 && node->matched_idx != -1)
			env_arr_len--;
		else if (node->flag == 1 && node->matched_idx == -1)
			env_arr_len++;
		node = node->next;
	}
	return (env_arr_len);
}

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
			new_env[env_arr_idx++] = ft_strdup(node->arg_str);
		node = node->next;
	}
	new_env[env_arr_idx] = NULL;
	return (new_env);
}
