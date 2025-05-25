/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirects.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:39:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/25 21:08:11 by bleow            ###   ########.fr       */
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
int setup_in_redir(t_node *node, t_vars *vars)
{
    char *file;
    struct stat file_stat;
    
    fprintf(stderr, "DEBUG-INREDIR: ===== INPUT REDIRECTION START =====\n");
    
    if (!node || !node->args || !node->args[0]) {
        fprintf(stderr, "DEBUG-INREDIR: Invalid node or missing filename\n");
        fprintf(stderr, "DEBUG-INREDIR: ===== INPUT REDIRECTION END (FAILED) =====\n");
        return (0);
    }
    
    file = node->args[0];
    fprintf(stderr, "DEBUG-INREDIR: File: '%s'\n", file);
    
    // Debug initial state
    fprintf(stderr, "DEBUG-INREDIR: FDs before: stdin=%d, redirection_fd=%d\n",
            fileno(stdin), vars->pipes->redirection_fd);
    
    // Close any previously opened redirection file descriptor
    if (vars->pipes->redirection_fd >= 0)
    {
        fprintf(stderr, "DEBUG-INREDIR: Closing previous fd %d\n", 
                vars->pipes->redirection_fd);
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
    }
    
    // Check file accessibility
    fprintf(stderr, "DEBUG-INREDIR: Checking file access for '%s'\n", file);
    if (!check_input_file_access(file, &file_stat, vars))
    {
        fprintf(stderr, "DEBUG-INREDIR: File access check failed\n");
        fprintf(stderr, "DEBUG-INREDIR: ===== INPUT REDIRECTION END (FAILED) =====\n");
        return (0);
    }
    
    // Set up the redirection
    int result = setup_input_redirection(file, vars);
    
    // Debug final state
    fprintf(stderr, "DEBUG-INREDIR: FDs after: stdin=%d, redirection_fd=%d\n",
            fileno(stdin), vars->pipes->redirection_fd);
    
    if (result)
        fprintf(stderr, "DEBUG-INREDIR: ===== INPUT REDIRECTION END (SUCCESS) =====\n");
    else
        fprintf(stderr, "DEBUG-INREDIR: ===== INPUT REDIRECTION END (FAILED) =====\n");
    
    return result;
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
int setup_input_redirection(char *file, t_vars *vars)
{
    fprintf(stderr, "DEBUG-SETUP-INFILE: ===== SETUP INPUT FILE START =====\n");
    fprintf(stderr, "DEBUG-SETUP-INFILE: File: '%s'\n", file);
    
    // Debug initial state
    fprintf(stderr, "DEBUG-SETUP-INFILE: Initial FDs: stdin=%d, redirection_fd=%d\n",
            fileno(stdin), vars->pipes->redirection_fd);
    
    // Close any previous file descriptor
    if (vars->pipes->redirection_fd >= 0)
    {
        fprintf(stderr, "DEBUG-SETUP-INFILE: Closing previous fd %d\n", 
                vars->pipes->redirection_fd);
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
    }
    
    // Open the file
    fprintf(stderr, "DEBUG-SETUP-INFILE: Opening file '%s'\n", file);
    vars->pipes->redirection_fd = open(file, O_RDONLY);
    if (vars->pipes->redirection_fd == -1)
    {
        fprintf(stderr, "DEBUG-SETUP-INFILE: Failed to open file: %s\n", strerror(errno));
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-SETUP-INFILE: ===== SETUP INPUT FILE END (FAILED) =====\n");
        return (0);
    }
    
    fprintf(stderr, "DEBUG-SETUP-INFILE: Opened file with fd=%d\n", 
            vars->pipes->redirection_fd);
    
    // Redirect stdin to the file
    fprintf(stderr, "DEBUG-SETUP-INFILE: Redirecting stdin to fd %d\n", 
            vars->pipes->redirection_fd);
    if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG-SETUP-INFILE: dup2 failed: %s\n", strerror(errno));
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-SETUP-INFILE: ===== SETUP INPUT FILE END (FAILED) =====\n");
        return (0);
    }
    
    // Debug final state
    fprintf(stderr, "DEBUG-SETUP-INFILE: Final FDs: stdin=%d, redirection_fd=%d\n",
            fileno(stdin), vars->pipes->redirection_fd);
    
    fprintf(stderr, "DEBUG-SETUP-INFILE: ===== SETUP INPUT FILE END (SUCCESS) =====\n");
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
int setup_out_redir(t_node *node, t_vars *vars)
{
    char *file;
    int mode;
    
    fprintf(stderr, "DEBUG-OUTREDIR: ===== OUTPUT REDIRECTION START =====\n");
    
    if (!node || !node->args || !node->args[0]) {
        fprintf(stderr, "DEBUG-OUTREDIR: Invalid node or missing filename\n");
        fprintf(stderr, "DEBUG-OUTREDIR: ===== OUTPUT REDIRECTION END (FAILED) =====\n");
        return (0);
    }
    
    file = node->args[0];
    fprintf(stderr, "DEBUG-OUTREDIR: File: '%s'\n", file);
    
    // Debug initial state
    fprintf(stderr, "DEBUG-OUTREDIR: FDs before: stdout=%d, redirection_fd=%d, out_mode=%d\n",
            fileno(stdout), vars->pipes->redirection_fd, vars->pipes->out_mode);
    
    // Set mode based on redirection type
    mode = O_WRONLY | O_CREAT | O_TRUNC;
    vars->pipes->out_mode = OUT_MODE_TRUNCATE;
    
    if (node->type == TYPE_APPEND_REDIRECT) {
        mode = O_WRONLY | O_CREAT | O_APPEND;
        vars->pipes->out_mode = OUT_MODE_APPEND;
        fprintf(stderr, "DEBUG-OUTREDIR: Using append mode\n");
    } else {
        fprintf(stderr, "DEBUG-OUTREDIR: Using truncate mode\n");
    }
    
    // Close any previously opened redirection file descriptor
    if (vars->pipes->redirection_fd >= 0)
    {
        fprintf(stderr, "DEBUG-OUTREDIR: Closing previous fd %d\n", 
                vars->pipes->redirection_fd);
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
    }
    
    // Check permissions
    fprintf(stderr, "DEBUG-OUTREDIR: Checking permissions for '%s'\n", file);
    if (!chk_permissions(file, mode, vars))
    {
        fprintf(stderr, "DEBUG-OUTREDIR: Permission check failed\n");
        fprintf(stderr, "DEBUG-OUTREDIR: ===== OUTPUT REDIRECTION END (FAILED) =====\n");
        return (0);
    }
    
    // Set up the redirection
    int result = setup_output_redirection(file, vars);
    
    // Debug final state
    fprintf(stderr, "DEBUG-OUTREDIR: FDs after: stdout=%d, redirection_fd=%d, out_mode=%d\n",
            fileno(stdout), vars->pipes->redirection_fd, vars->pipes->out_mode);
    
    if (result)
        fprintf(stderr, "DEBUG-OUTREDIR: ===== OUTPUT REDIRECTION END (SUCCESS) =====\n");
    else
        fprintf(stderr, "DEBUG-OUTREDIR: ===== OUTPUT REDIRECTION END (FAILED) =====\n");
    
    return result;
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
int setup_output_redirection(char *file, t_vars *vars)
{
    int mode;
    
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: ===== SETUP OUTPUT FILE START =====\n");
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: File: '%s'\n", file);
    
    // Debug initial state
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: Initial FDs: stdout=%d, redirection_fd=%d\n",
            fileno(stdout), vars->pipes->redirection_fd);
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: Output mode: %d\n", vars->pipes->out_mode);
    
    // Close any previous file descriptor
    if (vars->pipes->redirection_fd >= 0)
    {
        fprintf(stderr, "DEBUG-SETUP-OUTFILE: Closing previous fd %d\n", 
                vars->pipes->redirection_fd);
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
    }
    
    // Set the file open mode
    if (vars->pipes->out_mode == OUT_MODE_APPEND)
    {
        mode = O_WRONLY | O_CREAT | O_APPEND;
        fprintf(stderr, "DEBUG-SETUP-OUTFILE: Using append mode\n");
    }
    else
    {
        mode = O_WRONLY | O_CREAT | O_TRUNC;
        fprintf(stderr, "DEBUG-SETUP-OUTFILE: Using truncate mode\n");
    }
    
    // Open the file
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: Opening file '%s' with mode %d\n", file, mode);
    vars->pipes->redirection_fd = open(file, mode, 0666);
    if (vars->pipes->redirection_fd == -1)
    {
        fprintf(stderr, "DEBUG-SETUP-OUTFILE: Failed to open file: %s\n", strerror(errno));
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-SETUP-OUTFILE: ===== SETUP OUTPUT FILE END (FAILED) =====\n");
        return (0);
    }
    
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: Opened file with fd=%d\n", 
            vars->pipes->redirection_fd);
    
    // Redirect stdout to the file
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: Redirecting stdout to fd %d\n", 
            vars->pipes->redirection_fd);
    if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG-SETUP-OUTFILE: dup2 failed: %s\n", strerror(errno));
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-SETUP-OUTFILE: ===== SETUP OUTPUT FILE END (FAILED) =====\n");
        return (0);
    }
    
    // Debug final state
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: Final FDs: stdout=%d, redirection_fd=%d\n",
            fileno(stdout), vars->pipes->redirection_fd);
    
    fprintf(stderr, "DEBUG-SETUP-OUTFILE: ===== SETUP OUTPUT FILE END (SUCCESS) =====\n");
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
    if (!vars->pipes->heredoc_delim && node->right && node->right->args && node->right->args[0])
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
int check_input_file_access(char *file, struct stat *file_stat, t_vars *vars)
{
    fprintf(stderr, "DEBUG-FILE-ACCESS: Checking access for file: '%s'\n", file);
    
    // Check if file exists
    if (access(file, F_OK) != 0)
    {
        // Only log on failure
        fprintf(stderr, "DEBUG-FILE-ACCESS: File '%s' not found\n", file);
        not_found_error(file, vars);
        vars->error_code = ERR_REDIRECTION;
        
        return handle_missing_input(vars);
    }
    
    // Check if file is a directory
    if (stat(file, file_stat) == 0 && S_ISDIR(file_stat->st_mode))
    {
        // Only log on failure
        fprintf(stderr, "DEBUG-FILE-ACCESS: '%s' is a directory\n", file);
        shell_error(file, ERR_ISDIRECTORY, vars);
        vars->error_code = ERR_REDIRECTION;
        
        return handle_missing_input(vars);
    }
    
    // Check permissions
    if (!chk_permissions(file, O_RDONLY, vars))
    {
        // Only log on failure
        fprintf(stderr, "DEBUG-FILE-ACCESS: No permission for '%s'\n", file);
        vars->error_code = ERR_REDIRECTION;
        
        return handle_missing_input(vars);
    }
    
    fprintf(stderr, "DEBUG-FILE-ACCESS: File '%s' exists and is accessible\n", file);
    return (1);
}

/*
Handles case when input file is missing or inaccessible.
For pipe contexts, redirects stdin from /dev/null.
*/
int handle_missing_input(t_vars *vars)
{
    int null_fd;
    
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
int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
{
    t_node *current_node;
    t_node *next_redir;
    int redir_status = 1;
    
    fprintf(stderr, "DEBUG-REDIR-CHAIN: ===== REDIRECTION CHAIN START =====\n");
    fprintf(stderr, "DEBUG-REDIR-CHAIN: For command: %s (pid=%d)\n", 
            cmd_node->args ? cmd_node->args[0] : "NULL", getpid());
    
    if (start_node && start_node->args)
        fprintf(stderr, "DEBUG-REDIR-CHAIN: First redirection: %s [%s]\n", 
                get_token_str(start_node->type), start_node->args[0]);
    
    // Dump FD state before redirection
    fprintf(stderr, "DEBUG-REDIR-CHAIN: FDs before chain: stdin=%d, stdout=%d, stderr=%d\n",
            fileno(stdin), fileno(stdout), fileno(stderr));
    
    current_node = start_node;
    
    while (current_node && is_redirection(current_node->type))
    {
        // Print current redirection details
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing %s redirection for file: '%s'\n", 
                get_token_str(current_node->type),
                current_node->args ? current_node->args[0] : "NULL");
        
        // Debug: Current state before this redirection
        if (vars->pipes) {
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Current redirection_fd=%d, out_mode=%d\n", 
                    vars->pipes->redirection_fd, vars->pipes->out_mode);
        }
        
        // Setup this redirection
        if (!setup_redirection(current_node, vars))
        {
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection failed for file '%s'\n", 
                    current_node->args ? current_node->args[0] : "NULL");
            fprintf(stderr, "DEBUG-REDIR-CHAIN: FD state after failure: stdin=%d, stdout=%d\n",
                    fileno(stdin), fileno(stdout));
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Error code=%d\n", vars->error_code);
            
            redir_status = 0;
            break;
        }
        
        // Find next redirection in the chain
        next_redir = current_node->redir;
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Next redirection in chain: %p\n", 
                (void*)next_redir);
        if (next_redir)
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Next is %s [%s]\n", 
                    get_token_str(next_redir->type), 
                    next_redir->args ? next_redir->args[0] : "NULL");
        
        if (!next_redir)
            break;
            
        current_node = next_redir;
    }
    
    // Dump FD state after redirection chain
    fprintf(stderr, "DEBUG-REDIR-CHAIN: FDs after chain: stdin=%d, stdout=%d, stderr=%d\n",
            fileno(stdin), fileno(stdout), fileno(stderr));
    
    if (redir_status)
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain completed successfully\n");
    else
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain failed\n");
    
    fprintf(stderr, "DEBUG-REDIR-CHAIN: ===== REDIRECTION CHAIN END =====\n");
    
    return redir_status;
}
