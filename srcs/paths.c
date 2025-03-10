/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:23:30 by bleow             #+#    #+#             */
/*   Updated: 2025/03/10 13:59:08 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Gets PATH from environment variables.
1) Searches through environment variables to find one starting with "PATH"
2) If found, splits the PATH string by ':' delimiter into an array of paths
3) Returns NULL if PATH is not found
Works with get_cmd_path() to find executable commands.
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
Tries to find a command in a specific directory path.
1) Constructs a full path by joining directory path + "/" + command name
2) Checks if the file exists and is accessible using access()
3) Returns the full path if found, or NULL otherwise
Note: Caller is responsible for freeing the returned path string after use.
Works with get_cmd_path().
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
Searches for the command in all PATH directories.
1) Gets the PATH environment variable
2) Searches through each directory in PATH trying to find the command
3) When done, it cleans up allocated memory for path list
4) Returns the first valid full path or NULL if not found
Note: Caller is responsible for freeing the returned path string after use.
Works with execute_cmd().
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
Creates a deep copy of the environment variables array. (PATH, HOME, etc.)
1) Allocates memory for a new 2d string array
2) Copies each environment string individually
3) Sets NULL terminator at the end of the array
4) Handles memory errors by freeing partial allocations
Returns a newly allocated environment array or NULL on failure.
Used for safely executing commands with modified environment variables
like "export" and "unset".
*/
char	**dup_env(char **envp)
{
	char	**env;
	size_t	env_size;
	size_t i;

    env_size = ft_arrlen(envp);
    env = malloc(sizeof(char *) * (env_size + 1));
    if (!env)
        return (NULL);
    i = 0;
    while (i < env_size)
    {
        env[i] = ft_strdup(envp[i]);
        if (!env[i])
        {
            ft_free_2d(env, i);
            return (NULL);
        }
        i++;
    }
    env[env_size] = NULL;
    return (env);
}
