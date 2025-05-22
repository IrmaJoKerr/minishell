/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:19:15 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 17:29:06 by bleow            ###   ########.fr       */
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
	free(part_path);
	if (access(full_path, F_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}
