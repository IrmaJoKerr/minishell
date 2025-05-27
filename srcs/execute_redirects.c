/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirects.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:39:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/27 19:27:16 by bleow            ###   ########.fr       */
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
// int setup_in_redir(t_node *node, t_vars *vars)
// {
//     char *file;
//     struct stat file_stat;
//     fprintf(stderr, "DEBUG-INREDIR: Setting up input redirection\n");
//     if (!node)
//     {
//         fprintf(stderr, "DEBUG-INREDIR: NULL node\n");
//         return (0);
//     }
//     fprintf(stderr, "DEBUG-INREDIR: Node args pointer: %p\n", (void*)node->args);
//     if (!node->args || !node->args[0])
//     {
//         fprintf(stderr, "DEBUG-INREDIR: Missing filename in args[0]\n");
//         return (0);
//     }
//     file = node->args[0];
//     fprintf(stderr, "DEBUG-INREDIR: Using filename: '%s'\n", file);
//     // Close any previously opened redirection file descriptor
//     if (vars->pipes->redirection_fd >= 0)
//     {
//         fprintf(stderr, "DEBUG-INREDIR: Closing previous fd %d\n", 
//                 vars->pipes->redirection_fd);
//         close(vars->pipes->redirection_fd);
//         vars->pipes->redirection_fd = -1;
//     }
//     // Check file existence and type
//     fprintf(stderr, "DEBUG-INREDIR: Checking file access: '%s'\n", file);
//     if (!check_input_file_access(file, &file_stat, vars))
//     {
//         fprintf(stderr, "DEBUG-INREDIR: File access check failed for '%s'\n", file);
//         return (0);
//     }
//     // Open the file and set up redirection
//     fprintf(stderr, "DEBUG-INREDIR: Setting up input redirection for file: '%s'\n", file);
//     return setup_input_redirection(file, vars);
// }
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
// int setup_input_redirection(char *file, t_vars *vars)
// {
//     fprintf(stderr, "DEBUG-INPUT-REDIR: Setting up input redirection for file: '%s'\n", file);
//     // Close any previous file descriptor
//     if (vars->pipes->redirection_fd >= 0)
//     {
//         close(vars->pipes->redirection_fd);
//         vars->pipes->redirection_fd = -1;
//     }
//     // Open the file
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG-INPUT-REDIR: Failed to open file '%s': %s\n", 
//                 file, strerror(errno));
//         vars->error_code = ERR_REDIRECTION;
//         return (0);
//     }
//     fprintf(stderr, "DEBUG-INPUT-REDIR: Successfully opened file '%s' (fd=%d)\n", 
//             file, vars->pipes->redirection_fd);
//     // Redirect stdin to the file
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-INPUT-REDIR: dup2 failed: %s\n", strerror(errno));
//         close(vars->pipes->redirection_fd);
//         vars->pipes->redirection_fd = -1;
//         vars->error_code = ERR_REDIRECTION;
//         return (0);
//     }
//     fprintf(stderr, "DEBUG-INPUT-REDIR: Successfully redirected stdin to file '%s'\n", file);
//     return (1);
// }
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
// int setup_out_redir(t_node *node, t_vars *vars)
// {
//     char    *file;
//     if (!node->args || !node->args[0])
//         return (0);
//     file = node->args[0];
//     // Close any previously opened redirection file descriptor
//     if (vars->pipes->redirection_fd >= 0)
//     {
//         close(vars->pipes->redirection_fd);
//         vars->pipes->redirection_fd = -1;
//     }
//     // Set up output redirection
//     return setup_output_redirection(file, vars);
// }
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
// int setup_output_redirection(char *file, t_vars *vars)
// {
//     int flags;
//     // Set flags based on redirection mode
//     if (vars->pipes->out_mode == OUT_MODE_APPEND)
//         flags = O_WRONLY | O_CREAT | O_APPEND;
//     else
//         flags = O_WRONLY | O_CREAT | O_TRUNC;
//     if (!chk_permissions(file, flags, vars))
//     {
//         // Only log on failure
//         fprintf(stderr, "DEBUG: Permission denied for '%s'\n", file);
//         vars->error_code = ERR_REDIRECTION;
//         return (0);
//     }
//     vars->pipes->redirection_fd = open(file, flags, 0644);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG: Failed to open '%s'\n", file);
//         shell_error(file, ERR_PERMISSIONS, vars);
//         vars->error_code = ERR_REDIRECTION;
//         return (0);
//     }
//     // Reset stdout to its original state first
//     if (dup2(STDOUT_FILENO, STDOUT_FILENO) == -1)
//         fprintf(stderr, "DEBUG: Failed to reset stdout\n");
//     if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG: dup2 failed\n");
//         close(vars->pipes->redirection_fd);
//         vars->pipes->redirection_fd = -1;
//         return (0);
//     }
//     return (1);
// }
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
// int check_input_file_access(char *file, struct stat *file_stat, t_vars *vars)
// {
//     // Check if file exists
//     if (access(file, F_OK) != 0)
//     {
//         // Only log on failure
//         fprintf(stderr, "DEBUG: File '%s' not found\n", file);
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         return handle_missing_input(vars);
//     }
//     // Check if file is a directory
//     if (stat(file, file_stat) == 0 && S_ISDIR(file_stat->st_mode))
//     {
//         // Only log on failure
//         fprintf(stderr, "DEBUG: '%s' is a directory\n", file);
//         shell_error(file, ERR_ISDIRECTORY, vars);
//         vars->error_code = ERR_REDIRECTION;
//         return handle_missing_input(vars);
//     }
//     // Check permissions
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         // Only log on failure
//         fprintf(stderr, "DEBUG: No permission for '%s'\n", file);
//         vars->error_code = ERR_REDIRECTION;
//         return handle_missing_input(vars);
//     }
//     return (1);
// }
int	check_input_file_access(char *file, struct stat *file_stat, t_vars *vars)
{
	// Check if file exists
	if (access(file, F_OK) != 0)
	{
		not_found_error(file, vars);
		vars->error_code = ERR_REDIRECTION;
		return (handle_missing_input(vars));
	}
	// Check if file is a directory
	if (stat(file, file_stat) == 0 && S_ISDIR(file_stat->st_mode))
	{
		shell_error(file, ERR_ISDIRECTORY, vars);
		vars->error_code = ERR_REDIRECTION;
		return (handle_missing_input(vars));
	}
	// Check permissions
	if (!chk_permissions(file, O_RDONLY, vars))
	{
		vars->error_code = ERR_REDIRECTION;
		return (handle_missing_input(vars));
	}
	return (1);
}

/*
Handles case when input file is missing or inaccessible.
For pipe contexts, redirects stdin from /dev/null.
*/
int	handle_missing_input(t_vars *vars)
{
	int	null_fd;
	// If in pipe context, redirect stdin to /dev/null
	if (vars->pipes && vars->pipes->pipe_root)
	{
		null_fd = open("/dev/null", O_RDONLY);
		if (null_fd != -1)
		{
			dup2(null_fd, STDIN_FILENO);
			close(null_fd);
		}
	}
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
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing %s redirection for file: %s\n", 
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL");
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection failed for %s\n", 
//                     current_node->args ? current_node->args[0] : "NULL");
//             redir_status = 0;
//             break;
//         }
//         next_redir = current_node->redir;
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Next redirection in chain: %p (%s)\n", 
//                 (void*)next_redir, 
//                 next_redir ? get_token_str(next_redir->type) : "NULL");
//         if (!next_redir)
//             break;
//         current_node = next_redir;
//     }
//     // Only log final status if there was a failure
//     if (!redir_status)
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain failed\n");
//     else
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain completed successfully\n");
//     return redir_status;
// }
// int	proc_redir_chain(t_node *start_node, t_vars *vars)
// {
// 	t_node	*current_node;
// 	t_node	*next_redir;
// 	int		redir_status;

// 	redir_status = 1;
// 	(void)cmd_node; // Suppress unused parameter warning
// 	current_node = start_node;
// 	while (current_node && is_redirection(current_node->type))
// 	{
// 		// Setup this redirection
// 		if (!setup_redirection(current_node, vars))
// 		{
// 			redir_status = 0;
// 			break ;
// 		}
// 		// Find next redirection in the chain
// 		next_redir = current_node->redir;
// 		if (!next_redir)
// 			break ;
// 		current_node = next_redir;
// 	}
// 	return (redir_status);
// }
// int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
// 	t_node	*current_node;
// 	t_node	*next_redir;
// 	int		redir_status;
// 	int		prev_in_redir_error;

// 	redir_status = 1;
// 	prev_in_redir_error = 0;
// 	(void)cmd_node; // Suppress unused parameter warning
	
// 	current_node = start_node;
// 	while (current_node && is_redirection(current_node->type))
// 	{
// 		fprintf(stderr, "DEBUG-REDIR-EXEC: Processing %s redirection '%s', prev_result=%d\n",
// 			get_token_str(current_node->type),
// 			current_node->args ? current_node->args[0] : "NULL",
// 			prev_in_redir_error);
		
// 		// Skip input redirections if we've already had an error
// 		if (prev_in_redir_error && current_node->type == TYPE_IN_REDIRECT)
// 		{
// 			fprintf(stderr, "DEBUG-REDIR-EXEC: Skipping '%s' due to previous error\n",
// 				current_node->args ? current_node->args[0] : "NULL");
// 			next_redir = current_node->redir;
// 			if (!next_redir)
// 				break;
// 			current_node = next_redir;
// 			continue;
// 		}
		
// 		// Setup this redirection
// 		if (!setup_redirection(current_node, vars))
// 		{
// 			// Record error for input redirections
// 			if (current_node->type == TYPE_IN_REDIRECT)
// 			{
// 				prev_in_redir_error = 1;
// 				fprintf(stderr, "DEBUG-REDIR-EXEC: Input redirection error\n");
// 				// Don't break for input redirection errors, continue to next redirection
// 				next_redir = current_node->redir;
// 				if (!next_redir)
// 					break;
// 				current_node = next_redir;
// 				continue;
// 			}
// 			else
// 			{
// 				// For output redirections, still break on error
// 				redir_status = 0;
// 				break;
// 			}
// 		}
		
// 		// Find next redirection in the chain
// 		next_redir = current_node->redir;
// 		if (!next_redir)
// 			break;
// 		current_node = next_redir;
// 	}
	
// 	return (redir_status);
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;
//     int prev_in_redir_error = 0;
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-EXEC: Processing %s redirection '%s', prev_result=%d\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL",
//                 prev_in_redir_error);
        
//         // Skip input redirections if we've already had an error
//         if (prev_in_redir_error && current_node->type == TYPE_IN_REDIRECT)
//         {
//             fprintf(stderr, "DEBUG-REDIR-EXEC: Skipping '%s' due to previous error\n",
//                    current_node->args ? current_node->args[0] : "NULL");
//             next_redir = current_node->redir;
//             if (!next_redir)
//                 break;
//             current_node = next_redir;
//             continue;
//         }
        
//         // Use your existing setup_redirection function
//         if (!setup_redirection(current_node, vars))
//         {
//             // For input redirections, record error but continue
//             if (current_node->type == TYPE_IN_REDIRECT)
//             {
//                 prev_in_redir_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-EXEC: Input redirection error\n");
//                 next_redir = current_node->redir;
//                 if (!next_redir)
//                     break;
//                 current_node = next_redir;
//                 continue;
//             }
//             else
//             {
//                 // For output redirections, break on error
//                 redir_status = 0;
//                 break;
//             }
//         }
        
//         next_redir = current_node->redir;
//         if (!next_redir)
//             break;
//         current_node = next_redir;
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirections, status=%d\n", redir_status);
//     return (redir_status);
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;
//     int prev_in_redir_error = 0;
    
//     // Enhanced debug print - function entry
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Starting chain processing for cmd '%s'\n",
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         // Enhanced debug print - processing each redirection
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection type=%s, filename='%s'\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL");
//         fprintf(stderr, "DEBUG-REDIR-EXEC: Processing %s redirection '%s', prev_result=%d\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL",
//                 prev_in_redir_error);
        
//         // Skip input redirections if we've already had an error
//         if (prev_in_redir_error && current_node->type == TYPE_IN_REDIRECT)
//         {
//             fprintf(stderr, "DEBUG-REDIR-EXEC: Skipping '%s' due to previous error\n",
//                    current_node->args ? current_node->args[0] : "NULL");
//             next_redir = current_node->redir;
//             if (!next_redir)
//                 break;
//             current_node = next_redir;
//             continue;
//         }
        
//         // Use your existing setup_redirection function
//         if (!setup_redirection(current_node, vars))
//         {
//             // Enhanced debug print - redirection failure
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection result: success=%d, continuing=%s\n",
//                     0, "no");
            
//             // For input redirections, record error but continue
//             if (current_node->type == TYPE_IN_REDIRECT)
//             {
//                 prev_in_redir_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-EXEC: Input redirection error\n");
//                 next_redir = current_node->redir;
//                 if (!next_redir)
//                     break;
//                 current_node = next_redir;
//                 continue;
//             }
//             else
//             {
//                 // For output redirections, break on error
//                 redir_status = 0;
//                 break;
//             }
//         }
//         else
//         {
//             // Enhanced debug print - redirection success
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection result: success=%d, continuing=%s\n",
//                     1, "yes");
//         }
        
//         next_redir = current_node->redir;
//         if (!next_redir)
//             break;
//         current_node = next_redir;
//     }
    
//     // Enhanced debug print - function completion
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain processing completed, final_result=%d\n", redir_status);
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirections, status=%d\n", redir_status);
//     return (redir_status);
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;
//     int prev_in_redir_error = 0;
//     int any_error_occurred = 0;  // NEW: Track if any error occurred
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Starting chain processing for cmd '%s'\n",
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection type=%s, filename='%s'\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL");
//         fprintf(stderr, "DEBUG-REDIR-EXEC: Processing %s redirection '%s', prev_result=%d\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL",
//                 prev_in_redir_error);
        
//         // Skip input redirections if we've already had an error
//         if (prev_in_redir_error && current_node->type == TYPE_IN_REDIRECT)
//         {
//             fprintf(stderr, "DEBUG-REDIR-EXEC: Skipping '%s' due to previous error\n",
//                    current_node->args ? current_node->args[0] : "NULL");
//             next_redir = current_node->redir;
//             if (!next_redir)
//                 break;
//             current_node = next_redir;
//             continue;
//         }
        
//         // Use your existing setup_redirection function
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection result: success=%d, continuing=%s\n",
//                     0, "no");
            
//             // NEW: Set any_error_occurred for all redirection types
//             any_error_occurred = 1;
//             fprintf(stderr, "DEBUG-REDIR-FIX: Setting any_error_occurred=1 for failed %s\n",
//                     get_token_str(current_node->type));
            
//             // For input redirections, record error but continue
//             if (current_node->type == TYPE_IN_REDIRECT)
//             {
//                 prev_in_redir_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-EXEC: Input redirection error\n");
//                 next_redir = current_node->redir;
//                 if (!next_redir)
//                     break;
//                 current_node = next_redir;
//                 continue;
//             }
//             else
//             {
//                 // For output redirections, break on error
//                 redir_status = 0;
//                 fprintf(stderr, "DEBUG-REDIR-FIX: Breaking on output redirection error\n");
//                 break;
//             }
//         }
//         else
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection result: success=%d, continuing=%s\n",
//                     1, "yes");
//         }
        
//         next_redir = current_node->redir;
//         if (!next_redir)
//             break;
//         current_node = next_redir;
//     }
    
//     // NEW: Fix the return logic - if any error occurred, return 0
//     if (any_error_occurred)
//     {
//         redir_status = 0;
//         fprintf(stderr, "DEBUG-REDIR-FIX: Returning 0 due to any_error_occurred=1\n");
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain processing completed, final_result=%d\n", redir_status);
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirections, status=%d\n", redir_status);
//     return (redir_status);
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;
//     int prev_in_redir_error = 0;
//     int any_input_error = 0;      // NEW: Track input redirection errors
//     int any_output_error = 0;     // NEW: Track output redirection errors
//     int is_pipeline_context = 0;  // NEW: Detect if we're in a pipeline
    
//     // NEW: Detect pipeline context
//     is_pipeline_context = (vars->pipes && vars->pipes->pipe_root != NULL);
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Starting chain processing for cmd '%s' (pipeline_context=%d)\n",
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL", is_pipeline_context);
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection type=%s, filename='%s'\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL");
//         fprintf(stderr, "DEBUG-REDIR-EXEC: Processing %s redirection '%s', prev_result=%d\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL",
//                 prev_in_redir_error);
        
//         // Skip input redirections if we've already had an error
//         if (prev_in_redir_error && current_node->type == TYPE_IN_REDIRECT)
//         {
//             fprintf(stderr, "DEBUG-REDIR-EXEC: Skipping '%s' due to previous error\n",
//                    current_node->args ? current_node->args[0] : "NULL");
//             next_redir = current_node->redir;
//             if (!next_redir)
//                 break;
//             current_node = next_redir;
//             continue;
//         }
        
//         // Use your existing setup_redirection function
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection result: success=%d, continuing=%s\n",
//                     0, "no");
            
//             // NEW: Categorize the error type
//             if (current_node->type == TYPE_IN_REDIRECT || current_node->type == TYPE_HEREDOC)
//             {
//                 any_input_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-FIX: Setting any_input_error=1 for failed %s\n",
//                         get_token_str(current_node->type));
                
//                 prev_in_redir_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-EXEC: Input redirection error\n");
//                 next_redir = current_node->redir;
//                 if (!next_redir)
//                     break;
//                 current_node = next_redir;
//                 continue;
//             }
//             else if (current_node->type == TYPE_OUT_REDIRECT || current_node->type == TYPE_APPEND_REDIRECT)
//             {
//                 any_output_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-FIX: Setting any_output_error=1 for failed %s\n",
//                         get_token_str(current_node->type));
                
//                 // For output redirections, always break on error
//                 redir_status = 0;
//                 fprintf(stderr, "DEBUG-REDIR-FIX: Breaking on output redirection error\n");
//                 break;
//             }
//         }
//         else
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection result: success=%d, continuing=%s\n",
//                     1, "yes");
//         }
        
//         next_redir = current_node->redir;
//         if (!next_redir)
//             break;
//         current_node = next_redir;
//     }
    
//     // NEW: Context-aware error handling
//     if (any_output_error)
//     {
//         // Output errors always fail
//         redir_status = 0;
//         fprintf(stderr, "DEBUG-REDIR-FIX: Returning 0 due to output redirection error\n");
//     }
//     else if (any_input_error && !is_pipeline_context)
//     {
//         // Input errors fail only in non-pipeline context
//         redir_status = 0;
//         fprintf(stderr, "DEBUG-REDIR-FIX: Returning 0 due to input error in standalone command\n");
//     }
//     else if (any_input_error && is_pipeline_context)
//     {
//         // Input errors in pipeline context: let pipeline continue
//         redir_status = 1;  // Don't fail the redirection chain
//         fprintf(stderr, "DEBUG-REDIR-FIX: Allowing pipeline to continue despite input redirection error\n");
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain processing completed, final_result=%d\n", redir_status);
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirections, status=%d\n", redir_status);
//     return (redir_status);
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;
//     int prev_in_redir_error = 0;
//     int any_input_error = 0;      // NEW: Track input redirection errors
//     int any_output_error = 0;     // NEW: Track output redirection errors
//     int is_pipeline_context = 0;  // NEW: Detect if we're in a pipeline
    
//     // NEW: Detect pipeline context
//     is_pipeline_context = (vars->pipes && vars->pipes->pipe_root != NULL);
    
//     // NEW: Enhanced debug at start
//     fprintf(stderr, "DEBUG-REDIR-CHAIN-ENHANCED: === REDIRECTION CHAIN START ===\n");
//     fprintf(stderr, "DEBUG-REDIR-CHAIN-ENHANCED: start_node type: %s, filename: '%s'\n",
//             start_node ? get_token_str(start_node->type) : "NULL",
//             start_node && start_node->args ? start_node->args[0] : "NULL");
//     fprintf(stderr, "DEBUG-REDIR-CHAIN-ENHANCED: cmd_node type: %s, command: '%s'\n",
//             cmd_node ? get_token_str(cmd_node->type) : "NULL",
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
//     fprintf(stderr, "DEBUG-REDIR-CHAIN-ENHANCED: Pipeline context: %s\n",
//             (vars->pipes && vars->pipes->pipe_root) ? "YES" : "NO");
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Starting chain processing for cmd '%s' (pipeline_context=%d)\n",
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL", is_pipeline_context);
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection type=%s, filename='%s'\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL");
//         fprintf(stderr, "DEBUG-REDIR-EXEC: Processing %s redirection '%s', prev_result=%d\n",
//                 get_token_str(current_node->type),
//                 current_node->args ? current_node->args[0] : "NULL",
//                 prev_in_redir_error);
        
//         // Skip input redirections if we've already had an error
//         if (prev_in_redir_error && current_node->type == TYPE_IN_REDIRECT)
//         {
//             fprintf(stderr, "DEBUG-REDIR-EXEC: Skipping '%s' due to previous error\n",
//                    current_node->args ? current_node->args[0] : "NULL");
//             next_redir = current_node->redir;
//             if (!next_redir)
//                 break;
//             current_node = next_redir;
//             continue;
//         }
        
//         // Use your existing setup_redirection function
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection result: success=%d, continuing=%s\n",
//                     0, "no");
            
//             // NEW: Categorize the error type
//             if (current_node->type == TYPE_IN_REDIRECT || current_node->type == TYPE_HEREDOC)
//             {
//                 any_input_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-FIX: Setting any_input_error=1 for failed %s\n",
//                         get_token_str(current_node->type));
                
//                 prev_in_redir_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-EXEC: Input redirection error\n");
//                 next_redir = current_node->redir;
//                 if (!next_redir)
//                     break;
//                 current_node = next_redir;
//                 continue;
//             }
//             else if (current_node->type == TYPE_OUT_REDIRECT || current_node->type == TYPE_APPEND_REDIRECT)
//             {
//                 any_output_error = 1;
//                 fprintf(stderr, "DEBUG-REDIR-FIX: Setting any_output_error=1 for failed %s\n",
//                         get_token_str(current_node->type));
                
//                 // For output redirections, always break on error
//                 redir_status = 0;
//                 fprintf(stderr, "DEBUG-REDIR-FIX: Breaking on output redirection error\n");
//                 break;
//             }
//         }
//         else
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection result: success=%d, continuing=%s\n",
//                     1, "yes");
//         }
        
//         next_redir = current_node->redir;
//         if (!next_redir)
//             break;
//         current_node = next_redir;
//     }
    
//     // NEW: Context-aware error handling
//     if (any_output_error)
//     {
//         // Output errors always fail
//         redir_status = 0;
//         fprintf(stderr, "DEBUG-REDIR-FIX: Returning 0 due to output redirection error\n");
//     }
//     else if (any_input_error && !is_pipeline_context)
//     {
//         // Input errors fail only in non-pipeline context
//         redir_status = 0;
//         fprintf(stderr, "DEBUG-REDIR-FIX: Returning 0 due to input error in standalone command\n");
//     }
//     else if (any_input_error && is_pipeline_context)
//     {
//         // Input errors in pipeline context: let pipeline continue
//         redir_status = 1;  // Don't fail the redirection chain
//         fprintf(stderr, "DEBUG-REDIR-FIX: Allowing pipeline to continue despite input redirection error\n");
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain processing completed, final_result=%d\n", redir_status);
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirections, status=%d\n", redir_status);
//     return (redir_status);
// }
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