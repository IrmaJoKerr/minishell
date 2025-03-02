/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:51:05 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

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
Handle file redirection. Works for both input and output redirection.
mode: 0 for input, 1 for output, 2 for append
Returns: 1 on success, 0 on failure
*/
int	handle_redirect(t_node *node, int *fd, int mode)
{
	int	flags;

	if (!node || !node->args || !node->args[0])
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
	return (*fd != -1);
}
