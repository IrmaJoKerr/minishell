/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:56 by lechan            #+#    #+#             */
/*   Updated: 2025/03/14 01:42:10 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Checks if the argument is a valid environment variable.
*/
char	*valid_export(char *args)
{
	int	i;

	i = 0;
	if (!ft_isalpha(args[0]) && args[0] != '_')
		return (NULL);
	while (args[i] && args[i] != '=')
	{
		if (!(ft_isalnum(args[i]) || args[i] == '_'))
			return (NULL);
		i++;
	}
	return (args);
}

/*
Ascending order for environment variables.
*/
char	**asc_order(char **sort_env, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	j = 0;
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
Returns NULL on allocation failure.
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

/*
Sorts the environment variables in ascending order.
*/
/*
int	sort_env(int count, t_vars *vars)
{
	int		i;
	char	**sort_env;

	i = 0;
	sort_env = (char **)malloc((count + 1) * sizeof(char *));
	if (!sort_env)
		return (1);
	while (i < count)
	{
		sort_env[i] = ft_strdup(vars->env[i]);
		if (!sort_env[i])
		{
			while (i > 0)
			{
				i--;
				ft_safefree((void **)&sort_env[i]);	
			}
			ft_safefree((void **)&sort_env);
			return (1);
		}
		i++;
	}
	sort_env[i] = '\0';
	sort_env = asc_order(sort_env, count);
	i = 0;
	while (sort_env[i])
	{
		printf("declare -x %s", sort_env[i]);
		if (sort_env[i][ft_strlen(sort_env[i]) - 1] == '=')
			printf("\"\"");
		ft_safefree((void **)&sort_env[i++]);
		printf("\n");
	}
	ft_safefree((void **)&sort_env);
	return (0);
}
*/

/*
Sorts and prints environment variables in export format.
Returns 0 on success, 1 on failure.
*/
int	sort_env(int count, t_vars *vars)
{
	int		i;
	char	**sort_env;
	char    *equal_pos;
	
	i = 0;
	sort_env = make_sorted_env(count, vars);
	if (!sort_env)
		return (1);
	while (sort_env[i])
	{
		equal_pos = ft_strchr(sort_env[i], '=');
		if (equal_pos)
		{
			*equal_pos = '\0';
			// Use ft_putstr_fd instead of printf to respect redirection
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(sort_env[i], STDOUT_FILENO);
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(equal_pos + 1, STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
			*equal_pos = '=';
		}
		else
		{
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(sort_env[i], STDOUT_FILENO);
		}
		ft_safefree((void **)&sort_env[i++]);
		ft_putstr_fd("\n", STDOUT_FILENO);
	}
	ft_safefree((void **)&sort_env);
	return (0);
}

/*
Built-in command: export. Sets or displays environment variables.
*/
int	builtin_export(char **args, t_vars *vars)
{
	int i;
	int	count;

	if (!vars || !vars->env)
		return (1);
	printf("this is export\n");
	count = 0;
	while (vars->env[count])
		count++;
	if (!args[1])
		return (sort_env(count, vars), 0);
	else
	{
		i = 1;
		while (args[i])
		{
			if (valid_export(args[i]))
				modify_env(&vars->env, 1, args[i]);
			else
				printf("export: '%s': not a valid identifier\n", args[i]);
			i++;
		}
	}
	return (0);
}
