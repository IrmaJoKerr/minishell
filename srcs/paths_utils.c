/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:19:15 by bleow             #+#    #+#             */
/*   Updated: 2025/06/07 02:45:49 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Checks if a command exists in a specific directory.
- Joins directory path with command name.
- Uses access() to verify file existence.
- Frees intermediate memory allocations.
Returns:
- Full path to executable if found.
- NULL if not found.
Works with search_in_env() during command search.

Example: try_path("/usr/bin", "ls")
- Checks if "/usr/bin/ls" exists and is accessible
- Returns "/usr/bin/ls" if found
- Returns NULL if file doesn't exist or can't be accessed
*/
char	*try_path(char *path, char *cmd)
{
	char	*part_path;
	char	*full_path;

	part_path = ft_strjoin(path, "/");
	full_path = ft_strjoin(part_path, cmd);
	ft_safefree((void **)&part_path);
	if (access(full_path, F_OK) == 0)
		return (full_path);
	ft_safefree((void **)&full_path);
	return (NULL);
}

/*
Extracts directory path from a filename.
- Finds the last slash in the path.
- Returns directory containing the file.
- Handles special cases like root directory.
- Caller must free the returned string.
Returns:
- Allocated string with directory path.
- "." if no directory component is present.
Works with file permission checks and path operations.
*/
char	*extract_dir_path(char *filename)
{
	char	*dir_path;
	char	*last_slash;

	dir_path = ft_strdup(filename);
	if (!dir_path)
		return (NULL);
	last_slash = ft_strrchr(dir_path, '/');
	if (last_slash)
	{
		*last_slash = '\0';
		if (*dir_path == '\0')
			ft_strlcpy(dir_path, "/", 2);
	}
	else
		ft_strlcpy(dir_path, ".", 2);
	return (dir_path);
}

/*
Creates a deep copy of environment variables array.
- Allocates memory for the new array.
- Duplicates each environment string.
- Handles memory allocation failures.
Returns:
Newly allocated copy of environment or NULL on failure.
Works with init_shell() and builtin environment commands.

Example: When executing a command with custom environment
- Duplicates all environment variables safely
- Provides independent environment array for modification
- Ensures memory safety with proper cleanup on errors
*/
char	**dup_env(char **envp)
{
	char	**env;
	size_t	env_size;
	size_t	i;

	env_size = ft_arrlen(envp);
	env = (char **)malloc(sizeof(char *) * (env_size + 1));
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
