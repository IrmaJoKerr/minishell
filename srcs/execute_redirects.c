/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirects.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:39:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 01:33:36 by bleow            ###   ########.fr       */
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
	if (node->type == TYPE_APPEND_REDIRECT)
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
		if (!interactive_hd_mode(vars))
			return (0);
	}
	if (!handle_heredoc(node, vars))
		return (0);
	return (1);
}

/*
Checks if input file exists and has correct permissions.
*/
int	chk_in_file_access(char *file, struct stat *file_stat, t_vars *vars)
{
	if (access(file, F_OK) == -1)
	{
		not_found_error(file, vars);
		return (handle_bad_infile(vars));
	}
	if (stat(file, file_stat) == -1)
	{
		shell_error(file, ERR_DEFAULT, vars);
		return (0);
	}
	if (S_ISDIR(file_stat->st_mode))
	{
		shell_error(file, ERR_ISDIRECTORY, vars);
		return (0);
	}
	if (access(file, R_OK) == -1)
	{
		shell_error(file, ERR_PERMISSIONS, vars);
		return (0);
	}
	return (1);
}

/*
Handles case when input file is missing or inaccessible.
For pipe contexts, redirects stdin from /dev/null.
*/
int	handle_bad_infile(t_vars *vars)
{
	int	null_fd;

	if (vars->pipes && vars->pipes->pipe_root != NULL)
	{
		null_fd = open("/dev/null", O_RDONLY);
		if (null_fd == -1)
		{
			vars->error_code = ERR_DEFAULT;
			return (0);
		}
		if (dup2(null_fd, STDIN_FILENO) == -1)
		{
			close(null_fd);
			vars->error_code = ERR_DEFAULT;
			return (0);
		}
		close(null_fd);
		return (1);
	}
	else
		return (0);
}

/*
Processes a chain of redirections for a command node.
- Sets up each redirection in the chain.
- Navigates through connected redirections.
- Handles errors and cleanup if a redirection fails.
Returns:
- 1 on success (all redirections processed)
- 0 on failure (at least one redirection failed)
Works with exec_redirect_cmd().
*/
int	proc_redir_chain(t_node *start_node, t_vars *vars)
{
	t_node	*current;
	t_node	*prev_node;
	int		prev_result;
	int		result;

	prev_node = NULL;
	current = start_node;
	while (current)
	{
		if (current == prev_node)
			return (0);
		result = setup_redirection(current, vars);
		if (!result)
			return (0);
		prev_result = result;
		prev_node = current;
		current = current->next_redir;
	}
	return (prev_result);
}
