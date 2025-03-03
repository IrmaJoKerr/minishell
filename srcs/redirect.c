/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:51:05 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 13:00:11 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handle input redirection. Works by opening the file and setting the
file descriptor and permissions.
*/
int	input_redirect(t_node *node, int *fd_in)
{
	if (!node || !node->args || !node->args[0])
		return (0);
	if (!chk_permissions(node->args[0], O_RDONLY))
	{
		redirect_error(node->args[0]);
		return (0);
	}
	*fd_in = open(node->args[0], O_RDONLY);
	return (*fd_in != -1);
}

/*
Handle output redirection. Works by opening the file and setting the
file descriptor and permissions.
*/
int	output_redirect(t_node *node, int *fd_out, int append)
{
	int	flags;

	if (!node || !node->args || !node->args[0])
		return (0);
	flags = set_output_flags(append);
	if (!chk_permissions(node->args[0], flags))
	{
		redirect_error(node->args[0]);
		return (0);
	}
	*fd_out = open(node->args[0], flags, 0644);
	return (*fd_out != -1);
}

/*
Open file for redirection based on mode.
Mode: 0 for input, 1 for output, 2 for append
Returns: 1 on success, 0 on failure
Works with handle_redirect.
*/
int	open_redirect_file(t_node *node, int *fd, int mode)
{
	int	flags;

	if (!node || !node->args || !node->args[0] || !fd)
		return (0);
	flags = set_redirect_flags(mode);
	if (!chk_permissions(node->args[0], flags))
	{
		redirect_error(node->args[0]);
		return (0);
	}
	if (mode == 0)
		*fd = open(node->args[0], flags);
	else
		*fd = open(node->args[0], flags, 0644);
	if (*fd == -1)
	{
		redirect_error(node->args[0]);
		return (0);
	}
	return (1);
}

/*
Handle file redirection. Works for input, output and append.
mode: 0 for input, 1 for output, 2 for append
Returns: 1 on success, 0 on failure
*/
int	handle_redirect(t_node *node, int *fd, int mode)
{
	int	std_fd;
	int	result;
	int	success;

	success = open_redirect_file(node, fd, mode);
	if (!success)
		return (0);
	if (mode == 0)
		std_fd = STDIN_FILENO;
	else
		std_fd = STDOUT_FILENO;
	result = dup2(*fd, std_fd);
	close(*fd);
	if (result == -1)
		return (0);
	return (1);
}
