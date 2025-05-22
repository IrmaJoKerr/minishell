/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_permissions.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 08:28:42 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 17:04:50 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
Checks read permission for a file.
- Verifies file exists and is readable.
- Reports appropriate error if not found or not readable.
Returns:
- 1 if file can be read.
- 0 if file doesn't exist or lacks permission.
Works with chk_permissions for read mode checks.
*/
int	chk_read_permission(char *filename, t_vars *vars)
{
	if (access(filename, F_OK | R_OK) == -1)
	{
		if (access(filename, F_OK) == -1)
			not_found_error(filename, vars);
		else
			shell_error(filename, ERR_PERMISSIONS, vars);
		return (0);
	}
	return (1);
}

/*
Checks write permission for an existing file.
- Verifies file is writable.
- Reports permission error if not writable.
Returns:
- 1 if file can be written.
- 0 if file lacks write permission.
Works with chk_permissions for write mode on existing files.
*/
int	chk_file_write_permission(char *filename, t_vars *vars)
{
	if (access(filename, W_OK) == -1)
	{
		shell_error(filename, ERR_PERMISSIONS, vars);
		return (0);
	}
	return (1);
}

/*
Checks if parent directory is writable for a new file.
- Extracts directory path from filename.
- Verifies directory has write permission.
- Reports error if directory can't be written.
Returns:
- 1 if directory is writable.
- 0 if directory lacks write permission.
Works with chk_permissions for write mode on new files.
*/
int	chk_dir_write_permission(char *filename, t_vars *vars)
{
	char	*dir_path;
	int		result;

	dir_path = extract_dir_path(filename);
	if (!dir_path)
		return (0);
	result = 1;
	if (access(dir_path, W_OK) == -1)
	{
		shell_error(filename, ERR_PERMISSIONS, vars);
		result = 0;
	}
	free(dir_path);
	return (result);
}

/*
Checks file permissions before redirection starts.
- For read mode: Verifies file exists and is readable.
- For write mode: Checks if file exists and is writable.
- For new files: Checks if parent directory is writable.
Returns:
- 1 if file can be accessed with requested mode.
- 0 if not.
Works with redirection setup to validate file operations.
*/
int	chk_permissions(char *filename, int mode, t_vars *vars)
{
	int	result;

	if (mode == O_RDONLY)
	{
		result = chk_read_permission(filename, vars);
		return (result);
	}
	else if (mode & O_WRONLY)
	{
		if (access(filename, F_OK) == 0)
		{
			result = chk_file_write_permission(filename, vars);
			return (result);
		}
		else
		{
			result = chk_dir_write_permission(filename, vars);
			return (result);
		}
	}
	else if (mode & O_RDWR)
		return (1);
	return (0);
}
