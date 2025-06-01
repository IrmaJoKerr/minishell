/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_envops_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 20:32:31 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 00:37:53 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Extract the key from an environment argument string.
- Sets key_len to the length of the key.
Returns a newly allocated key string.
*/
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

/*
Free a linked list of t_envop nodes.
- Frees all associated memory.
*/
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

/*
Print an error message for an invalid export argument.
- Outputs to standard error.
*/
void	err_invalid_export_arg(char *arg)
{
	ft_putstr_fd("export: '", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

/*
Copy environment variables into a new array, applying envop list changes.
- Overwrites or copies as needed.
Returns the number of entries copied.
*/
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

/*
Copy environment variables into a new array, applying export operations.
- Overwrites or copies as needed.
Returns the number of entries copied.
*/
// int	copy_env_with_ops(char **env, t_envop *env_list, char **new_env)
// {
// 	int		i;
// 	int		j;
// 	int		action;
// 	t_envop	*overwrite_node;

// 	i = 0;
// 	j = 0;
// 	while (env && env[i])
// 	{
// 		overwrite_node = NULL;
// 		action = decide_action_at_index(i, env_list, &overwrite_node);
// 		if (action == 0)
// 			new_env[j++] = ft_strdup(overwrite_node->arg_str);
// 		else
// 			new_env[j++] = ft_strdup(env[i]);
// 		i++;
// 	}
// 	return (j);
// }
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
		{
            new_env[j] = ft_strdup(overwrite_node->arg_str);
            // fprintf(stderr, "[DEBUG] copy_env_with_ops: overwrite [%d] with '%s'\n", j, overwrite_node->arg_str);
            j++;
        } 
		else if (action == 1)
		{
            new_env[j] = ft_strdup(env[i]);
            j++;
        }
		// else
		// {
        //     fprintf(stderr, "[DEBUG] copy_env_with_ops: skip [%d] '%s'\n", i, env[i]);
        // }
        i++;
    }
    // fprintf(stderr, "[DEBUG] copy_env_with_ops: total copied=%d\n", j);
    return (j);
}
