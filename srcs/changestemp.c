
/*
Process quotes in redirection file names and arguments.
- Handles both single and double quotes.
- Removes quotes from filename/argument but preserves content.
- Recognizes that "file name" is a complete quoted string, not unclosed.
*/
void process_quotes_in_redirection(char **str_ptr)
{
    char *str;
    char *result;
    int i;
    int j;
    int in_single_quote;
    int in_double_quote;
    
    if (!str_ptr || !*str_ptr)
        return;
    
    str = *str_ptr;
    result = malloc(strlen(str) + 1);
    if (!result)
        return;
    
    i = 0;
    j = 0;
    in_single_quote = 0;
    in_double_quote = 0;
    
    while (str[i])
    {
        if (str[i] == '\'' && !in_double_quote)
            in_single_quote = !in_single_quote;
        else if (str[i] == '\"' && !in_single_quote)
            in_double_quote = !in_double_quote;
        else
            result[j++] = str[i];
        i++;
    }
    
    result[j] = '\0';
    ft_safefree((void **)str_ptr);
    *str_ptr = result;
}

// Update redirection handling
int setup_redirection(t_node *node, t_vars *vars, int *fd)
{
    char *filename;
    int result;
    
    // Save the original filename
    if (node->args && node->args[0])
        filename = ft_strdup(node->args[0]);
    else
        return 0;
    
    // Process quotes in the filename
    process_quotes_in_redirection(&filename);
    
    // Rest of your redirection setup with the processed filename
    if (node->type == TYPE_IN_REDIRECT)
        result = setup_in_redir(filename, fd);
    else if (node->type == TYPE_OUT_REDIRECT)
        result = setup_out_redir(filename, fd, 0);
    else if (node->type == TYPE_APPEND_REDIRECT)
        result = setup_out_redir(filename, fd, 1);
    else if (node->type == TYPE_HEREDOC)
        result = read_heredoc(node, vars, fd);
    else
        result = 0;
    
    ft_safefree((void **)&filename);
    return result;
}

/*
Execute a command node with redirections.
- Handles input, output, and append redirections.
- Restores original file descriptors after execution.
Returns:
Exit code from the command execution.
OLD VERSION
int exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
{
    int fd_in = STDIN_FILENO;
    int fd_out = STDOUT_FILENO;
    int saved_in = dup(STDIN_FILENO);
    int saved_out = dup(STDOUT_FILENO);
    int result;
    
    // Set up the redirection
    if (!setup_redirection(node, vars, &fd_in) || !setup_redirection(node, vars, &fd_out))
    {
        // Handle error
        cleanup_fds(saved_in, saved_out);
        return 1;
    }
    
    // Find the command node associated with this redirection
    t_node *cmd_node = find_associated_cmd(vars->head, node);
    if (!cmd_node)
    {
        cleanup_fds(saved_in, saved_out);
        return 1;
    }
    
    // Execute the command with redirection
    result = exec_std_cmd(cmd_node, envp, vars);
    
    // Restore original file descriptors
    dup2(saved_in, STDIN_FILENO);
    dup2(saved_out, STDOUT_FILENO);
    cleanup_fds(saved_in, saved_out);
    
    return result;
}
*/
int exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
{
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);
    int result;
    
    // Apply redirection
    if (!setup_redirection(node, vars, NULL))
    {
        // Restore original stdin/stdout
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdin);
        close(saved_stdout);
        
        // Return with error code already set in setup_redirection
        return vars->error_code;
    }
    
    // Execute the command with redirected I/O
    result = execute_cmd(find_associated_cmd(node, vars), envp, vars);
    
    // Restore original stdin/stdout
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdin);
    close(saved_stdout);
    
    return (result);
}

int setup_multi_redirects(t_node *cmd_node, t_vars *vars)
{
    t_node *current = vars->head;
    int fd_in = STDIN_FILENO;
    int fd_out = STDOUT_FILENO;
    int result = 0;
    
    // First pass: set up all input redirections
    while (current)
    {
        if ((current->type == TYPE_IN_REDIRECT || current->type == TYPE_HEREDOC) &&
            is_related_to_cmd(current, cmd_node))
        {
            // Process the input redirection
            if (!setup_redirection(current, vars, &fd_in))
                return 0;
        }
        current = current->next;
    }
    
    // Second pass: set up all output redirections
    current = vars->head;
    while (current)
    {
        if ((current->type == TYPE_OUT_REDIRECT || current->type == TYPE_APPEND_REDIRECT) &&
            is_related_to_cmd(current, cmd_node))
        {
            // Process the output redirection
            if (!setup_redirection(current, vars, &fd_out))
                return 0;
        }
        current = current->next;
    }
    
    return 1;
}

int setup_in_redir(char *filename, int *fd)
{
    *fd = open(filename, O_RDONLY);
    if (*fd == -1)
    {
        ft_putstr_fd("bleshell: ", 2);
        ft_putstr_fd(filename, 2);
        ft_putendl_fd(": No such file or directory", 2);
        return 0;
    }
    // Redirect stdin to the file
    if (dup2(*fd, STDIN_FILENO) == -1)
    {
        close(*fd);
        return (0);
    }
    
    return (1);
}

int execute_pipeline(t_node *pipe_node, t_vars *vars)
{
    int pipefd[2];
    pid_t left_pid, right_pid;
    int status = 0;
    
    if (!pipe_node || pipe_node->type != TYPE_PIPE)
        return 1;
    
    // Create the pipe
    if (pipe(pipefd) == -1)
    {
        print_error("pipe creation failed", vars, 1);
        return 1;
    }
    
    // Execute left side of pipe
    left_pid = fork();
    if (left_pid == 0)
    {
        // Child process for left command
        close(pipefd[0]);  // Close read end
        
        // Redirect stdout to pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
            exit(1);
            
        close(pipefd[1]);  // Close original write end
        
        // Handle any redirections in the left command
        t_node *left_cmd = pipe_node->left;
        setup_cmd_redirects(left_cmd, vars);
        
        // Execute the left command
        int result = execute_cmd(left_cmd, vars->env, vars);
        exit(result);
    }
    else if (left_pid < 0)
    {
        print_error("fork failed", vars, 1);
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    
    // Execute right side of pipe
    right_pid = fork();
    if (right_pid == 0)
    {
        // Child process for right command
        close(pipefd[1]);  // Close write end
        
        // Redirect stdin to pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
            exit(1);
            
        close(pipefd[0]);  // Close original read end
        
        // Handle any redirections in the right command
        t_node *right_cmd = pipe_node->right;
        setup_cmd_redirects(right_cmd, vars);
        
        // Execute the right command
        int result = execute_cmd(right_cmd, vars->env, vars);
        exit(result);
    }
    else if (right_pid < 0)
    {
        print_error("fork failed", vars, 1);
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    
    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    
    // Wait for both processes
    waitpid(left_pid, &status, 0);
    waitpid(right_pid, &status, 0);
    
    return handle_cmd_status(status, vars);
}


int builtin_export(char **args, t_vars *vars)
{
    // Your existing export code
    
    // Special handling for pipelines
    if (vars->in_pipeline)
    {
        // If we're in a pipeline, we need to communicate env changes to parent
        // This isn't how bash works, but it's how your tests expect it
        // You could use a temporary file to store changes and read them back
        // in the parent process after pipeline completion
    }
    
    return 0;
}
