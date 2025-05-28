/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirects.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:39:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 03:14:05 by bleow            ###   ########.fr       */
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
Works with setup_redirection().
*/
int	setup_in_redir(t_node *node, t_vars *vars)
{
	char		*file;
	int			result;
	struct stat	file_stat;

	if (!node || !node->args || !node->args[0])
		return (0);
	file = node->args[0];
	// Close any previously opened redirection file descriptor
	if (vars->pipes->redirection_fd >= 0)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	// Check file accessibility
	if (!check_input_file_access(file, &file_stat, vars))
		return (0);
	// Set up the redirection
	result = setup_input_redirection(file, vars);
	return (result);
}

/*
Opens input file and sets up stdin redirection.
*/
int	setup_input_redirection(char *file, t_vars *vars)
{
	// Close any previous file descriptor
	if (vars->pipes->redirection_fd >= 0)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	// Open the file
	vars->pipes->redirection_fd = open(file, O_RDONLY);
	if (vars->pipes->redirection_fd == -1)
	{
		vars->error_code = ERR_REDIRECTION;
		return (0);
	}
	// Redirect stdin to the file
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
Works with setup_redirection().
*/
int	setup_out_redir(t_node *node, t_vars *vars)
{
	char	*file;
	int		mode;
	int		result;

	if (!node || !node->args || !node->args[0])
		return (0);
	file = node->args[0];
	// Set mode based on redirection type
	mode = O_WRONLY | O_CREAT | O_TRUNC;
	vars->pipes->out_mode = OUT_MODE_TRUNCATE;
	if (node->type == TYPE_APPEND_REDIRECT)
	{
		mode = O_WRONLY | O_CREAT | O_APPEND;
		vars->pipes->out_mode = OUT_MODE_APPEND;
	}
	// Close any previously opened redirection file descriptor
	if (vars->pipes->redirection_fd >= 0)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	// Check permissions
	if (!chk_permissions(file, mode, vars))
		return (0);
	// Set up the redirection
	result = setup_output_redirection(file, vars);
	return (result);
}

/*
Opens output file and sets up stdout redirection.
*/
int	setup_output_redirection(char *file, t_vars *vars)
{
	int	mode;

	// Close any previous file descriptor
	if (vars->pipes->redirection_fd >= 0)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	// Set the file open mode
	if (vars->pipes->out_mode == OUT_MODE_APPEND)
	{
		mode = O_WRONLY | O_CREAT | O_APPEND;
	}
	else
	{
		mode = O_WRONLY | O_CREAT | O_TRUNC;
	}
	// Open the file
	vars->pipes->redirection_fd = open(file, mode, 0666);
	if (vars->pipes->redirection_fd == -1)
	{
		vars->error_code = ERR_REDIRECTION;
		return (0);
	}
	// Redirect stdout to the file
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
	// Basic heredoc setup checks
	if (!node || !vars)
		return (0);
	// Check if we need to extract the heredoc delimiter
	if (!vars->pipes->heredoc_delim && node->right && node->right->args
		&& node->right->args[0])
	{
		if (!is_valid_delim(node->right->args[0], vars))
			return (0);
	}
	// Gather heredoc content if not already done
	if (!vars->hd_text_ready)
	{
		if (!interactive_hd_mode(vars))
			return (0);
	}
	// Process and set up the heredoc fd
	if (!handle_heredoc(node, vars))
		return (0);
	return (1);
}

/*
Checks if input file exists and has correct permissions.
*/
// int	check_input_file_access(char *file, struct stat *file_stat, t_vars *vars)
// {
// 	// Check if file exists
// 	if (access(file, F_OK) != 0)
// 	{
// 		not_found_error(file, vars);
// 		vars->error_code = ERR_REDIRECTION;
// 		return (handle_missing_input(vars));
// 	}
// 	// Check if file is a directory
// 	if (stat(file, file_stat) == 0 && S_ISDIR(file_stat->st_mode))
// 	{
// 		shell_error(file, ERR_ISDIRECTORY, vars);
// 		vars->error_code = ERR_REDIRECTION;
// 		return (handle_missing_input(vars));
// 	}
// 	// Check permissions
// 	if (!chk_permissions(file, O_RDONLY, vars))
// 	{
// 		vars->error_code = ERR_REDIRECTION;
// 		return (handle_missing_input(vars));
// 	}
// 	return (1);
// }
int check_input_file_access(char *file, struct stat *file_stat, t_vars *vars)
{
	fprintf(stderr, "DEBUG-INPUT-ACCESS: Checking access to file '%s'\n", file);
	
	if (access(file, F_OK) == -1)
	{
		fprintf(stderr, "DEBUG-INPUT-ACCESS: File '%s' does not exist\n", file);
		not_found_error(file, vars);
		return handle_missing_input(vars); // Let handle_missing_input decide what to do
	}
	
	if (stat(file, file_stat) == -1)
	{
		fprintf(stderr, "DEBUG-INPUT-ACCESS: stat() failed for file '%s'\n", file);
		shell_error(file, ERR_DEFAULT, vars);
		return 0;
	}
	
	if (S_ISDIR(file_stat->st_mode))
	{
		fprintf(stderr, "DEBUG-INPUT-ACCESS: File '%s' is a directory\n", file);
		shell_error(file, ERR_ISDIRECTORY, vars);
		return 0;
	}
	
	if (access(file, R_OK) == -1)
	{
		fprintf(stderr, "DEBUG-INPUT-ACCESS: No read permission for file '%s'\n", file);
		shell_error(file, ERR_PERMISSIONS, vars);
		return 0;
	}
	
	fprintf(stderr, "DEBUG-INPUT-ACCESS: File '%s' is accessible\n", file);
	return 1;
}

/*
Handles case when input file is missing or inaccessible.
For pipe contexts, redirects stdin from /dev/null.
*/
// int	handle_missing_input(t_vars *vars)
// {
// 	int	null_fd;
// 	// If in pipe context, redirect stdin to /dev/null
// 	if (vars->pipes && vars->pipes->pipe_root)
// 	{
// 		null_fd = open("/dev/null", O_RDONLY);
// 		if (null_fd != -1)
// 		{
// 			dup2(null_fd, STDIN_FILENO);
// 			close(null_fd);
// 		}
// 	}
// 	return (0);
// }
int handle_missing_input(t_vars *vars)
{
	int null_fd;
	int is_pipeline_context;
	
	is_pipeline_context = (vars->pipes && vars->pipes->pipe_root != NULL);
	
	fprintf(stderr, "DEBUG-MISSING-INPUT: Handling missing input file (pipeline_context=%d)\n", 
			is_pipeline_context);
	
	if (is_pipeline_context)
	{
		// In pipeline context, redirect stdin from /dev/null instead of failing
		fprintf(stderr, "DEBUG-MISSING-INPUT: Pipeline context - redirecting from /dev/null\n");
		null_fd = open("/dev/null", O_RDONLY);
		if (null_fd == -1)
		{
			fprintf(stderr, "DEBUG-MISSING-INPUT: Failed to open /dev/null\n");
			vars->error_code = ERR_DEFAULT;
			return 0;
		}
		
		if (dup2(null_fd, STDIN_FILENO) == -1)
		{
			fprintf(stderr, "DEBUG-MISSING-INPUT: Failed to dup2 /dev/null to stdin\n");
			close(null_fd);
			vars->error_code = ERR_DEFAULT;
			return 0;
		}
		
		close(null_fd);
		fprintf(stderr, "DEBUG-MISSING-INPUT: Successfully redirected stdin from /dev/null\n");
		return 1; // Continue execution
	}
	else
	{
		// Non-pipeline context - fail as before
		fprintf(stderr, "DEBUG-MISSING-INPUT: Non-pipeline context - failing execution\n");
		vars->error_code = ERR_DEFAULT;
		return 0;
	}
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
int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
{
	t_node *current;
	t_node *prev_node = NULL;
	int prev_result = 0;
	int result;
	int chain_length = 0;
	
	fprintf(stderr, "DEBUG-FIX-CHAIN: Starting redirection chain processing\n");
	fprintf(stderr, "DEBUG-FIX-CHAIN: Start node: %s '%s'\n", 
			get_token_str(start_node->type),
			start_node->args ? start_node->args[0] : "NULL");
	fprintf(stderr, "DEBUG-FIX-CHAIN: Command: '%s'\n",
			cmd_node->args ? cmd_node->args[0] : "NULL");
	
	current = start_node;
	
	while (current) {
		// CRITICAL FIX #4: Better cycle detection with previous pointer tracking
		if (current == prev_node) {
			fprintf(stderr, "DEBUG-FIX-CHAIN: Self-reference cycle detected!\n");
			return 0;
		}
		
		// Process the redirection
		fprintf(stderr, "DEBUG-FIX-CHAIN: Processing redirection %s '%s'\n",
				get_token_str(current->type),
				current->args ? current->args[0] : "NULL");
		
		// CRITICAL FIX #5: Process quoted filenames correctly
		if (current->args && current->args[0]) {
			fprintf(stderr, "DEBUG-FIX-CHAIN: Before stripping quotes: '%s'\n", 
					current->args[0]);
			strip_outer_quotes(&current->args[0], vars);
			fprintf(stderr, "DEBUG-FIX-CHAIN: After stripping quotes: '%s'\n", 
					current->args[0]);
		}
		
		result = setup_redirection(current, vars);
		
		if (!result) {
			fprintf(stderr, "DEBUG-FIX-CHAIN: Redirection failed for '%s'\n",
					current->args ? current->args[0] : "NULL");
			return 0;
		}
		
		prev_result = result;
		
		// Save current before advancing
		prev_node = current;
		
		// CRITICAL FIX #6: Use next_redir field for chain traversal
		current = current->next_redir;
		fprintf(stderr, "DEBUG-FIX-CHAIN: Moving to next redirection: %s\n",
				current ? (current->args ? current->args[0] : "NULL") : "END");
		
		// Hard limit on chain length as a fallback
		if (++chain_length > 20) {
			fprintf(stderr, "DEBUG-FIX-CHAIN: Chain too long, possible cycle\n");
			return 0;
		}
	}
	
	fprintf(stderr, "DEBUG-FIX-CHAIN: Chain processing completed successfully\n");
	return prev_result;
}
