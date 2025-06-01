/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_redirects.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 12:18:17 by bleow             #+#    #+#             */
/*   Updated: 2025/06/01 23:25:22 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles redirection setup for input files.
- Opens file for reading.
- Redirects stdin to read from the file.
- Properly handles and reports errors.
Returns:
- 1 on success.
- 0 on failure.
*/
int	setup_in_redir(t_node *node, t_vars *vars)
{
	char		*file;
	int			result;
	struct stat	file_stat;

	if (!node || !node->args || !node->args[0])
		return (0);
	file = node->args[0];
	if (vars->pipes->redirection_fd >= 0)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	if (!chk_in_file_access(file, &file_stat, vars))
		return (0);
	result = setup_input_redirection(file, vars);
	return (result);
}

/*
Opens input file and sets up stdin redirection.
*/
int	setup_input_redirection(char *file, t_vars *vars)
{
	if (vars->pipes->redirection_fd >= 0)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	vars->pipes->redirection_fd = open(file, O_RDONLY);
	if (vars->pipes->redirection_fd == -1)
	{
		vars->error_code = ERR_REDIRECTION;
		return (0);
	}
	if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
		vars->error_code = ERR_REDIRECTION;
		return (0);
	}
	return (1);
}

/*
Handles redirection setup for output files.
- Opens file for writing in truncate or append mode.
- Redirects stdout to the opened file.
- Properly handles and reports errors.
Returns:
- 1 on success.
- 0 on failure.
*/
int	setup_out_redir(t_node *node, t_vars *vars)
{
	char	*file;
	int		mode;
	int		result;

	if (!node || !node->args || !node->args[0])
		return (0);
	file = node->args[0];
	mode = O_WRONLY | O_CREAT | O_TRUNC;
	vars->pipes->out_mode = OUT_MODE_TRUNCATE;
	if (node->type == TYPE_APPD_REDIR)
	{
		mode = O_WRONLY | O_CREAT | O_APPEND;
		vars->pipes->out_mode = OUT_MODE_APPEND;
	}
	if (vars->pipes->redirection_fd >= 0)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	if (!chk_permissions(file, mode, vars))
		return (0);
	result = setup_output_redirection(file, vars);
	return (result);
}

/*
Opens output file and sets up stdout redirection.
*/
int	setup_output_redirection(char *file, t_vars *vars)
{
	int	mode;

	if (vars->pipes->redirection_fd >= 0)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	if (vars->pipes->out_mode == OUT_MODE_APPEND)
		mode = O_WRONLY | O_CREAT | O_APPEND;
	else
		mode = O_WRONLY | O_CREAT | O_TRUNC;
	vars->pipes->redirection_fd = open(file, mode, 0666);
	if (vars->pipes->redirection_fd == -1)
	{
		vars->error_code = ERR_REDIRECTION;
		return (0);
	}
	if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
		vars->error_code = ERR_REDIRECTION;
		return (0);
	}
	return (1);
}

/*
Sets up heredoc redirection handling.
- Checks if heredoc content is ready.
- Initiates interactive mode if end delimiter not found.
- Handles the actual heredoc setup via handle_heredoc().
Returns:
- 1 on success
- 0 on failure
Works with redir_mode_setup().
*/
int	setup_heredoc_redir(t_node *node, t_vars *vars)
{
	if (!node || !vars)
		return (0);
	if (!vars->pipes->heredoc_delim && node->right && node->right->args
		&& node->right->args[0])
	{
		if (!is_valid_delim(node->right->args[0], vars))
			return (0);
	}
	if (!vars->hd_text_ready)
	{
		if (!interactive_hd_mode(vars, g_signal_received))
			return (0);
	}
	if (!handle_heredoc(node, vars))
		return (0);
	return (1);
}
