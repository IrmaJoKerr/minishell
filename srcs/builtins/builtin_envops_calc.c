/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_envops_calc.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 20:30:57 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 00:31:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Decide what action to take for an environment variable at a given index.
- Checks if the variable should be overwritten, deleted, or left unchanged.
Returns:
-1 for delete.
0 for overwrite.
1 for copy addon at the end.
*/
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

/*
Calculate the new length of the environment array after applying envop list.
- Accounts for additions and deletions.
Returns the new environment array length.
*/
// int	calc_new_env_len(t_envop *envop_list, int old_len)
// {
// 	t_envop	*node;
// 	int		env_arr_len;

// 	node = envop_list;
// 	env_arr_len = old_len;
// 	while (node)
// 	{
// 		if (node->flag == -1 && node->matched_idx != -1)
// 			env_arr_len--;
// 		else if (node->flag == 1 && node->matched_idx == -1)
// 			env_arr_len++;
// 		node = node->next;
// 	}
// 	return (env_arr_len);
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
    // fprintf(stderr, "[DEBUG] calc_new_env_len: old_len=%d, new_len=%d\n", old_len, env_arr_len);
    return (env_arr_len);
}

/*
Check if an environment variable should be copied or overwritten.
- Used for building the new environment array.
Returns -1 for delete, 0 for overwrite, 1 for copy as is.
*/
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
