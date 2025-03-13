/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:51:05 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 02:53:30 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check if token type is a redirection.
Returns 1 if it's a redirection type, 0 otherwise.
Types: TYPE_HEREDOC, TYPE_IN_REDIRECT, TYPE_OUT_REDIRECT, TYPE_APPEND_REDIRECT
*/
int	is_redirection(t_tokentype type)
{
	return (type == TYPE_HEREDOC
		|| type == TYPE_IN_REDIRECT
		|| type == TYPE_OUT_REDIRECT
		|| type == TYPE_APPEND_REDIRECT);
}

/*
Handles input redirection (< filename).
1) Validates the node structure and arguments
2) Checks read permissions on the target file with chk_permissions(). Error
   message is displayed if permissions are denied.
3) Opens the file in read-only mode
4) Sets the provided fd_in pointer to the new FD
Returns 1 on success, 0 on any failure 
(invalid args, permission denied, open failure).
Works with handle_redirect_nodes().
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
Handle output redirection (> filename or >> filename).
1) Validates the node structure and arguments
2) Sets flags based on append mode (> vs >>) with set_output_flags()
3) Checks read permissions on the target file with chk_permissions(). Error
   message is displayed if permissions are denied.
4) Opens the file with mode type and set permissions (0644)
6) Sets the provided fd_out pointer to the new FD
Parameters:
- node: AST node with the filename as args[0]
- fd_out: Pointer to store the opened FD
- append: Flag for APPEND based on mode (0 for >, 1 for >>)
Returns 1 on success, 0 on any failure
(invalid args, permission denied, open failure).
Works with handle_redirect_nodes().
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
1) Checks the node structure and arguments
2) Sets flags based on mode using set_redirect_flags()
3) Checks read permissions on the target file with chk_permissions(). Error
   message is displayed if permissions are denied.
4) Opens the file with mode type and set permissions (0644)
6) Sets the provided fd pointer to the new FD
7) Reports errors if file open fails
Parameters:
- node: AST node with the filename as args[0] 
- fd: Pointer to store the opened FD
- mode: 0 for INPUT (<), 1 for OUTPUT (>), 2 for APPEND (>>)
Returns 1 on success, 0 on any failure.
(invalid args, permission denied, open failure).
Works with handle_redirect().
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
Main redirection controller function.
1) Opens the file using open_redirect_file()
2) Checks the standard file descriptor to redirect (STDIN or STDOUT)
3) Uses dup2() to redirect the standard descriptor to the opened file:
   - INPUT(mode 0): STDIN_FILENO becomes the opened file
   - OUTPUT (modes 1,2): STDOUT_FILENO becomes the opened file
4) Closes the original file descriptor to prevent leaks
5) Returns success/failure status
Parameters:
- node: AST node with the filename as args[0] 
- fd: Pointer to store the opened FD
- mode: 0 for INPUT (<), 1 for OUTPUT (>), 2 for APPEND (>>)
Returns 1 on success, 0 on any failure.
Works with execute_redirects().
*/
/*
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
	if (result == -1)
	{
		close(*fd);
		return (0);
	}
	close(*fd);
	if (result == -1)
		return (0);
	return (1);
}
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
	if (result == -1)
	{
		close(*fd);
		return (0);
	}
	close(*fd);
	return (1);
}
