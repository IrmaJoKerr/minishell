/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 19:03:11 by lechan            #+#    #+#             */
/*   Updated: 2025/06/01 18:18:48 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Checks if the argument is a valid environment variable.
- Checks if the first character is a letter or underscore.
- Checks if the rest of the characters are alphanumeric or underscore.
- Checks if the argument contains an equals sign.
Returns the argument if valid, NULL if invalid.
*/
char	*valid_export(char *args)
{
	int		i;
	int		len;
	char	*equal_sign;

	i = 0;
	if (!ft_isalpha(args[0]) && args[0] != '_')
		return (NULL);
	equal_sign = ft_strchr(args, '=');
	if (equal_sign)
		len = equal_sign - args;
	else
		len = ft_strlen(args);
	while (i < len)
	{
		if (!(ft_isalnum(args[i]) || args[i] == '_'))
			return (NULL);
		i++;
	}
	return (args);
}

/*
Sorts environment variables in ascending order.
- Uses bubble sort algorithm.
Returns the sorted array.
*/
char	**asc_order(char **sort_env, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(sort_env[i], sort_env[j]) > 0)
			{
				temp = sort_env[i];
				sort_env[i] = sort_env[j];
				sort_env[j] = temp;
			}
			j++;
		}
		i++;
	}
	return (sort_env);
}

/*
Creates a sorted copy of environment variables.
- Allocates memory for temporary sorted environment variables array.
- Copies environment variables to the sorted array.
- Sorts the array in ascending order.
Returns the sorted array on success, NULL on failure.
*/
char	**make_sorted_env(int count, t_vars *vars)
{
	int		i;
	char	**sort_env;

	sort_env = (char **)malloc((count + 1) * sizeof(char *));
	if (!sort_env)
		return (NULL);
	i = 0;
	while (i < count)
	{
		sort_env[i] = ft_strdup(vars->env[i]);
		if (!sort_env[i])
		{
			cleanup_env_error(sort_env, i);
			return (NULL);
		}
		i++;
	}
	sort_env[i] = NULL;
	return (asc_order(sort_env, count));
}
