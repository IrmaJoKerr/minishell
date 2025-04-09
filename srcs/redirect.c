/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:51:05 by bleow             #+#    #+#             */
/*   Updated: 2025/04/09 20:52:36 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Checks file permissions before redirection starts.
- For read mode: Verifies file exists and is readable.
- For write mode: Checks if file exists and is writable.
- For new files: Returns 1 to allow creation.
Returns:
- 1 if file can be accessed with requested mode.
- 0 if not.
Works with open_redirect_file() for permission validation.
*/
int	chk_permissions(char *filename, int mode, t_vars *vars)
{
    fprintf(stderr, "DEBUG: Checking permissions for file: '%s', mode: %d\n", 
            filename, mode);
    if (mode == O_RDONLY)
	{
	
        if (access(filename, F_OK | R_OK) == -1)
		{
            ft_putstr_fd("bleshell: ", 2);
            ft_putstr_fd(filename, 2);
            if (access(filename, F_OK) == -1)
                ft_putendl_fd(": No such file or directory", 2);
            else
                ft_putendl_fd(": Permission denied", 2);
            vars->error_code = 1;
            return (0);
        }
    } else if (mode & O_WRONLY) {
        if (access(filename, F_OK) == 0)
		{
            if (access(filename, W_OK) == -1)
			{
                ft_putstr_fd("bleshell: ", 2);
                ft_putstr_fd(filename, 2);
                ft_putendl_fd(": Permission denied", 2);
                vars->error_code = 1;
                return (0);
            }
        }
		else
		{
            char *dir_path = ft_strdup(filename);
            char *last_slash = ft_strrchr(dir_path, '/');
            if (last_slash) {
                *last_slash = '\0';
                if (*dir_path == '\0')
                    strcpy(dir_path, ".");
            } else {
                strcpy(dir_path, ".");
            }
            if (access(dir_path, W_OK) == -1) {
                ft_putstr_fd("bleshell: ", 2);
                ft_putstr_fd(filename, 2);
                ft_putendl_fd(": Permission denied", 2);
                free(dir_path);
                vars->error_code = 1;
                return (0);
            }
            free(dir_path);
        }
    }
    return (1);
}

// /*
// Sets file open flags for output redirection.
// - Configures flags for > (overwrite) or >> (append) modes.
// - Always includes O_WRONLY and O_CREAT flags.
// - Adds O_APPEND for append mode or O_TRUNC for overwrite.
// Returns:
// - Combined flag value to use with open().
// Works with output_redirect() to set proper file flags.

// Example: For append redirection ">>"
// - Returns O_WRONLY | O_CREAT | O_APPEND
// - For standard redirection, returns O_WRONLY | O_CREAT | O_TRUNC
// */
// int	set_output_flags(int append)
// {
// 	int	flags;

// 	flags = O_WRONLY | O_CREAT;
// 	if (append)
// 		flags |= O_APPEND;
// 	else
// 		flags |= O_TRUNC;
// 	return (flags);
// }

// /*
// Sets file open flags for all redirection types.
// - For input (<): Uses O_RDONLY.
// - For output (>): Uses O_WRONLY | O_CREAT | O_TRUNC.
// - For append (>>): Uses O_WRONLY | O_CREAT | O_APPEND.
// Returns:
// - Combined flag value to use with open().
// Works with open_redirect_file() to set file access mode.

// Example: For different redirection types
// - mode 0 (input): Returns O_RDONLY
// - mode 1 (output): Returns O_WRONLY | O_CREAT | O_TRUNC
// - mode 2 (append): Returns O_WRONLY | O_CREAT | O_APPEND
// */
// int	set_redirect_flags(int mode)
// {
// 	int	flags;

// 	if (mode == 0)
// 		flags = O_RDONLY;
// 	else
// 	{
// 		flags = O_WRONLY | O_CREAT;
// 		if (mode == 2)
// 			flags |= O_APPEND;
// 		else
// 			flags |= O_TRUNC;
// 	}
// 	return (flags);
// }

/*
Determines if token is a redirection operator.
- Checks if token type matches any redirection types.
- Includes all input and output redirection variants.
Returns:
- 1 if token is a redirection.
- 0 if not.
Works with process_redirections() and other redirection handlers.

Example: When processing token list
- Returns 1 for tokens of type <, >, >>, or <<
- Returns 0 for command, pipe, or other token types
*/
int	is_redirection(t_tokentype type)
{
	if (type == TYPE_HEREDOC || type == TYPE_IN_REDIRECT
		|| type == TYPE_OUT_REDIRECT || type == TYPE_APPEND_REDIRECT)
		return (1);
	else
		return (0);
}

// /*
// Resets saved standard file descriptors.
// - Restores original stdin and stdout if they were changed.
// - Closes any open heredoc file descriptor.
// - Updates the pipes state in vars.
// Works with execute_cmd() to clean up after command execution.

// Example: After command execution
// - Restores original stdin/stdout
// - Cleans up any open file descriptors
// - Resets pipes state for next command
// */
// void reset_redirect_fds(t_vars *vars)
// {
//     if (!vars || !vars->pipes)
//         return ;
//     if (vars->pipes->saved_stdin > 2)
//     {
//         dup2(vars->pipes->saved_stdin, STDIN_FILENO);
//         close(vars->pipes->saved_stdin);
//         vars->pipes->saved_stdin = -1;
//     }
//     if (vars->pipes->saved_stdout > 2)
//     {
//         dup2(vars->pipes->saved_stdout, STDOUT_FILENO);
//         close(vars->pipes->saved_stdout);
//         vars->pipes->saved_stdout = -1;
//     }
//     if (vars->pipes->heredoc_fd > 2)
//     {
//         close(vars->pipes->heredoc_fd);
//         vars->pipes->heredoc_fd = -1;
//     }
//     if (vars->pipes->redirection_fd > 2)
//     {
//         close(vars->pipes->redirection_fd);
//         vars->pipes->redirection_fd = -1;
//     }
//     vars->pipes->out_mode = OUT_MODE_NONE;
//     vars->pipes->current_redirect = NULL;
// }
/*
Resets saved standard file descriptors.
- Restores original stdin and stdout if they were changed.
- Closes any open heredoc file descriptor.
- Updates the pipes state in vars.
Works with execute_cmd() to clean up after command execution.
*/
void reset_redirect_fds(t_vars *vars)
{
    if (!vars || !vars->pipes)
        return;
    if (vars->pipes->saved_stdin > 2)
    {
        dup2(vars->pipes->saved_stdin, STDIN_FILENO);
        close(vars->pipes->saved_stdin);
        vars->pipes->saved_stdin = -1;
    }
    if (vars->pipes->saved_stdout > 2)
    {
        dup2(vars->pipes->saved_stdout, STDOUT_FILENO);
        close(vars->pipes->saved_stdout);
        vars->pipes->saved_stdout = -1;
    }
    if (vars->pipes->heredoc_fd > 2)
    {
        close(vars->pipes->heredoc_fd);
        vars->pipes->heredoc_fd = -1;
    }
    if (vars->pipes->redirection_fd > 2)
    {
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
    }
    vars->pipes->out_mode = OUT_MODE_NONE;
    vars->pipes->current_redirect = NULL;
    vars->pipes->last_heredoc = NULL;
    vars->pipes->last_in_redir = NULL;
    vars->pipes->last_out_redir = NULL;
    vars->pipes->cmd_redir = NULL;
}


// /*
// Handles input redirection (< filename).
// - Validates the redirection node and args.
// - Checks read permissions on target file.
// - Opens file in read-only mode.
// - Sets fd_in to the opened file descriptor.
// Returns:
// - 1 on success.
// - 0 on failure (invalid args, permissions, open error).
// Works with handle_redirect_cmd() for input setup.

// Example: For "cmd < input.txt"
// - Opens "input.txt" for reading
// - Sets fd_in to the file descriptor
// - Returns success/failure
// */
// int input_redirect(t_node *node, int *fd_in, t_vars *vars)
// {
//     char *file;
//     char cwd[PATH_MAX];
    
//     if (!node || !node->args || !node->args[0])
//         return (0);
//     if (getcwd(cwd, sizeof(cwd)) != NULL)
// 	{
//         fprintf(stderr, "DEBUG: Current working directory: %s\n", cwd);
//     }
//     file = node->args[0];
//     fprintf(stderr, "DEBUG: Before quote processing: '%s'\n", file);
//     process_arg_quotes(&node->args[0]);
//     file = node->args[0];
//     fprintf(stderr, "DEBUG: After quote processing: '%s'\n", file);
//     fprintf(stderr, "DEBUG: Testing access to file: '%s'\n", file);
//     if (access(file, F_OK) == -1)
//     {
//         not_found_error(file);
//         vars->error_code = 1;
//         return (0);
//     }
//     *fd_in = open(file, O_RDONLY);
//     if (*fd_in == -1)
//     {
//         fprintf(stderr, "DEBUG: Open failed, errno: %d (%s)\n", errno, strerror(errno));
//         ft_putstr_fd("bleshell: ", 2);
//         ft_putstr_fd(file, 2);
//         ft_putendl_fd(": Cannot open file", 2);
//         vars->error_code = 1;
//         return (0);
//     }
//     fprintf(stderr, "DEBUG: Successfully opened fd: %d\n", *fd_in);
//     if (dup2(*fd_in, STDIN_FILENO) == -1)
//     {
//         close(*fd_in);
//         vars->error_code = 1;
//         return (0);
//     }
//     return (1);
// }

// /*
// Handles output redirection (> or >> filename).
// - Validates the redirection node and args.
// - Sets flags based on append mode.
// - Checks write permissions on target file.
// - Opens file with appropriate flags and permissions.
// Returns:
// - 1 on success.
// - 0 on failure (invalid args, permissions, open error).
// Works with handle_redirect_cmd() for output setup.

// Example: For "cmd > output.txt"
// - Opens "output.txt" for writing with truncate flag
// - Sets fd_out to the file descriptor
// - Returns success/failure

// int	output_redirect(t_node *node, int *fd_out, int append, t_vars *vars)
// {
// 	int	flags;

// 	if (!node || !node->args || !node->args[0] || !fd_out)
// 		return (0);
// 	flags = set_output_flags(append);
// 	if (!chk_permissions(node->args[0], flags, vars))
// 		return (0);
// 	*fd_out = open(node->args[0], flags, 0644);
// 	if (*fd_out == -1)
// 		return (redirect_error(node->args[0], vars, 1));
// 	return (1);
// }
// int output_redirect(t_node *node, int *fd_out, int append, t_vars *vars)
// {
//     char *file;
//     int flags;
    
//     if (!node || !node->args || !node->args[0] || !fd_out)
//         return (0);
    
//     // Process quotes before attempting to access file
//     file = node->args[0];
//     fprintf(stderr, "DEBUG: Before quote processing: '%s'\n", file);
//     process_arg_quotes(&node->args[0]);
//     file = node->args[0];
//     fprintf(stderr, "DEBUG: After quote processing: '%s'\n", file);
    
//     // Set flags based on append mode - ALWAYS include O_CREAT
//     flags = O_WRONLY | O_CREAT;
//     if (append)
//         flags |= O_APPEND;
//     else
//         flags |= O_TRUNC;
    
//     // Open the file - note permissions 0644 for file creation
//     *fd_out = open(file, flags, 0644);
//     if (*fd_out == -1)
//     {
//         fprintf(stderr, "DEBUG: Open failed, errno: %d (%s)\n", errno, strerror(errno));
//         ft_putstr_fd("bleshell: ", 2);
//         ft_putstr_fd(file, 2);
//         ft_putendl_fd(": Permission denied", 2);
//         vars->error_code = 1;
//         return (0);
//     }
    
//     // Redirect stdout to the file
//     if (dup2(*fd_out, STDOUT_FILENO) == -1)
//     {
//         close(*fd_out);
//         vars->error_code = 1;
//         return (0);
//     }
    
//     return (1);
// }

// /*
// Opens file for redirection with appropriate mode.
// - Sets flags based on redirection type.
// - Validates file permissions.
// - Opens file with correct flags.
// - Reports errors if file operations fail.
// Returns:
// - 1 on success.
// - 0 on failure.
// Works with handle_redirect() for file opening.

// Example: Opening a file for redirection
// - Sets correct flags based on redirection type
// - Verifies permissions
// - Opens file and captures descriptor
// - Reports errors if necessary
// */
// int open_redirect_file(t_node *node, int *fd, int mode, t_vars *vars)
// {
//     int flags;

//     if (!node || !node->args || !node->args[0] || !fd)
//         return (0);
//     flags = set_redirect_flags(mode);
    
//     if (!chk_permissions(node->args[0], flags, vars))
//         return (0);
//     // Debug print before opening file
//     fprintf(stderr, "DEBUG: Opening redirect file: '%s' with flags: %d\n", 
//             node->args[0], flags);
//     if (mode == 0)
//         *fd = open(node->args[0], flags);
//     else
//         *fd = open(node->args[0], flags, 0644);
//     if (*fd == -1)
//         return (redirect_error(node->args[0], vars, 1));
//     return (1);
// }

// /*
// Main redirection controller function.
// - Opens redirection file.
// - Determines which standard descriptor to redirect.
// - Redirects standard descriptor to opened file.
// - Cleans up original descriptor to prevent leaks.
// Returns:
// - 1 on success.
// - 0 on any failure.
// Works with execute_redirects() for I/O redirection.

// Example: For "cmd > output.txt"
// - Opens output.txt
// - Redirects stdout to this file
// - Closes original file descriptor
// - Returns success status
// */
// int	handle_redirect(t_node *node, int *fd, int mode, t_vars *vars)
// {
// 	int	std_fd;
// 	int	result;
// 	int	success;

// 	success = open_redirect_file(node, fd, mode, vars);
// 	if (!success)
// 		return (0);
// 	if (mode == 0)
// 		std_fd = STDIN_FILENO;
// 	else
// 		std_fd = STDOUT_FILENO;
// 	result = dup2(*fd, std_fd);
// 	if (result == -1)
// 	{
// 		close(*fd);
// 		return (0);
// 	}
// 	close(*fd);
// 	return (1);
// }

// /**
//  * Sets up all redirections for a specific command
//  * Finds all redirections linked to this command and processes them
//  * Returns 1 on success, 0 on failure
//  */
// int setup_cmd_redirects(t_node *cmd_node, t_vars *vars)
// {
//     t_node *redirect_node;
//     // int fd = -1;
    
//     // Find all redirections linked to this command
//     redirect_node = find_linked_redirects(cmd_node, vars);
    
//     if (!redirect_node)
//         return (1);  // No redirections, nothing to do
    
//     // Save original file descriptors
//     if (vars->pipes->saved_stdin == -1)
//         vars->pipes->saved_stdin = dup(STDIN_FILENO);
//     if (vars->pipes->saved_stdout == -1)
//         vars->pipes->saved_stdout = dup(STDOUT_FILENO);
    
//     // Process the redirection
//     if (!setup_redirection(redirect_node, vars))
//     {
//         // Error occurred during redirection setup
//         reset_redirect_fds(vars);
//         return (0);
//     }
    
//     return (1);
// }

// /**
//  * Find redirections linked to a specific command
//  * Scans both forward and backward from command node to find related redirections
//  * Returns the redirection node if found, NULL otherwise
//  */
// t_node *find_linked_redirects(t_node *cmd_node, t_vars *vars)
// {
//     t_node *current;
//     t_node *cmd_prev;
    
//     if (!cmd_node || !vars || !vars->head)
//         return (NULL);
    
//     // First find the command in the token list
//     current = vars->head;
//     cmd_prev = NULL;
    
//     while (current && current != cmd_node)
//     {
//         cmd_prev = current;
//         current = current->next;
//     }
//     if (!current) // Command not found in token list
//         return (NULL);
    
//     // Start from command and scan forward for redirections
//     // until we hit another command or the end
//     current = cmd_node->next;
//     while (current && current->type != TYPE_CMD && current->type != TYPE_PIPE)
//     {
//         if (is_redirection(current->type))
//             return (current);
//         current = current->next;
//     }
    
//     // Start from before command and scan backward for redirections
//     // until we hit another command or the beginning
//     current = cmd_prev;
//     while (current && current->type != TYPE_CMD && current->type != TYPE_PIPE)
//     {
//         if (is_redirection(current->type))
//             return (current);
//         current = current->prev;
//     }
    
//     return (NULL);
// }

// /**
//  * Handle multiple redirections for a command
//  * Last redirection of each type (input/output) takes precedence
//  * Returns 1 on success, 0 on failure
//  */
// int setup_multi_redirects(t_node *cmd_node, t_vars *vars)
// {
//     t_node *current;
//     t_node *last_in_redir = NULL;
//     t_node *last_out_redir = NULL;
//     int fd = -1;
    
//     if (!cmd_node || !vars || !vars->head)
//         return (1);
    
//     // Store the current command in AST
//     vars->pipes->last_cmd = cmd_node;
//     vars->pipes->cmd_redir = NULL;
    
//     // Scan for redirections related to this command
//     current = vars->head;
//     while (current)
//     {
//         if (current->type == TYPE_IN_REDIRECT && 
//             is_related_to_cmd(current, cmd_node, vars))
//         {
//             last_in_redir = current;
//         }
//         else if ((current->type == TYPE_OUT_REDIRECT || 
//                 current->type == TYPE_APPEND_REDIRECT) && 
//                 is_related_to_cmd(current, cmd_node, vars))
//         {
//             last_out_redir = current;
//         }
//         else if (current->type == TYPE_HEREDOC && 
//                 is_related_to_cmd(current, cmd_node, vars))
//         {
//             vars->pipes->last_heredoc = current;
//         }
        
//         current = current->next;
//     }
    
//     // Save original file descriptors if we have any redirections
//     if (last_in_redir || last_out_redir || vars->pipes->last_heredoc)
//     {
//         if (vars->pipes->saved_stdin == -1)
//             vars->pipes->saved_stdin = dup(STDIN_FILENO);
//         if (vars->pipes->saved_stdout == -1)
//             vars->pipes->saved_stdout = dup(STDOUT_FILENO);
//     }
    
//     // Process redirections in order: heredoc first, input second, output last
//     if (vars->pipes->last_heredoc)
//     {
//         vars->pipes->cmd_redir = vars->pipes->last_heredoc;
//         if (!setup_redirection(vars->pipes->last_heredoc, vars, &fd))
//             return (0);
//     }
//     else if (last_in_redir)
//     {
//         vars->pipes->cmd_redir = last_in_redir;
//         if (!setup_redirection(last_in_redir, vars, &fd))
//             return (0);
//     }
    
//     if (last_out_redir)
//     {
//         vars->pipes->cmd_redir = last_out_redir;
//         if (!setup_redirection(last_out_redir, vars, &fd))
//             return (0);
//     }
    
//     return (1);
// }
// int	setup_multi_redirects(t_node *cmd_node, t_vars *vars)
// {
//     t_node	*current;
//     // int		fd;
    
// 	// fd = -1;
//     if (!cmd_node || !vars || !vars->head || !vars->pipes)
//         return (1);
    
//     /* Store the current command in pipes structure */
//     vars->pipes->last_cmd = cmd_node;
//     vars->pipes->cmd_redir = NULL;
    
//     /* Reset redirection tracking */
//     vars->pipes->last_in_redir = NULL;
//     vars->pipes->last_out_redir = NULL;
//     vars->pipes->last_heredoc = NULL;
    
//     /* Scan for redirections related to this command */
//     current = vars->head;
//     while (current)
//     {
//         if (current->type == TYPE_IN_REDIRECT && 
//             is_related_to_cmd(current, cmd_node, vars))
//         {
//             vars->pipes->last_in_redir = current;
//         }
//         else if ((current->type == TYPE_OUT_REDIRECT || 
//                 current->type == TYPE_APPEND_REDIRECT) && 
//                 is_related_to_cmd(current, cmd_node, vars))
//         {
//             vars->pipes->last_out_redir = current;
//         }
//         else if (current->type == TYPE_HEREDOC && 
//                 is_related_to_cmd(current, cmd_node, vars))
//         {
//             vars->pipes->last_heredoc = current;
//         }
        
//         current = current->next;
//     }
    
//     /* Save original file descriptors if we have any redirections */
//     if (vars->pipes->last_in_redir || vars->pipes->last_out_redir || vars->pipes->last_heredoc)
//     {
//         if (vars->pipes->saved_stdin == -1)
//             vars->pipes->saved_stdin = dup(STDIN_FILENO);
//         if (vars->pipes->saved_stdout == -1)
//             vars->pipes->saved_stdout = dup(STDOUT_FILENO);
//     }
    
//     /* Process redirections in order: heredoc first, input second, output last */
//     if (vars->pipes->last_heredoc)
//     {
//         vars->pipes->cmd_redir = vars->pipes->last_heredoc;
//         if (!setup_redirection(vars->pipes->last_heredoc, vars))
//             return (0);
//     }
//     else if (vars->pipes->last_in_redir)
//     {
//         vars->pipes->cmd_redir = vars->pipes->last_in_redir;
//         if (!setup_redirection(vars->pipes->last_in_redir, vars))
//             return (0);
//     }
    
//     if (vars->pipes->last_out_redir)
//     {
//         vars->pipes->cmd_redir = vars->pipes->last_out_redir;
//         if (!setup_redirection(vars->pipes->last_out_redir, vars))
//             return (0);
//     }
    
//     return (1);
// }
