/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirects.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:39:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 09:23:02 by bleow            ###   ########.fr       */
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
// int	setup_in_redir(t_node *node, t_vars *vars)
// {
// 	char	*file;

// 	if (!node->right || !node->right->args || !node->right->args[0])
// 		return (0);
// 	file = node->right->args[0];
// 	if (!chk_permissions(file, O_RDONLY, vars))
// 	{
// 		end_pipe_processes(vars);
// 		return (0);
// 	}
// 	vars->pipes->redirection_fd = open(file, O_RDONLY);
// 	if (vars->pipes->redirection_fd == -1)
// 	{
// 		not_found_error(file, vars);
// 		end_pipe_processes(vars);
// 		return (0);
// 	}
// 	if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
// 	{
// 		close(vars->pipes->redirection_fd);
// 		return (0);
// 	}
// 	return (1);
// }
// int	setup_in_redir(t_node *node, t_vars *vars)
// {
//     char	*file;

//     if (!node->right || !node->right->args || !node->right->args[0])
//         return (0);
//     file = node->right->args[0];
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Opening file '%s' for input redirection\n", file);
    
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Permission check failed for '%s'\n", file);
//         vars->error_code = ERR_REDIRECTION; // Set specific error code
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Failed to open '%s' (error: %s)\n", file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION; // Set specific error code
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully opened '%s' with fd %d\n", file, vars->pipes->redirection_fd);
    
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: dup2 failed for fd %d\n", vars->pipes->redirection_fd);
//         close(vars->pipes->redirection_fd);
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully redirected stdin to file '%s'\n", file);
//     return (1);
// }
// int	setup_in_redir(t_node *node, t_vars *vars)
// {
//     char	*file;
//     struct stat	file_stat;

//     if (!node->right || !node->right->args || !node->right->args[0])
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: No valid redirection target\n");
//         return (0);
//     }
    
//     file = node->right->args[0];
//     fprintf(stderr, "DEBUG-IN-REDIR: Setting up input redirection from file '%s'\n", file);
    
//     // Check if file exists first
//     if (access(file, F_OK) != 0)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: File '%s' does not exist (errno: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     // Check if file is a directory
//     if (stat(file, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: '%s' is a directory\n", file);
//         shell_error(file, ERR_ISDIRECTORY, vars);
//         vars->error_code = ERR_REDIRECTION;
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Permission check failed for '%s'\n", file);
//         vars->error_code = ERR_REDIRECTION; 
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Failed to open '%s' (error: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully opened '%s' with fd %d\n", 
//             file, vars->pipes->redirection_fd);
    
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: dup2 failed for fd %d (error: %s)\n", 
//                 vars->pipes->redirection_fd, strerror(errno));
//         close(vars->pipes->redirection_fd);
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully redirected stdin to file '%s'\n", file);
//     return (1);
// }
// int	setup_in_redir(t_node *node, t_vars *vars)
// {
//     char	*file;
//     struct stat	file_stat;

//     if (!node->right || !node->right->args || !node->right->args[0])
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: No valid redirection target\n");
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         return (0);
//     }
    
//     file = node->right->args[0];
//     fprintf(stderr, "DEBUG-IN-REDIR: Setting up input redirection from file '%s'\n", file);
    
//     // Check if file exists first
//     if (access(file, F_OK) != 0)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: File '%s' does not exist (errno: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     // Check if file is a directory
//     if (stat(file, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: '%s' is a directory\n", file);
//         shell_error(file, ERR_ISDIRECTORY, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Permission check failed for '%s'\n", file);
//         vars->error_code = ERR_REDIRECTION; 
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Failed to open '%s' (error: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully opened '%s' with fd %d\n", 
//             file, vars->pipes->redirection_fd);
    
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: dup2 failed for fd %d (error: %s)\n", 
//                 vars->pipes->redirection_fd, strerror(errno));
//         close(vars->pipes->redirection_fd);
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully redirected stdin to file '%s'\n", file);
//     return (1);
// }
// int	setup_in_redir(t_node *node, t_vars *vars)
// {
//     char	*file;
//     struct stat	file_stat;

//     if (!node->right || !node->right->args || !node->right->args[0])
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: No valid redirection target\n");
//         return (0);
//     }
    
//     file = node->right->args[0];
//     fprintf(stderr, "DEBUG-IN-REDIR: Setting up input redirection from file '%s'\n", file);
    
//     // Check if file exists first
//     if (access(file, F_OK) != 0)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: File '%s' does not exist (errno: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Exiting.\n", 
//                 getpid(), file);
//         // end_pipe_processes(vars);
//         return (0);
//     }
    
//     // Check if file is a directory
//     if (stat(file, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: '%s' is a directory\n", file);
//         shell_error(file, ERR_ISDIRECTORY, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Exiting.\n", 
//                 getpid(), file);
//         // end_pipe_processes(vars);
//         return (0);
//     }
    
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Permission check failed for '%s'\n", file);
//         vars->error_code = ERR_REDIRECTION; 
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Exiting.\n", 
//                 getpid(), file);
//         // end_pipe_processes(vars);
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Failed to open '%s' (error: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Exiting.\n", 
//                 getpid(), file);
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully opened '%s' with fd %d\n", 
//             file, vars->pipes->redirection_fd);
    
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: dup2 failed for fd %d (error: %s)\n", 
//                 vars->pipes->redirection_fd, strerror(errno));
//         close(vars->pipes->redirection_fd);
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully redirected stdin to file '%s'\n", file);
//     return (1);
// }
// int setup_in_redir(t_node *node, t_vars *vars)
// {
//     char    *file;
//     struct stat file_stat;

//     if (!node->right || !node->right->args || !node->right->args[0])
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: No valid redirection target\n");
//         return (0);
//     }
    
//     file = node->right->args[0];
//     fprintf(stderr, "DEBUG-IN-REDIR: Setting up input redirection from file '%s'\n", file);
    
//     // Check if file exists first
//     if (access(file, F_OK) != 0)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: File '%s' does not exist (errno: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
//                 getpid(), file);
//         // Removed call to end_pipe_processes(vars)
//         return (0);
//     }
    
//     // Check if file is a directory
//     if (stat(file, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: '%s' is a directory\n", file);
//         shell_error(file, ERR_ISDIRECTORY, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
//                 getpid(), file);
//         // Removed call to end_pipe_processes(vars)
//         return (0);
//     }
    
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Permission check failed for '%s'\n", file);
//         vars->error_code = ERR_REDIRECTION; 
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
//                 getpid(), file);
//         // Removed call to end_pipe_processes(vars)
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Failed to open '%s' (error: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
//                 getpid(), file);
//         // Retain this call for file descriptor cleanup only
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     // Rest of function remains unchanged
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully opened '%s' with fd %d\n", 
//             file, vars->pipes->redirection_fd);
    
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: dup2 failed for fd %d (error: %s)\n", 
//                 vars->pipes->redirection_fd, strerror(errno));
//         close(vars->pipes->redirection_fd);
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully redirected stdin to file '%s'\n", file);
//     return (1);
// }
// int setup_in_redir(t_node *node, t_vars *vars)
// {
//     char    *file;
//     struct stat file_stat;

//     if (!node->right || !node->right->args || !node->right->args[0])
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: No valid redirection target\n");
//         return (0);
//     }
    
//     file = node->right->args[0];
//     fprintf(stderr, "DEBUG-IN-REDIR: Setting up input redirection from file '%s'\n", file);
    
//     // Check if file exists first
//     if (access(file, F_OK) != 0)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: File '%s' does not exist (errno: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
//                 getpid(), file);
//         // Removed call to end_pipe_processes(vars)
//         return (0);
//     }
    
//     // Check if file is a directory
//     if (stat(file, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: '%s' is a directory\n", file);
//         shell_error(file, ERR_ISDIRECTORY, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
//                 getpid(), file);
//         // Removed call to end_pipe_processes(vars)
//         return (0);
//     }
    
//     if (!chk_permissions(file, O_RDONLY, vars))
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Permission check failed for '%s'\n", file);
//         vars->error_code = ERR_REDIRECTION; 
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
//                 getpid(), file);
//         // Removed call to end_pipe_processes(vars)
//         return (0);
//     }
    
//     vars->pipes->redirection_fd = open(file, O_RDONLY);
//     if (vars->pipes->redirection_fd == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: Failed to open '%s' (error: %s)\n", 
//                 file, strerror(errno));
//         not_found_error(file, vars);
//         vars->error_code = ERR_REDIRECTION;
//         fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
//                 getpid(), file);
//         // Keep this call for file descriptor cleanup
//         end_pipe_processes(vars);
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully opened '%s' with fd %d\n", 
//             file, vars->pipes->redirection_fd);
    
//     if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
//     {
//         fprintf(stderr, "DEBUG-IN-REDIR: dup2 failed for fd %d (error: %s)\n", 
//                 vars->pipes->redirection_fd, strerror(errno));
//         close(vars->pipes->redirection_fd);
//         fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
//                 getpid(), getppid());
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-IN-REDIR: Successfully redirected stdin to file '%s'\n", file);
//     return (1);
// }
int setup_in_redir(t_node *node, t_vars *vars)
{
    char    *file;
    struct stat file_stat;
    int     null_fd;

    if (!node->right || !node->right->args || !node->right->args[0])
    {
        fprintf(stderr, "DEBUG-IN-REDIR: No valid redirection target\n");
        return (0);
    }
    
    file = node->right->args[0];
    fprintf(stderr, "DEBUG-IN-REDIR: Setting up input redirection from file '%s'\n", file);
    
    // Check if file exists first
    if (access(file, F_OK) != 0)
    {
        fprintf(stderr, "DEBUG-IN-REDIR: File '%s' does not exist (errno: %s)\n", 
                file, strerror(errno));
        not_found_error(file, vars);
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
                getpid(), file);
        
        // If in pipe context, redirect stdin to /dev/null to prevent hanging
        if (vars->pipes && vars->pipes->pipe_root)
        {
            fprintf(stderr, "DEBUG-IN-REDIR: In pipe context, redirecting stdin to /dev/null\n");
            null_fd = open("/dev/null", O_RDONLY);
            if (null_fd != -1)
            {
                dup2(null_fd, STDIN_FILENO);
                close(null_fd);
                fprintf(stderr, "DEBUG-IN-REDIR: Stdin now connected to /dev/null\n");
            }
        }
        
        return (0);
    }
    
    // Check if file is a directory
    if (stat(file, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
    {
        fprintf(stderr, "DEBUG-IN-REDIR: '%s' is a directory\n", file);
        shell_error(file, ERR_ISDIRECTORY, vars);
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
                getpid(), file);
        
        // If in pipe context, redirect stdin to /dev/null to prevent hanging
        if (vars->pipes && vars->pipes->pipe_root)
        {
            fprintf(stderr, "DEBUG-IN-REDIR: In pipe context, redirecting stdin to /dev/null\n");
            null_fd = open("/dev/null", O_RDONLY);
            if (null_fd != -1)
            {
                dup2(null_fd, STDIN_FILENO);
                close(null_fd);
                fprintf(stderr, "DEBUG-IN-REDIR: Stdin now connected to /dev/null\n");
            }
        }
        
        return (0);
    }
    
    if (!chk_permissions(file, O_RDONLY, vars))
    {
        fprintf(stderr, "DEBUG-IN-REDIR: Permission check failed for '%s'\n", file);
        vars->error_code = ERR_REDIRECTION; 
        fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
                getpid(), file);
        
        // If in pipe context, redirect stdin to /dev/null to prevent hanging
        if (vars->pipes && vars->pipes->pipe_root)
        {
            fprintf(stderr, "DEBUG-IN-REDIR: In pipe context, redirecting stdin to /dev/null\n");
            null_fd = open("/dev/null", O_RDONLY);
            if (null_fd != -1)
            {
                dup2(null_fd, STDIN_FILENO);
                close(null_fd);
                fprintf(stderr, "DEBUG-IN-REDIR: Stdin now connected to /dev/null\n");
            }
        }
        
        return (0);
    }
    
    vars->pipes->redirection_fd = open(file, O_RDONLY);
    if (vars->pipes->redirection_fd == -1)
    {
        fprintf(stderr, "DEBUG-IN-REDIR: Failed to open '%s' (error: %s)\n", 
                file, strerror(errno));
        not_found_error(file, vars);
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-CRITICAL: Input redirection error in PID %d for file '%s'. Continuing.\n", 
                getpid(), file);
        
        // Keep this call for file descriptor cleanup
        end_pipe_processes(vars);
        return (0);
    }
    
    fprintf(stderr, "DEBUG-IN-REDIR: Successfully opened '%s' with fd %d\n", 
            file, vars->pipes->redirection_fd);
    
    if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG-IN-REDIR: dup2 failed for fd %d (error: %s)\n", 
                vars->pipes->redirection_fd, strerror(errno));
        close(vars->pipes->redirection_fd);
        fprintf(stderr, "DEBUG-IN-REDIR: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-IN-REDIR: Successfully redirected stdin to file '%s'\n", file);
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
// 	char	*file;
// 	int		flags;

// 	if (!node->right || !node->right->args || !node->right->args[0])
// 		return (0);
// 	flags = O_WRONLY | O_CREAT;
// 	file = node->right->args[0];
// 	if (vars->pipes->out_mode == OUT_MODE_APPEND)
// 		flags |= O_APPEND;
// 	else
// 		flags |= O_TRUNC;
// 	if (!chk_permissions(file, flags, vars))
// 		return (0);
// 	vars->pipes->redirection_fd = open(file, flags, 0644);
// 	if (vars->pipes->redirection_fd == -1)
// 	{
// 		shell_error(file, ERR_PERMISSIONS, vars);
// 		return (0);
// 	}
// 	if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
// 	{
// 		close(vars->pipes->redirection_fd);
// 		return (0);
// 	}
// 	return (1);
// }
int	setup_out_redir(t_node *node, t_vars *vars)
{
    char	*file;
    int		flags;

    if (!node->right || !node->right->args || !node->right->args[0])
    {
        fprintf(stderr, "DEBUG-OUT-REDIR: No valid redirection target\n");
        fprintf(stderr, "DEBUG-OUT-REDIR: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    file = node->right->args[0];
    fprintf(stderr, "DEBUG-OUT-REDIR: Setting up output redirection to file '%s'\n", file);
    
    // Set flags based on redirection mode (truncate or append)
    if (vars->pipes->out_mode == OUT_MODE_APPEND)
        flags = O_WRONLY | O_CREAT | O_APPEND;
    else
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    
    if (!chk_permissions(file, flags, vars))
    {
        fprintf(stderr, "DEBUG-OUT-REDIR: Permission check failed for '%s'\n", file);
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-OUT-REDIR: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    vars->pipes->redirection_fd = open(file, flags, 0644);
    if (vars->pipes->redirection_fd == -1)
    {
        fprintf(stderr, "DEBUG-OUT-REDIR: Failed to open '%s' for writing (error: %s)\n", 
                file, strerror(errno));
        shell_error(file, ERR_PERMISSIONS, vars);
        vars->error_code = ERR_REDIRECTION;
        fprintf(stderr, "DEBUG-OUT-REDIR: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-OUT-REDIR: Successfully opened '%s' with fd %d\n", 
            file, vars->pipes->redirection_fd);
    
    if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG-OUT-REDIR: dup2 failed for fd %d (error: %s)\n", 
                vars->pipes->redirection_fd, strerror(errno));
        close(vars->pipes->redirection_fd);
        fprintf(stderr, "DEBUG-OUT-REDIR: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-OUT-REDIR: Successfully redirected stdout to file '%s'\n", file);
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
// int	setup_heredoc_redir(t_node *node, t_vars *vars)
// {
// 	int	result;

// 	result = 1;
// 	if (!vars->hd_text_ready)
// 	{
// 		if (!interactive_hd_mode(vars))
// 			return (0);
// 	}
// 	result = handle_heredoc(node, vars);
// 	return (result);
// }
int	setup_heredoc_redir(t_node *node, t_vars *vars)
{
    // Basic heredoc setup checks
    if (!node || !vars)
    {
        fprintf(stderr, "DEBUG-HEREDOC-REDIR: Invalid node or vars\n");
        fprintf(stderr, "DEBUG-HEREDOC-REDIR: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-HEREDOC-REDIR: Setting up heredoc redirection\n");
    
    // Check if we need to extract the heredoc delimiter
    if (!vars->pipes->heredoc_delim && node->right && node->right->args && node->right->args[0])
    {
        if (!is_valid_delim(node->right->args[0], vars))
        {
            fprintf(stderr, "DEBUG-HEREDOC-REDIR: Invalid heredoc delimiter\n");
            fprintf(stderr, "DEBUG-HEREDOC-REDIR: Redirection failed in process %d, parent: %d\n", 
                    getpid(), getppid());
            return (0);
        }
        
        fprintf(stderr, "DEBUG-HEREDOC-REDIR: Using delimiter: %s\n", vars->pipes->heredoc_delim);
    }
    
    // Gather heredoc content if not already done
    if (!vars->hd_text_ready)
    {
        fprintf(stderr, "DEBUG-HEREDOC-REDIR: Starting interactive heredoc mode\n");
        if (!interactive_hd_mode(vars))
        {
            fprintf(stderr, "DEBUG-HEREDOC-REDIR: Interactive heredoc mode failed\n");
            fprintf(stderr, "DEBUG-HEREDOC-REDIR: Redirection failed in process %d, parent: %d\n", 
                    getpid(), getppid());
            return (0);
        }
    }
    
    // Process and set up the heredoc fd
    if (!handle_heredoc(node, vars))
    {
        fprintf(stderr, "DEBUG-HEREDOC-REDIR: Failed to handle heredoc\n");
        fprintf(stderr, "DEBUG-HEREDOC-REDIR: Redirection failed in process %d, parent: %d\n", 
                getpid(), getppid());
        return (0);
    }
    
    fprintf(stderr, "DEBUG-HEREDOC-REDIR: Successfully set up heredoc redirection\n");
    return (1);
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
// int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars) PRE ADDED DEBUG PRINTS
// {
// 	t_node	*current_node;
// 	t_node	*next_redir;

// 	current_node = start_node;
// 	while (current_node && is_redirection(current_node->type))
// 	{
// 		vars->pipes->current_redirect = current_node;
// 		if (!setup_redirection(current_node, vars))
// 		{
// 			reset_redirect_fds(vars);
// 			return (0);
// 		}
// 		if (current_node->redir)
// 			current_node = current_node->redir;
// 		else
// 		{
// 			next_redir = get_next_redir(current_node, cmd_node);
// 			if (next_redir)
// 				current_node = next_redir;
// 			else
// 				break ;
// 		}
// 	}
// 	return (1);
// }
// int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
// 	t_node	*current_node;
// 	t_node	*next_redir;

// 	fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain starting with node type=%d\n", 
// 		start_node->type);
// 	current_node = start_node;
// 	while (current_node && is_redirection(current_node->type))
// 	{
// 		vars->pipes->current_redirect = current_node;
// 		fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection node type=%d, target: %s\n", 
// 			current_node->type, 
// 			current_node->right && current_node->right->args ? 
// 			current_node->right->args[0] : "NULL");
// 		if (!setup_redirection(current_node, vars))
// 		{
// 			fprintf(stderr, "DEBUG-REDIR-CHAIN: Setup redirection failed\n");
// 			reset_redirect_fds(vars);
// 			return (0);
// 		}
// 		if (current_node->redir)
// 		{
// 			fprintf(stderr, "DEBUG-REDIR-CHAIN: Moving to linked redirection node\n");
// 			current_node = current_node->redir;
// 		}
// 		else
// 		{
// 			next_redir = get_next_redir(current_node, cmd_node);
// 			if (next_redir)
// 			{
// 				fprintf(stderr, "DEBUG-REDIR-CHAIN: Moving to next redirection node in list\n");
// 				current_node = next_redir;
// 			}
// 			else
// 			{
// 				fprintf(stderr, "DEBUG-REDIR-CHAIN: No more redirections in chain\n");
// 				break ;
// 			}
// 		}
// 	}
// 	fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection chain processing complete\n");
// 	return (1);
// }
// int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node	*current_node;
//     t_node	*next_redir;

//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain starting with node type=%d for command: %s\n", 
//         start_node->type, cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection node type=%d\n", current_node->type);
//         vars->pipes->current_redirect = current_node;
        
//         // Print target file for input/output redirections
//         if (current_node->right && current_node->right->args)
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection target file: '%s'\n", 
//                 current_node->right->args[0] ? current_node->right->args[0] : "NULL");
                
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup failed (error code %d)\n", vars->error_code);
//             reset_redirect_fds(vars);
//             return (0);
//         }
        
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup successful\n");
        
//         if (current_node->redir)
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Following redir link to next redirection node\n");
//             current_node = current_node->redir;
//         }
//         else
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: No direct redir link, searching for next redirection\n");
//             next_redir = get_next_redir(current_node, cmd_node);
//             if (next_redir)
//             {
//                 fprintf(stderr, "DEBUG-REDIR-CHAIN: Found next redirection node type=%d\n", next_redir->type);
//                 current_node = next_redir;
//             }
//             else
//             {
//                 fprintf(stderr, "DEBUG-REDIR-CHAIN: No more redirections found for this command\n");
//                 break;
//             }
//         }
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirection chain successfully\n");
//     return (1);
// }
// int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node	*current_node;
//     t_node	*next_redir;

//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain starting with node type=%d for command: %s\n", 
//         start_node->type, cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection node type=%d\n", current_node->type);
//         vars->pipes->current_redirect = current_node;
        
//         // Print target file for input/output redirections
//         if (current_node->right && current_node->right->args)
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection target file: '%s'\n", 
//                 current_node->right->args[0] ? current_node->right->args[0] : "NULL");
                
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup failed (error code %d)\n", vars->error_code);
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection failed in process %d, parent: %d\n", 
//                     getpid(), getppid());
//             reset_redirect_fds(vars);
//             return (0);
//         }
        
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup successful\n");
        
//         if (current_node->redir)
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Following redir link to next redirection node\n");
//             current_node = current_node->redir;
//         }
//         else
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: No direct redir link, searching for next redirection\n");
//             next_redir = get_next_redir(current_node, cmd_node);
//             if (next_redir)
//             {
//                 fprintf(stderr, "DEBUG-REDIR-CHAIN: Found next redirection node type=%d\n", next_redir->type);
//                 current_node = next_redir;
//             }
//             else
//             {
//                 fprintf(stderr, "DEBUG-REDIR-CHAIN: No more redirections found for this command\n");
//                 break;
//             }
//         }
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirection chain successfully\n");
//     return (1);
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;
//     int had_errors = 0;

//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain starting with node type=%d for command: %s\n", 
//         start_node->type, cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection node type=%d\n", current_node->type);
//         vars->pipes->current_redirect = current_node;
        
//         // Print target file for input/output redirections
//         if (current_node->right && current_node->right->args)
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection target file: '%s'\n", 
//                 current_node->right->args[0] ? current_node->right->args[0] : "NULL");
        
//         // Try setup but don't abort chain on failure
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup failed (error code %d)\n", vars->error_code);
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection failed in process %d, parent: %d\n", 
//                     getpid(), getppid());
//             had_errors = 1;
//             // Continue with the chain despite error
//         }
//         else
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup successful\n");
//         }
        
//         if (current_node->redir)
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Following redir link to next redirection node\n");
//             current_node = current_node->redir;
//         }
//         else
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: No direct redir link, searching for next redirection\n");
//             next_redir = get_next_redir(current_node, cmd_node);
//             if (next_redir)
//             {
//                 fprintf(stderr, "DEBUG-REDIR-CHAIN: Found next redirection node type=%d\n", next_redir->type);
//                 current_node = next_redir;
//             }
//             else
//             {
//                 fprintf(stderr, "DEBUG-REDIR-CHAIN: No more redirections found for this command\n");
//                 break;
//             }
//         }
//     }
    
//     if (had_errors)
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Some redirections failed, but continuing with command execution\n");
//         reset_redirect_fds(vars);
//         // Return 1 to allow command execution despite redirection errors
//         return (1);
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirection chain successfully\n");
//     return (1);
// }
// int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
// {
//     t_node *current_node;
//     t_node *next_redir;

//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain starting with node type=%d for command: %s\n", 
//         start_node->type, cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    
//     current_node = start_node;
//     while (current_node && is_redirection(current_node->type))
//     {
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection node type=%d\n", current_node->type);
//         vars->pipes->current_redirect = current_node;
        
//         // Print target file for input/output redirections
//         if (current_node->right && current_node->right->args)
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection target file: '%s'\n", 
//                 current_node->right->args[0] ? current_node->right->args[0] : "NULL");
        
//         // Try setup but stop chain on failure (bash behavior)
//         if (!setup_redirection(current_node, vars))
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup failed (error code %d)\n", vars->error_code);
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection failed in process %d, parent: %d\n", 
//                     getpid(), getppid());
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Stopping redirection chain processing after first failure\n");
            
//             reset_redirect_fds(vars);
//             // For bash-like behavior: still return 1 to allow command execution despite failures
//             return (1);
//         }
        
//         fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup successful\n");
        
//         // Continue to next redirection only if this one succeeded
//         if (current_node->redir)
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: Following redir link to next redirection node\n");
//             current_node = current_node->redir;
//         }
//         else
//         {
//             fprintf(stderr, "DEBUG-REDIR-CHAIN: No direct redir link, searching for next redirection\n");
//             next_redir = get_next_redir(current_node, cmd_node);
//             if (next_redir)
//             {
//                 fprintf(stderr, "DEBUG-REDIR-CHAIN: Found next redirection node type=%d\n", next_redir->type);
//                 current_node = next_redir;
//             }
//             else
//             {
//                 fprintf(stderr, "DEBUG-REDIR-CHAIN: No more redirections found for this command\n");
//                 break;
//             }
//         }
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirection chain successfully\n");
//     return (1);
// }
int proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
{
    t_node *current_node;
    t_node *next_redir;
    int had_errors = 0;
    // Determine if we're in a pipe context by directly checking pipe_root
    int in_pipe_context = (vars->pipes && vars->pipes->pipe_root != NULL);

    fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain starting with node type=%d for command: %s\n", 
        start_node->type, cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
    fprintf(stderr, "DEBUG-REDIR-CONTEXT: In %s context (pipe_root: %p)\n", 
        in_pipe_context ? "pipe" : "standalone", (void*)(vars->pipes ? vars->pipes->pipe_root : NULL));
    
    current_node = start_node;
    while (current_node && is_redirection(current_node->type))
    {
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection node type=%d\n", current_node->type);
        vars->pipes->current_redirect = current_node;
        
        // Print target file for input/output redirections
        if (current_node->right && current_node->right->args)
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection target file: '%s'\n", 
                current_node->right->args[0] ? current_node->right->args[0] : "NULL");
        
        // Try setup but stop chain on failure (bash behavior)
        if (!setup_redirection(current_node, vars))
        {
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup failed (error code %d)\n", vars->error_code);
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection failed in process %d, parent: %d\n", 
                    getpid(), getppid());
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Stopping redirection chain processing after first failure\n");
            
            had_errors = 1;
            break; // Stop processing more redirections after first failure (bash behavior)
        }
        
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection setup successful\n");
        
        // Continue to next redirection only if this one succeeded
        if (current_node->redir)
        {
            fprintf(stderr, "DEBUG-REDIR-CHAIN: Following redir link to next redirection node\n");
            current_node = current_node->redir;
        }
        else
        {
            fprintf(stderr, "DEBUG-REDIR-CHAIN: No direct redir link, searching for next redirection\n");
            next_redir = get_next_redir(current_node, cmd_node);
            if (next_redir)
            {
                fprintf(stderr, "DEBUG-REDIR-CHAIN: Found next redirection node type=%d\n", next_redir->type);
                current_node = next_redir;
            }
            else
            {
                fprintf(stderr, "DEBUG-REDIR-CHAIN: No more redirections found for this command\n");
                break;
            }
        }
    }
    
    if (had_errors)
    {
        fprintf(stderr, "DEBUG-REDIR-CHAIN: Some redirections failed\n");
        reset_redirect_fds(vars);
        
        if (in_pipe_context)
        {
            fprintf(stderr, "DEBUG-REDIR-CHAIN: In pipe context, continuing despite errors\n");
            return (1); // Continue in pipe context
        }
        else
        {
            fprintf(stderr, "DEBUG-REDIR-CHAIN: In standalone context, aborting execution\n");
            return (0); // Abort in standalone context (bash-like behavior)
        }
    }
    
    fprintf(stderr, "DEBUG-REDIR-CHAIN: Finished processing redirection chain successfully\n");
    return (1);
}

