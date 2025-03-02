/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:23:30 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Gets PATH from environment variables.
*/
char	**get_path_env(char **envp)
{
	int	i;

	i = 0;
	while (!ft_strnstr(envp[i], "PATH", 4) && envp[i])
		i++;
	if (!envp[i])
		return (NULL);
	return (ft_split(envp[i] + 5, ':'));
}

/*
Tries to find command in a specific path.
*/
char	*try_path(char *path, char *cmd)
{
	char	*part_path;
	char	*full_path;

	part_path = ft_strjoin(path, "/");
	full_path = ft_strjoin(part_path, cmd);
	free(part_path);
	if (access(full_path, F_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}

/*
Searches for the command in the PATH environment variable.
Returns the full path.
*/
char	*get_cmd_path(char *cmd, char **envp)
{
	char	**paths;
	char	*path;
	int		i;

	paths = get_path_env(envp);
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		path = try_path(paths[i], cmd);
		if (path)
		{
			ft_free_2d(paths, ft_arrlen(paths));
			return (path);
		}
		i++;
	}
	ft_free_2d(paths, ft_arrlen(paths));
	return (NULL);
}

/*
Duplicates the environment variables.
Returns a pointer to the new environment.
*/
char	**dup_env(char **envp)
{
	char	**env;
	size_t	env_size;

	env_size = ft_arrlen(envp);
	env = malloc(sizeof(char *) * (env_size + 1));
	if (!env)
		return (NULL);
	while (env_size--)
	{
		env[env_size] = ft_strdup(envp[env_size]);
		if (!env[env_size])
		{
			ft_free_2d(env, env_size);
			return (NULL);
		}
	}
	env[ft_arrlen(envp)] = NULL;
	return (env);
}
