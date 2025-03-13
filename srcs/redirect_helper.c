/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 07:59:58 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 02:53:34 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check file permissions before redirection starts.
1) Read mode (O_RDONLY): Checks if file exists and is readable
2) Write mode (O_WRONLY): Checks if file exists and is writable,
   or if it doesn't exist, whether it can be created
Returns:
- 1 (true) if the file can be accessed with the current mode
- 1 (true) for write mode if file doesn't exist (creates new file)
- 0 (false) if permission is denied or other access error
*/
int	chk_permissions(char *filename, int mode)
{
	if (mode == O_RDONLY && access(filename, R_OK) == -1)
		return (0);
	if ((mode & O_WRONLY) && access(filename, W_OK) == -1)
	{
		if (access(filename, F_OK) == -1)
			return (1);
		return (0);
	}
	return (1);
}

/*
Sets file open flags for output redirection.
Can set flags for two output modes:
1) STDOUT redirection (>): Uses O_TRUNC to overwrite existing file
2) APPEND redirection (>>): Uses O_APPEND to add to existing file
Also sets for both modes:
- O_WRONLY: Write-only access
- O_CREAT: Create file if it doesn't exist
Parameters:
- 1 for APPEND mode (>>), 0 for STDOUT mode (>)
Returns: The combined flag value for open().
Used with handle_output_redirect().
*/
int	set_output_flags(int append)
{
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	return (flags);
}

/*
Sets file open flags for 3 redirection types:
1) INPUT redirection (<): O_RDONLY
2) OUTPUT redirection (>): O_WRONLY | O_CREAT | O_TRUNC
3) APPEND redirection (>>): O_WRONLY | O_CREAT | O_APPEND
Creates OUTPUT files with permission 0644 if they don't exist.
Parameters:
- mode: 0 for INPUT, 1 for OUTPUT, 2 for APPEND
Returns: The combined flag value for open().
Used by handle_redirect() to open files with correct modes.
*/
int	set_redirect_flags(int mode)
{
	int	flags;

	if (mode == 0)
		flags = O_RDONLY;
	else
	{
		flags = O_WRONLY | O_CREAT;
		if (mode == 2)
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;
	}
	return (flags);
}
