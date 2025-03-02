/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 07:59:58 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:31 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

/*
Check file permissions for redirection
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
Set flags for output redirection. Works for both output and append.
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
Set flags based on redirection mode.
mode: 0 for input, 1 for output, 2 for append
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
