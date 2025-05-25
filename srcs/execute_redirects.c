/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirects.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:39:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/25 18:04:12 by bleow            ###   ########.fr       */
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
//     char    *file;
//     struct stat file_stat;
//     int     null_fd;

//     if (!node->right || !node->right->args || !node->right->args[0])
//         return (0);
    
//     file = node->right->args[0];
    
//     // Check if file exists first
//     if (access(file, F_OK) != 0)
//     {
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
        
//         // If in pipe context, redirect stdin to /dev/null to prevent hanging
//         if (vars->pipes && vars->pipes->pipe_root)
//         {
//             null_fd = open("/dev/null", O_RDONLY);
//             if (null_fd != -1)
//             {
//                 dup2(null_fd, STDIN_FILENO);
//                 close(null_fd);
//             }
//         }
        
//         return (0);
//     }
    
//     // Check if file is a directory
//     if (stat(file, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
//     {
//         shell_error(file, ERR_ISDIRECTORY, vars);
//         vars->error_code = ERR_REDIRECTION;
        
//         // If in pipe context, redirect stdin to /dev/null to prevent hanging
//         if (vars->pipes && vars->pipes->pipe_root)
//         {
//             null_fd = open("/dev/null", O_RDONLY);
//             if (null_fd != -1)
//             {
//                 dup2(null_fd, STDIN_FILENO);
//                 close(null_fd);
//             }
//         }
        
//         return (0);
//     }
    
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         vars->error_code = ERR_REDIRECTION; 
        
//         // If in pipe context, redirect stdin to /dev/null to prevent hanging
//         if (vars->pipes && vars->pipes->pipe_root)
//         {
//             null_fd = open("/dev/null", O_RDONLY);
//             if (null_fd != -1)
//             {
//                 dup2(null_fd, STDIN_FILENO);
//                 close(null_fd);
//             }
//         }
        
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
        
//         // Keep this call for file descriptor cleanup
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         close(vars->pipes->redirection_fd);
//         return (0);
//     }
    
//     return (1);
// }
// int setup_in_redir(t_node *node, t_vars *vars)
// {
//     char    *file;
//     struct stat file_stat;
//     int     null_fd;

//     fprintf(stderr, "DEBUG: setup_in_redir for file: %s\n", 
//             node->right && node->right->args ? node->right->args[0] : "NULL");
    
//     if (!node->right || !node->right->args || !node->right->args[0])
//         return (0);
    
//     file = node->right->args[0];
    
//     // Check if file exists first
//     if (access(file, F_OK) != 0)
//     {
//         fprintf(stderr, "DEBUG: Input file %s does not exist\n", file);
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
        
//         // If in pipe context, redirect stdin to /dev/null to prevent hanging
//         if (vars->pipes && vars->pipes->pipe_root)
//         {
//             fprintf(stderr, "DEBUG: In pipe context, redirecting stdin to /dev/null\n");
//             null_fd = open("/dev/null", O_RDONLY);
//             if (null_fd != -1)
//             {
//                 dup2(null_fd, STDIN_FILENO);
//                 close(null_fd);
//             }
//         }
        
//         return (0);
//     }
    
//     // Check if file is a directory
//     if (stat(file, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
//     {
//         fprintf(stderr, "DEBUG: Input file %s is a directory\n", file);
//         shell_error(file, ERR_ISDIRECTORY, vars);
//         vars->error_code = ERR_REDIRECTION;
        
//         // If in pipe context, redirect stdin to /dev/null to prevent hanging
//         if (vars->pipes && vars->pipes->pipe_root)
//         {
//             fprintf(stderr, "DEBUG: In pipe context, redirecting stdin to /dev/null\n");
//             null_fd = open("/dev/null", O_RDONLY);
//             if (null_fd != -1)
//             {
//                 dup2(null_fd, STDIN_FILENO);
//                 close(null_fd);
//             }
//         }
        
//         return (0);
//     }
    
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         fprintf(stderr, "DEBUG: No permission to read file %s\n", file);
//         vars->error_code = ERR_REDIRECTION;
        
//         // If in pipe context, redirect stdin to /dev/null to prevent hanging
//         if (vars->pipes && vars->pipes->pipe_root)
//         {
//             fprintf(stderr, "DEBUG: In pipe context, redirecting stdin to /dev/null\n");
//             null_fd = open("/dev/null", O_RDONLY);
//             if (null_fd != -1)
//             {
//                 dup2(null_fd, STDIN_FILENO);
//                 close(null_fd);
//             }
//         }
        
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG: Failed to open input file %s\n", file);
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
        
//         // Keep this call for file descriptor cleanup
//         fprintf(stderr, "DEBUG: Calling end_pipe_processes for cleanup\n");
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG: Successfully redirected stdin from %s (fd=%d)\n", 
//             file, vars->pipes->redirection_fd);
    
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG: dup2 failed for stdin redirection\n");
//         close(vars->pipes->redirection_fd);
//         vars->pipes->redirection_fd = -1;
//         return (0);
//     }
    
//     return (1);
// }
int setup_in_redir(t_node *node, t_vars *vars)
{
    char    *file;
    struct stat file_stat;
    
    fprintf(stderr, "DEBUG-INREDIR: Setting up input redirection for file: '%s'\n", 
            node->args ? node->args[0] : "NULL");
    
    if (!node->args || !node->args[0])
        return (0);
    
    file = node->args[0];
    
    // Close any previously opened redirection file descriptor
    if (vars->pipes->redirection_fd >= 0)
    {
        fprintf(stderr, "DEBUG-INREDIR: Closing previous fd %d\n", 
                vars->pipes->redirection_fd);
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
    }
    
    // Check file existence and type
    if (!check_input_file_access(file, &file_stat, vars))
        return (0);
    
    // Open the file and set up redirection
    return setup_input_redirection(file, vars);
}

/*
Opens input file and sets up stdin redirection.
*/
int setup_input_redirection(char *file, t_vars *vars)
{
    vars->pipes->redirection_fd = open(file, O_RDONLY);
    if (vars->pipes->redirection_fd == -1)
    {
        fprintf(stderr, "DEBUG: Failed to open input file %s\n", file);
        not_found_error(file, vars);
        vars->error_code = ERR_REDIRECTION;
        
        // Keep this call for file descriptor cleanup
        fprintf(stderr, "DEBUG: Calling end_pipe_processes for cleanup\n");
        end_pipe_processes(vars);
        return (0);
    }
    
    fprintf(stderr, "DEBUG: Successfully opened %s (fd=%d)\n", 
            file, vars->pipes->redirection_fd);
    
    // Reset STDIN_FILENO to its original state first
    if (dup2(STDIN_FILENO, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG: Failed to reset stdin before redirection\n");
    }
    
    if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG: dup2 failed for stdin redirection\n");
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
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
// int	setup_out_redir(t_node *node, t_vars *vars)
// {
//     char	*file;
//     int		flags;

//     if (!node->right || !node->right->args || !node->right->args[0])
//         return (0);
    
//     file = node->right->args[0];
    
//     // Set flags based on redirection mode (truncate or append)
//     if (vars->pipes->out_mode == OUT_MODE_APPEND)
//         flags = O_WRONLY | O_CREAT | O_APPEND;
//     else
//         flags = O_WRONLY | O_CREAT | O_TRUNC;
    
//     if (!chk_permissions(file, flags, vars))
//     {
//         vars->error_code = ERR_REDIRECTION;
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, flags, 0644);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         shell_error(file, ERR_PERMISSIONS, vars);
//         vars->error_code = ERR_REDIRECTION;
//         return (0);
//     }
    
//     if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
//     {
//         close(vars->pipes->redirection_fd);
//         return (0);
//     }
    
//     return (1);
// }
// int setup_out_redir(t_node *node, t_vars *vars)
// {
//     char    *file;
//     int     flags;

//     fprintf(stderr, "DEBUG: setup_out_redir for file: %s (mode: %s)\n", 
//             node->right && node->right->args ? node->right->args[0] : "NULL",
//             vars->pipes->out_mode == OUT_MODE_APPEND ? "append" : "truncate");
    
//     if (!node->right || !node->right->args || !node->right->args[0])
//         return (0);
    
//     file = node->right->args[0];
    
//     // Set flags based on redirection mode (truncate or append)
//     if (vars->pipes->out_mode == OUT_MODE_APPEND)
//         flags = O_WRONLY | O_CREAT | O_APPEND;
//     else
//         flags = O_WRONLY | O_CREAT | O_TRUNC;
    
//     if (!chk_permissions(file, flags, vars))
//     {
//         fprintf(stderr, "DEBUG: No permission to write to file %s\n", file);
//         vars->error_code = ERR_REDIRECTION;
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, flags, 0644);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG: Failed to open output file %s\n", file);
//         shell_error(file, ERR_PERMISSIONS, vars);
//         vars->error_code = ERR_REDIRECTION;
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG: Successfully redirected stdout to %s (fd=%d)\n", 
//             file, vars->pipes->redirection_fd);
    
//     if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
//     {
//         close(vars->pipes->redirection_fd);
//         vars->pipes->redirection_fd = -1;
//         return (0);
//     }
    
//     return (1);
// }
int setup_out_redir(t_node *node, t_vars *vars)
{
    char    *file;
    
    fprintf(stderr, "DEBUG-OUTREDIR: Setting up output redirection for file: '%s' (mode: %s)\n", 
            node->args ? node->args[0] : "NULL",
            vars->pipes->out_mode == OUT_MODE_APPEND ? "append" : "truncate");
    
    if (!node->args || !node->args[0])
        return (0);
    
    file = node->args[0];
    
    // Close any previously opened redirection file descriptor
    if (vars->pipes->redirection_fd >= 0)
    {
        fprintf(stderr, "DEBUG-OUTREDIR: Closing previous fd %d (enforcing 'last wins')\n", 
                vars->pipes->redirection_fd);
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
    }
    
    // Set up output redirection
    return setup_output_redirection(file, vars);
}

/*
Opens output file and sets up stdout redirection.
*/
int setup_output_redirection(char *file, t_vars *vars)
{
    int flags;
    
    // Set flags based on redirection mode
    if (vars->pipes->out_mode == OUT_MODE_APPEND)
        flags = O_WRONLY | O_CREAT | O_APPEND;
    else
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    
    if (!chk_permissions(file, flags, vars))
    {
        fprintf(stderr, "DEBUG: Permission check failed for '%s'\n", file);
        vars->error_code = ERR_REDIRECTION;
        return (0);
    }
    
    vars->pipes->redirection_fd = open(file, flags, 0644);
    if (vars->pipes->redirection_fd == -1)
    {
        fprintf(stderr, "DEBUG: Failed to open file '%s'\n", file);
        shell_error(file, ERR_PERMISSIONS, vars);
        vars->error_code = ERR_REDIRECTION;
        return (0);
    }
    
    fprintf(stderr, "DEBUG: Opened file '%s' (fd=%d)\n", 
            file, vars->pipes->redirection_fd);
    
    // Reset stdout to its original state first
    if (dup2(STDOUT_FILENO, STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG: Failed to reset stdout before redirection\n");
    }
    
    if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG: dup2 failed\n");
        close(vars->pipes->redirection_fd);
        vars->pipes->redirection_fd = -1;
        return (0);
    }
    
    fprintf(stderr, "DEBUG: Successfully redirected stdout to '%s'\n", file);
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
int check_input_file_access(char *file, struct stat *file_stat, t_vars *vars)
{
    // Check if file exists
    if (access(file, F_OK) != 0)
    {
        fprintf(stderr, "DEBUG: Input file %s does not exist\n", file);
        not_found_error(file, vars);
        vars->error_code = ERR_REDIRECTION;
        
        return handle_missing_input(vars);
    }
    
    // Check if file is a directory
    if (stat(file, file_stat) == 0 && S_ISDIR(file_stat->st_mode))
    {
        fprintf(stderr, "DEBUG: Input file %s is a directory\n", file);
        shell_error(file, ERR_ISDIRECTORY, vars);
        vars->error_code = ERR_REDIRECTION;
        
        return handle_missing_input(vars);
    }
    
    // Check permissions
    if (!chk_permissions(file, O_RDONLY, vars))
    {
        fprintf(stderr, "DEBUG: No permission to read file %s\n", file);
        vars->error_code = ERR_REDIRECTION;
        
        return handle_missing_input(vars);
    }
    
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
        fprintf(stderr, "DEBUG: In pipe context, redirecting stdin to /dev/null\n");
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
//     int had_errors = 0;
//     // Determine if we're in a pipe context by directly checking pipe_root
//     int in_pipe_context = (vars->pipes && vars->pipes->pipe_root != NULL);
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         vars->pipes->current_redirect = current_node;
        
//         // Try setup but stop chain on failure (bash behavior)
//         if (!setup_redirection(current_node, vars))
//         {
//             had_errors = 1;
//             break; // Stop processing more redirections after first failure (bash behavior)
//         }
        
//         // Continue to next redirection only if this one succeeded
//         if (current_node->redir)
//         {
//             current_node = current_node->redir;
//         }
//         else
//         {
//             next_redir = get_next_redir(current_node, cmd_node);
//             if (next_redir)
//             {
//                 current_node = next_redir;
//             }
//             else
//             {
//                 break;
//             }
//         }
//     }
    
//     if (had_errors)
//     {
//         reset_redirect_fds(vars);
        
//         if (in_pipe_context)
//             return (1); // Continue in pipe context
//         else
//             return (0); // Abort in standalone context (bash-like behavior)
//     }
    
//     return (1);
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;  // Success unless we encounter an error
    
//     fprintf(stderr, "DEBUG: proc_redir_chain starting for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         // Process this redirection
//         fprintf(stderr, "DEBUG: Processing redirection type: %s\n", 
//                 get_token_str(current_node->type));
                
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG: Redirection failed for %s\n", 
//                     current_node->right && current_node->right->args ? 
//                     current_node->right->args[0] : "NULL");
//             redir_status = 0;
//             break;  // Stop processing redirections on first failure
//         }
        
//         // Find next redirection in the chain
//         next_redir = current_node->redir;
//         if (!next_redir)
//             break;
            
//         current_node = next_redir;
//     }
    
//     fprintf(stderr, "DEBUG: proc_redir_chain completed with status: %d (error_code=%d)\n", 
//             redir_status, vars->error_code);
    
//     return redir_status;
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;  // Success unless we encounter an error
    
//     fprintf(stderr, "DEBUG: proc_redir_chain starting for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         // Process this redirection
//         fprintf(stderr, "DEBUG: Processing redirection type: %s for file: %s\n", 
//                 get_token_str(current_node->type),
//                 current_node->right && current_node->right->args ? 
//                 current_node->right->args[0] : "NULL");
                
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG: Redirection failed for %s (error_code=%d)\n", 
//                     current_node->right && current_node->right->args ? 
//                     current_node->right->args[0] : "NULL",
//                     vars->error_code);
//             redir_status = 0;
//             break;  // Stop processing redirections on first failure
//         }
        
//         // Find next redirection in the chain
//         next_redir = current_node->redir;
//         if (!next_redir)
//             break;
            
//         current_node = next_redir;
//     }
    
//     fprintf(stderr, "DEBUG: proc_redir_chain completed with status: %d (error_code=%d)\n", 
//             redir_status, vars->error_code);
    
//     return redir_status;
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int redir_status = 1;
//     int in_count = 0, out_count = 0;
    
//     fprintf(stderr, "DEBUG-REDIR: Processing redirection chain for command: %s\n", 
//             cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         // Track redirection counts
//         if (current_node->type == TYPE_IN_REDIRECT || current_node->type == TYPE_HEREDOC)
//             in_count++;
//         else
//             out_count++;
            
//         fprintf(stderr, "DEBUG-REDIR: Processing %s redirection #%d for file: %s\n", 
//                 get_token_str(current_node->type),
//                 (current_node->type == TYPE_IN_REDIRECT || current_node->type == TYPE_HEREDOC) 
//                 ? in_count : out_count,
//                 current_node->args ? current_node->args[0] : "NULL");
        
//         // Update total count in pipes struct
//         vars->pipes->redir_count++;
        
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR: Redirection failed for %s (error_code=%d)\n", 
//                     current_node->args ? current_node->args[0] : "NULL",
//                     vars->error_code);
                    
//             redir_status = 0;
//             break;
//         }
        
//         next_redir = current_node->redir;
//         if (!next_redir)
//             break;
            
//         current_node = next_redir;
//     }
    
//     fprintf(stderr, "DEBUG-REDIR: Chain completed with status=%d, in_count=%d, out_count=%d\n", 
//             redir_status, in_count, out_count);
    
//     return redir_status;
// }
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
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup failed for %s\n", 
//                     current_node->args ? current_node->args[0] : "NULL");
//             redir_status = 0;
//             break;
//         }
        
//         next_redir = current_node->redir;
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Next redirection: %p\n", (void*)next_redir);
//         if (!next_redir)
//             break;
            
//         current_node = next_redir;
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain processing complete, status=%d\n", redir_status);
//     return redir_status;
// }
int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
{
    t_node *current_node;
    t_node *next_redir;
    int redir_status = 1;
    int in_count = 0, out_count = 0;
    
    fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain for command: %s\n", 
            cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
    current_node = start_node;
    while (current_node && is_redirection(current_node->type))
    {
        // Track redirection counts by type
        if (current_node->type == TYPE_IN_REDIRECT || current_node->type == TYPE_HEREDOC)
            in_count++;
        else
            out_count++;
            
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing %s redirection #%d for file: %s\n", 
                get_token_str(current_node->type),
                (current_node->type == TYPE_IN_REDIRECT || current_node->type == TYPE_HEREDOC) 
                ? in_count : out_count,
                current_node->args ? current_node->args[0] : "NULL");
        
        // Update total count in pipes struct
        vars->pipes->redir_count++;
        
        if (!setup_redirection(current_node, vars))
        {
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection failed for %s (error_code=%d)\n", 
                    current_node->args ? current_node->args[0] : "NULL",
                    vars->error_code);
                    
            redir_status = 0;
            break;
        }
        
        next_redir = current_node->redir;
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Next redirection: %p\n", (void*)next_redir);
        if (!next_redir)
            break;
            
        current_node = next_redir;
    }
    
    fprintf(stderr, "DEBUG-REDIR-CHAIN: Chain completed with status=%d, in_count=%d, out_count=%d\n", 
            redir_status, in_count, out_count);
    
    return redir_status;
}
