/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/03/23 02:34:58 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles command execution status and updates error code.
- Processes exit status from waitpid() for child processes.
- For normal exits, stores the exit code (0-255) directly.
- For signals, adds 128 to the signal number (POSIX standard).
Returns:
The final error code stored in vars->error_code.
Works with exec_child_cmd() and execute_pipeline().

Example: Child process terminated by SIGINT (signal 2)
- Sets vars->error_code to 130 (128+2)
- Returns 130
OLD VERSION
int	handle_cmd_status(int status, t_vars *vars)
{
	if (WIFEXITED(status))
		vars->error_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		vars->error_code = WTERMSIG(status) + 128;
	return (vars->error_code);
}
*/
int handle_cmd_status(int status, t_vars *vars)
{
    int exit_code = 0;
    
    if (WIFEXITED(status))
    {
        exit_code = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        exit_code = 128 + WTERMSIG(status);
    }
    
    if (vars)
        vars->error_code = exit_code;
    
    return exit_code;
}


/*
Handles redirection setup for output files.
- Opens file for writing in truncate or append mode.
- Redirects stdout to the opened file.
- Properly handles and reports errors.
Returns:
1 on success, 0 on failure.
Works with setup_redirection().
OLD VERSION
int	setup_out_redir(t_node *node, int *fd, int append)
{
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	*fd = open(node->right->args[0], flags, 0644);
	if (*fd == -1)
	{
		perror("open");
		return (0);
	}
	if (dup2(*fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		return (0);
	}
	return (1);
}
*/
/*NEWER OLD VERSION
int	setup_out_redir(t_node *node, int *fd, int append)
{
    char	*file;
    char	*processed_file;
    int		flags;
    
    // Handle possible NULL checks
    if (!node || !node->right || !node->right->args || !node->right->args[0])
        return (0);
    // Get the filename from the node
    file = node->right->args[0];
    // Make a copy of the filename to process quotes
    processed_file = ft_strdup(file);
    if (!processed_file)
        return (0);
    // Process quotes in filename if present
    process_quotes_in_arg(&processed_file);
    // Set flags based on append mode
    flags = O_WRONLY | O_CREAT;
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    // Open the file
    *fd = open(processed_file, flags, 0644);
    // Handle open errors
    if (*fd == -1)
    {
        ft_putstr_fd("bleshell: ", 2);
        ft_putstr_fd(processed_file, 2);
        ft_putendl_fd(": Permission denied", 2);
        ft_safefree((void **)&processed_file);
        return (0);
    }
    // Redirect stdout to the file
    if (dup2(*fd, STDOUT_FILENO) == -1)
    {
        close(*fd);
        ft_safefree((void **)&processed_file);
        return (0);
    }
    // Free the processed filename
    ft_safefree((void **)&processed_file);
    return (1);
}
*/
int setup_out_redir(t_node *node, int *fd, int append)
{
    char *file;
    int flags;
    
    if (!node || !node->args || !node->args[0])
        return (0);
    
    // Process quotes in file before accessing it
    process_quotes_in_redirect(node);
    
    file = node->args[0];
    
    // Set flags based on append mode
    flags = O_WRONLY | O_CREAT;
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    
    // Open the file
    *fd = open(file, flags, 0644);
    if (*fd == -1)
    {
        ft_putstr_fd("bleshell: ", 2);
        ft_putstr_fd(file, 2);
        ft_putendl_fd(": Permission denied", 2);
        return (0);
    }
    
    // Redirect stdout to the file
    if (dup2(*fd, STDOUT_FILENO) == -1)
    {
        close(*fd);
        return (0);
    }
    
    return (1);
}

/*
Handles redirection setup for input files.
- Opens file for reading.
- Redirects stdin to read from the file.
- Properly handles and reports errors.
Returns:
1 on success, 0 on failure.
Works with setup_redirection().
OLD VERSION
int	setup_in_redir(t_node *node, int *fd)
{
	*fd = open(node->right->args[0], O_RDONLY);
	if (*fd == -1)
	{
		perror("open");
		return (0);
	}
	if (dup2(*fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		return (0);
	}
	return (1);
}
*/
/*NEWER OLD VERSION
int setup_in_redir(t_node *node, int *fd)
{
    char	*filename;
    char	*processed_file;
    
    // Handle possible NULL checks
    if (!node || !node->right || !node->right->args || !node->right->args[0])
        return (0);
    // Get the filename from the node
    filename = node->right->args[0];
    // Make a copy to process quotes
    processed_file = ft_strdup(filename);
    if (!processed_file)
        return (0);
    // Process quotes in filename if present - use existing function
    process_quotes_in_arg(&processed_file);
    // Open the file
    *fd = open(processed_file, O_RDONLY);
    // Handle open errors
    if (*fd == -1)
    {
        ft_putstr_fd("bleshell: ", 2);
        ft_putstr_fd(processed_file, 2);
        ft_putendl_fd(": No such file or directory", 2);
        ft_safefree((void **)&processed_file);
        return (0);
    }
    // Redirect stdin to the file
    if (dup2(*fd, STDIN_FILENO) == -1)
    {
        close(*fd);
        ft_safefree((void **)&processed_file);
        return (0);
    }
    // Free the processed filename
    ft_safefree((void **)&processed_file);
    return (1);
}
*/
/*NEWER OLD VERSION
int setup_in_redir(t_node *node, int *fd)
{
    char *filename;
    
    if (!node || !node->args || !node->args[0])
        return (0);
    
    filename = node->args[0];
    
    // Open file for reading
    *fd = open(filename, O_RDONLY);
    if (*fd == -1)
    {
        ft_putstr_fd("bleshell: ", 2);
        ft_putstr_fd(filename, 2);
        ft_putendl_fd(": No such file or directory", 2);
        return (0);
    }
    
    // Redirect stdin to the file
    if (dup2(*fd, STDIN_FILENO) == -1)
    {
        close(*fd);
        return (0);
    }
    
    return (1);
}
*/
/*LATEST OLD VERSION
int setup_in_redir(t_node *node, int *fd)
{
    char *filename;
    
    if (!node || !node->args || !node->args[0])
        return (0);
    
    // Process quotes in filename before accessing it
    process_quotes_in_redirect(node);
    
    filename = node->args[0];
    
    // Open file for reading
    *fd = open(filename, O_RDONLY);
    if (*fd == -1)
    {
        ft_putstr_fd("bleshell: ", 2);
        ft_putstr_fd(filename, 2);
        ft_putendl_fd(": No such file or directory", 2);
        return (0);
    }
    
    // Redirect stdin to the file
    if (dup2(*fd, STDIN_FILENO) == -1)
    {
        close(*fd);
        return (0);
    }
    
    return (1);
}
*/
int setup_in_redir(t_node *node, int *fd, t_vars *vars)
{
    char *file;
    
    if (!node || !node->args || !node->args[0])
        return (0);
    
    file = node->args[0];
    
    // Open file for reading
    *fd = open(file, O_RDONLY);
    if (*fd == -1)
    {
        ft_putstr_fd("bleshell: ", 2);
        ft_putstr_fd(file, 2);
        ft_putendl_fd(": No such file or directory", 2);
        vars->error_code = 1; // Set error code to 1
        return (0);
    }
    
    // Redirect stdin to the file
    if (dup2(*fd, STDIN_FILENO) == -1)
    {
        close(*fd);
        return (0);
    }
    
    return (1);
}

/*
Sets up appropriate redirection based on node type.
- Handles all redirection types (input, output, append, heredoc).
- Creates or opens files with appropriate permissions.
- Redirects stdin/stdout as needed.
Returns:
1 on success, 0 on failure.
Works with exec_redirect_cmd().
OLD VERSION
int	setup_redirection(t_node *node, t_vars *vars, int *fd)
{
	if (node->type == TYPE_OUT_REDIRECT)
		return (setup_out_redir(node, fd, 0));
	else if (node->type == TYPE_APPEND_REDIRECT)
		return (setup_out_redir(node, fd, 1));
	else if (node->type == TYPE_IN_REDIRECT)
		return (setup_in_redir(node, fd));
	else if (node->type == TYPE_HEREDOC)
		return (proc_heredoc(node, vars));
	return (0);
}
*/
int setup_redirection(t_node *node, t_vars *vars, int *fd)
{
    // Process quotes in redirection filename
    process_quotes_in_redirect(node);
    
    // Handle different redirection types
    if (node->type == TYPE_IN_REDIRECT)
    {
        if (!setup_in_redir(node, fd, vars))
        {
            vars->error_code = 1;
            return (0);
        }
    }
    else if (node->type == TYPE_OUT_REDIRECT)
    {
        if (!setup_out_redir(node, fd, 0))
        {
            vars->error_code = 1;
            return (0);
        }
    }
    else if (node->type == TYPE_APPEND_REDIRECT)
    {
        if (!setup_out_redir(node, fd, 1))
        {
            vars->error_code = 1;
            return (0);
        }
    }
    else if (node->type == TYPE_HEREDOC)
    {
        char *delimiter = NULL;
        if (node->args && node->args[0])
            delimiter = node->args[0];
        else
            return (vars->error_code = 1);
            
        if (!read_heredoc(fd, delimiter, vars, 1))
        {
            vars->error_code = 1;
            return (0);
        }
        vars->pipeline->heredoc_fd = *fd;
    }
    
    return (1);
}

/*
Executes a command with redirection.
- Saves original file descriptors.
- Sets up redirection according to node type.
- Executes the command with redirection in place.
- Restores original file descriptors afterward.
Returns:
Result of command execution.
Works with execute_cmd().
*/
int	exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
{
	int	saved_stdout;
	int	saved_stdin;
	int	fd;
	int	result;

	if (!node->left || !node->right)
		return (1);
	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	fd = -1;
	if (!setup_redirection(node, vars, &fd))
		return (1);
	result = execute_cmd(node->left, envp, vars);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	cleanup_fds(saved_stdout, saved_stdin);
	if (fd > 2)
		close(fd);
	return (result);
}

/*
Executes a child process for external commands.
- Forks a child process.
- In child: executes the external command.
- In parent: waits for child and processes exit status.
Returns:
Exit code from the command execution.
Works with execute_cmd().
*/
int	exec_child_cmd(t_node *node, char **envp, t_vars *vars, char *cmd_path)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
	{
		if (execve(cmd_path, node->args, envp) == -1)
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
		return (handle_cmd_status(status, vars));
	}
	return (0);
}

int	exec_std_cmd(t_node *node, char **envp, t_vars *vars)
{
	char	*cmd_path;
	int		i;

	if (!node->args || !node->args[0])
		return (1);
	i = 0;
	while (node->args[i])
		i++;
	expand_cmd_args(node, vars);
	if (is_builtin(node->args[0]))
		return (execute_builtin(node->args[0], node->args, vars));
	cmd_path = get_cmd_path(node->args[0], envp);
	if (!cmd_path)
	{
		ft_putstr_fd("bleshell: command not found: ", 2);
		ft_putendl_fd(node->args[0], 2);
		vars->error_code = 127;
		return (vars->error_code);
	}
	return (exec_child_cmd(node, envp, vars, cmd_path));
}

/*
Executes a command node.
- Handles pipeline commands separately.
- Handles redirection commands separately.
OLDER VERSION
int	execute_cmd(t_node *node, char **envp, t_vars *vars)
{
	int		i;
	char	*exit_code;

	if (!node || !node->args || !node->args[0])
		return (1);
	if (!(is_builtin(node->args[0]) && ft_strcmp(node->args[0], "echo") == 0))
	{
		i = 0;
		while (node->args && node->args[i])
		{
			if (node->arg_quote_type && node->arg_quote_type[i] == 1)
			{
				i++;
				continue ;
			}
			if (ft_strcmp(node->args[i], "$?") == 0)
			{
				exit_code = ft_itoa(vars->error_code);
				if (exit_code)
				{
					ft_safefree((void **)&node->args[i]);
					node->args[i] = exit_code;
				}
			}
			i++;
		}
	}
	if (node->type == TYPE_PIPE)
		return (execute_pipeline(node, vars));
	if (node->type == TYPE_OUT_REDIRECT || node->type == TYPE_APPEND_REDIRECT
		|| node->type == TYPE_IN_REDIRECT || node->type == TYPE_HEREDOC)
		return (exec_redirect_cmd(node, envp, vars));
	return (exec_std_cmd(node, envp, vars));
}
*/
/*
Main command execution function.
- Handles all command types (builtin, external, redirections, pipes).
- Ensures consistent error code handling in vars->error_code.
Returns:
Exit code which is also stored in vars->error_code.
*/
int execute_cmd(t_node *node, char **envp, t_vars *vars)
{
    int result = 0;
    
    if (!node)
        return (vars->error_code = 1);
        
    // Handle different node types
    if (node->type == TYPE_PIPE)
        result = execute_pipeline(node, vars);
    else if (is_redirection(node->type))
        result = exec_redirect_cmd(node, envp, vars);
    else if (node->type == TYPE_CMD)
    {
        if (is_builtin(node->args[0]))
            result = execute_builtin(node->args[0], node->args, vars);
        else
            result = exec_external_cmd(node, envp, vars);
    }
    
    // Ensure error code is set in vars
    vars->error_code = result;
    return (result);
}

/*
Executes an external command (non-builtin).
- Finds the command path in the PATH environment.
- Forks a child process to execute the command.
- In parent: waits for child and handles the exit status.
- Properly updates vars->error_code with the command result.
Returns:
Exit code from the command execution.
Works with execute_cmd().

Example: For "ls -la"
- Locates path to ls executable (/bin/ls)
- Executes in child process
- Returns exit code (0 for success)
*/
int	exec_external_cmd(t_node *node, char **envp, t_vars *vars)
{
    pid_t	pid;
    int		status;
    char	*cmd_path;

    if (!node || !node->args || !node->args[0])
        return (vars->error_code = 1);

    // Get command path
    cmd_path = get_cmd_path(node->args[0], envp);
    if (!cmd_path)
    {
        ft_putstr_fd("bleshell: ", 2);
        ft_putstr_fd(node->args[0], 2);
        ft_putendl_fd(": command not found", 2);
        return (vars->error_code = 127);
    }

    // Fork and execute
    pid = fork();
    if (pid < 0)
    {
        ft_putstr_fd("bleshell: fork failed\n", 2);
        ft_safefree((void **)&cmd_path);
        return (vars->error_code = 1);
    }
    
    // Child process
    if (pid == 0)
    {
        execve(cmd_path, node->args, envp);
        // If execve returns, an error occurred
        perror("bleshell");
        ft_safefree((void **)&cmd_path);
        exit(127);
    }
    
    // Parent process
    ft_safefree((void **)&cmd_path);
    waitpid(pid, &status, 0);
    
    // Handle exit status and update vars->error_code
    return (handle_cmd_status(status, vars));
}
