/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:23:30 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 12:40:13 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Extracts PATH directories from environment variables.
- Searches for the variable starting with "PATH=".
- Splits the PATH value by colon delimiter.
- Handles NULL environment array or missing PATH.
Returns:
- Array of path strings.
- NULL if PATH not found.
Works with search_in_env() for command path building.

Example: When PATH=/usr/bin:/bin:/usr/local/bin
- Returns array with ["/usr/bin", "/bin", "/usr/local/bin", NULL]
- Returns NULL if PATH is not in environment
*/
char	**get_path_env(char **envp)
{
	int	i;

	i = 0;
	if (!envp)
		return (NULL);
	while (envp[i] && !ft_strnstr(envp[i], "PATH=", 5))
		i++;
	if (!envp[i])
		return (NULL);
	return (ft_split(envp[i] + 5, ':'));
}

/*
Searches for a command in PATH environment directories.
- Gets list of directories from PATH environment variable.
- Tries each directory to find the command.
- Reports detailed search progress for ddebugging.
Returns:
-Full path to command if found, NULL otherwise.
Works with get_cmd_path() for command resolution.

Example: For command "grep" with PATH=/usr/bin:/bin
- Searches in /usr/bin and /bin
- Returns "/usr/bin/grep" if found there
- Returns NULL if not found in any directory
*/
char	*search_in_env(char *cmd, char **envp, t_vars *vars)
{
	char	**paths;
	char	*path;
	int		i;

	paths = get_path_env(envp);
	if (!paths)
	{
		ft_putendl_fd("No PATH found in environment", 2);
		return (NULL);
	}
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
	shell_error(cmd, ERR_CMD_NOT_FOUND, vars);
	ft_free_2d(paths, ft_arrlen(paths));
	return (NULL);
}

/*
Validates and resolves a direct path command.
- Checks if the path exists and is executable.
- Handles directories, permissions, and not found errors.
- Reports appropriate errors and updates error code.
Returns:
- Duplicated path string if valid and executable.
- NULL if path is invalid, a directory, or lacks permissions.
Works with get_cmd_path() for direct path handling.
*/
char	*handle_direct_path(char *cmd, t_vars *vars)
{
	struct stat	statbuf;

	if (stat(cmd, &statbuf) == 0)
	{
		if (S_ISDIR(statbuf.st_mode))
		{
			shell_error(cmd, ERR_ISDIRECTORY, vars);
			return (NULL);
		}
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		shell_error(cmd, ERR_PERMISSIONS, vars);
	}
	else
		shell_error(cmd, ERR_CMD_NOT_FOUND, vars);
	vars->error_code = 127;
	return (NULL);
}

/*
Resolves command path for execution with node validation.
- Validates the command node structure.
- Handles absolute and relative paths directly.
- Searches PATH environment for other commands.
- Verifies executable permissions.
- Updates error code in vars structure.
Returns:
- Full path to executable (caller must free).
- NULL if invalid node, command not found, or permission denied.
Works with exec_external_cmd() during command execution.

Example: For "ls" command
- Validates node has valid arguments
- Checks if it's a direct path (starts with / or ./)
- If not, searches in PATH environment directories
- Returns "/bin/ls" (or similar) if found
- Returns NULL with error message if not found/invalid
*/
char	*get_cmd_path(t_node *node, char **envp, t_vars *vars)
{
	char	*cmd;
	char	*path;
	char	*direct_path;

	if (!node || !node->args || !node->args[0])
	{
		vars->error_code = 1;
		return (NULL);
	}
	cmd = node->args[0];
	if (cmd[0] == '/' || (cmd[0] == '.' && (cmd[1] == '/'
				|| (cmd[1] == '.' && cmd[2] == '/'))))
	{
		direct_path = handle_direct_path(cmd, vars);
		return (direct_path);
	}
	path = search_in_env(cmd, envp, vars);
	if (!path)
		vars->error_code = 127;
	return (path);
}
