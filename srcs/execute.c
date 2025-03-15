/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 21:29:28 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handle command execution status and update vars->error_code with the
exit status.
For normal exits, the exit code (0-255) is stored directly.
For signals, 128 is added to the signal number (POSIX standard).
Examples:
- Command exits normally with status 1: error_code = 1
- Command terminated by SIGINT (signal 2): error_code = 130 (128+2)
- Command terminated by SIGQUIT (signal 3): error_code = 131 (128+3)
*/
int	handle_cmd_status(int status, t_vars *vars)
{
	if (WIFEXITED(status))
		vars->error_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		vars->error_code = WTERMSIG(status) + 128;
	return (vars->error_code);
}

/*OLD VERSION
int execute_cmd(t_node *cmd_node, char **envp, t_vars *vars)
{
    char    *cmd_path;
    pid_t   pid;
    int     status;
    int     i;

    if (!cmd_node)
    {
        fprintf(stderr, "DEBUG: NULL command node\n");
        return (1);
    }
    
    fprintf(stderr, "DEBUG: Executing %s node: %p\n",
        get_token_str(cmd_node->type), (void*)cmd_node);
    
    // Handle pipe nodes differently
    if (cmd_node && cmd_node->type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Executing pipe command\n");
        return execute_pipeline(cmd_node, envp, vars);
    }
    
    // Handle redirection nodes
    if (cmd_node && (cmd_node->type == TYPE_OUT_REDIRECT || 
                    cmd_node->type == TYPE_APPEND_REDIRECT || 
                    cmd_node->type == TYPE_IN_REDIRECT ||
                    cmd_node->type == TYPE_HEREDOC))
    {
        fprintf(stderr, "DEBUG: Executing redirection\n");
        
        // Save original file descriptors
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stdin = dup(STDIN_FILENO);
        int fd = -1;
        
        // Set up the redirection based on type
        if (cmd_node->type == TYPE_OUT_REDIRECT)
        {
            // Open file for writing (create/truncate)
            fd = open(cmd_node->right->args[0], O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd != -1) {
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else {
                perror("open");
            }
        }
        else if (cmd_node->type == TYPE_APPEND_REDIRECT)
        {
            // Open file for appending
            fd = open(cmd_node->right->args[0], O_WRONLY|O_CREAT|O_APPEND, 0644);
            if (fd != -1) {
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else {
                perror("open");
            }
        }
        else if (cmd_node->type == TYPE_IN_REDIRECT)
        {
            // Open file for reading
            fd = open(cmd_node->right->args[0], O_RDONLY);
            if (fd != -1) {
                dup2(fd, STDIN_FILENO);
                close(fd);
            } else {
                perror("open");
                return 1;
            }
        }
        else if (cmd_node->type == TYPE_HEREDOC)
        {
            // Handle heredoc (using existing run_heredoc function if available)
            if (!run_heredoc(cmd_node, vars))
                return 1;
        }
        
        // Execute the command with redirection in place
        int result = execute_cmd(cmd_node->left, envp, vars);
        
        // Restore original file descriptors
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdout);
        close(saved_stdin);
        
        return result;
    }
    
    // Regular command processing
    if (!cmd_node->args || !cmd_node->args[0])
    {
        fprintf(stderr, "DEBUG: Invalid command node or missing arguments\n");
        return (1);
    }
    
    expand_command_args(cmd_node, vars);
    fprintf(stderr, "DEBUG: Command: '%s' with %ld arguments\n", 
           cmd_node->args[0], ft_arrlen(cmd_node->args) - 1);
    i = 0;
    fprintf(stderr, "DEBUG: Arguments: ");
    while (cmd_node->args[i])
    {
        printf("'%s'%s", cmd_node->args[i], cmd_node->args[i+1] ? ", " : "");
        i++;
    }
    printf("\n");
    
    if (is_builtin(cmd_node->args[0]))
    {
        fprintf(stderr, "DEBUG: Executing builtin %s command: %s\n", 
            get_token_str(cmd_node->type), cmd_node->args[0]);
        return (execute_builtin(cmd_node->args[0], cmd_node->args, vars));
    }
    
    cmd_path = get_cmd_path(cmd_node->args[0], envp);
    if (!cmd_path)
    {
        ft_putstr_fd("bleshell: command not found: ", 2);
        ft_putendl_fd(cmd_node->args[0], 2);
        vars->error_code = 127;
        return (vars->error_code);
    }
    
    fprintf(stderr, "DEBUG: Found command path: %s\n", cmd_path);
    pid = fork();
    if (pid == 0)
    {
        fprintf(stderr, "DEBUG: Child process executing: %s\n", cmd_path);
        if (execve(cmd_path, cmd_node->args, envp) == -1)
        {
            perror("bleshell");
            exit(1);
        }
    }
    else if (pid < 0)
    {
        perror("bleshell: fork");
        ft_safefree((void **)&cmd_path);
        return (1);
    }
    else
    {
        waitpid(pid, &status, 0);
        ft_safefree((void **)&cmd_path);
        return handle_cmd_status(status, vars);
    }
    return (0);
}
*/
int execute_cmd(t_node *cmd_node, char **envp, t_vars *vars)
{
    char    *cmd_path;
    pid_t   pid;
    int     status;
    int     i;

    if (!cmd_node)
    {
        fprintf(stderr, "DEBUG: NULL command node\n");
        return (1);
    }
    
    fprintf(stderr, "DEBUG: Executing %s node: %p\n",
        get_token_str(cmd_node->type), (void*)cmd_node);
    
    // Handle pipe nodes differently
    if (cmd_node && cmd_node->type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Executing pipe command\n");
        return execute_pipeline(cmd_node, envp, vars);
    }
    
    // Handle redirection nodes
    if (cmd_node && (cmd_node->type == TYPE_OUT_REDIRECT || 
                    cmd_node->type == TYPE_APPEND_REDIRECT || 
                    cmd_node->type == TYPE_IN_REDIRECT ||
                    cmd_node->type == TYPE_HEREDOC))
    {
        fprintf(stderr, "DEBUG: Executing redirection %s\n", get_token_str(cmd_node->type));
        
        // Check redirection node structure
        if (!cmd_node->left || !cmd_node->right)
        {
            fprintf(stderr, "DEBUG: Invalid redirection node structure: left=%p, right=%p\n", 
                   (void*)cmd_node->left, (void*)cmd_node->right);
            return 1;
        }
        
        // Print details about the redirection
        fprintf(stderr, "DEBUG: Redirecting %s to %s\n", 
               cmd_node->left->args ? cmd_node->left->args[0] : "NULL",
               cmd_node->right->args ? cmd_node->right->args[0] : "NULL");
        
        // Save original file descriptors
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stdin = dup(STDIN_FILENO);
        int fd = -1;
        
        // Set up the redirection based on type
        if (cmd_node->type == TYPE_OUT_REDIRECT)
        {
            // Open file for writing (create/truncate)
            fprintf(stderr, "DEBUG: Opening '%s' for output redirection (O_WRONLY|O_CREAT|O_TRUNC)\n", 
                   cmd_node->right->args[0]);
            fd = open(cmd_node->right->args[0], O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd != -1) {
                fprintf(stderr, "DEBUG: Successfully opened file, fd=%d\n", fd);
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    fprintf(stderr, "DEBUG: dup2 failed for stdout redirection\n");
                    perror("dup2");
                } else {
                    fprintf(stderr, "DEBUG: Successfully redirected stdout to fd %d\n", fd);
                }
                close(fd);
            } else {
                fprintf(stderr, "DEBUG: Failed to open file '%s'\n", cmd_node->right->args[0]);
                perror("open");
            }
        }
        else if (cmd_node->type == TYPE_APPEND_REDIRECT)
        {
            // Open file for appending
            fprintf(stderr, "DEBUG: Opening '%s' for append redirection (O_WRONLY|O_CREAT|O_APPEND)\n", 
                   cmd_node->right->args[0]);
            fd = open(cmd_node->right->args[0], O_WRONLY|O_CREAT|O_APPEND, 0644);
            if (fd != -1) {
                fprintf(stderr, "DEBUG: Successfully opened file for append, fd=%d\n", fd);
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    fprintf(stderr, "DEBUG: dup2 failed for stdout redirection\n");
                    perror("dup2");
                } else {
                    fprintf(stderr, "DEBUG: Successfully redirected stdout to fd %d\n", fd);
                }
                close(fd);
            } else {
                fprintf(stderr, "DEBUG: Failed to open file '%s' for append\n", cmd_node->right->args[0]);
                perror("open");
            }
        }
        else if (cmd_node->type == TYPE_IN_REDIRECT)
        {
            // Open file for reading
            fprintf(stderr, "DEBUG: Opening '%s' for input redirection (O_RDONLY)\n", 
                   cmd_node->right->args[0]);
            fd = open(cmd_node->right->args[0], O_RDONLY);
            if (fd != -1) {
                fprintf(stderr, "DEBUG: Successfully opened file for reading, fd=%d\n", fd);
                if (dup2(fd, STDIN_FILENO) == -1) {
                    fprintf(stderr, "DEBUG: dup2 failed for stdin redirection\n");
                    perror("dup2");
                } else {
                    fprintf(stderr, "DEBUG: Successfully redirected stdin from fd %d\n", fd);
                }
                close(fd);
            } else {
                fprintf(stderr, "DEBUG: Failed to open file '%s' for reading\n", cmd_node->right->args[0]);
                perror("open");
                return 1;
            }
        }
        else if (cmd_node->type == TYPE_HEREDOC)
        {
            // Handle heredoc (using existing run_heredoc function if available)
            fprintf(stderr, "DEBUG: Processing heredoc\n");
            if (!run_heredoc(cmd_node, vars))
                return 1;
        }
        
        // Execute the command with redirection in place
        fprintf(stderr, "DEBUG: Executing command with redirection: %s\n", 
               cmd_node->left->args ? cmd_node->left->args[0] : "NULL");
        int result = execute_cmd(cmd_node->left, envp, vars);
        
        // Restore original file descriptors
        fprintf(stderr, "DEBUG: Restoring original file descriptors\n");
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdout);
        close(saved_stdin);
        
        return result;
    }
    
    // Regular command processing
    if (!cmd_node->args || !cmd_node->args[0])
    {
        fprintf(stderr, "DEBUG: Invalid command node or missing arguments\n");
        return (1);
    }
    
    expand_command_args(cmd_node, vars);
    fprintf(stderr, "DEBUG: Command: '%s' with %ld arguments\n", 
           cmd_node->args[0], ft_arrlen(cmd_node->args) - 1);
    i = 0;
    fprintf(stderr, "DEBUG: Arguments: ");
    while (cmd_node->args[i])
    {
		fprintf(stderr, "'%s'%s", cmd_node->args[i], cmd_node->args[i+1] ? ", " : "");
		i++;
	}
	fprintf(stderr, "\n");
    
    if (is_builtin(cmd_node->args[0]))
    {
        fprintf(stderr, "DEBUG: Executing builtin %s command: %s\n", 
            get_token_str(cmd_node->type), cmd_node->args[0]);
        return (execute_builtin(cmd_node->args[0], cmd_node->args, vars));
    }
    
    cmd_path = get_cmd_path(cmd_node->args[0], envp);
    if (!cmd_path)
    {
        ft_putstr_fd("bleshell: command not found: ", 2);
        ft_putendl_fd(cmd_node->args[0], 2);
        vars->error_code = 127;
        return (vars->error_code);
    }
    
    fprintf(stderr, "DEBUG: Found command path: %s\n", cmd_path);
    pid = fork();
    if (pid == 0)
    {
        fprintf(stderr, "DEBUG: Child process executing: %s\n", cmd_path);
        if (execve(cmd_path, cmd_node->args, envp) == -1)
        {
            perror("bleshell");
            exit(1);
        }
    }
    else if (pid < 0)
    {
        perror("bleshell: fork");
        ft_safefree((void **)&cmd_path);
        return (1);
    }
    else
    {
        waitpid(pid, &status, 0);
        ft_safefree((void **)&cmd_path);
        return handle_cmd_status(status, vars);
    }
    return (0);
}
