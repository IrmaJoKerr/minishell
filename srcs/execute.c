/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 01:25:06 by bleow            ###   ########.fr       */
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

int execute_cmd(t_node *cmd_node, char **envp, t_vars *vars)
{
    char    *cmd_path;
    pid_t   pid;
    int     status;
    int     i;

    if (!cmd_node)
    {
        printf("DEBUG: NULL command node\n");
        return (1);
    }
    
    printf("DEBUG: Executing %s node: %p\n",
        get_token_str(cmd_node->type), (void*)cmd_node);
    
    // Handle pipe nodes differently
    if (cmd_node->type == TYPE_PIPE)
    {
        printf("DEBUG: Executing pipe command\n");
        return execute_pipeline(cmd_node, envp, vars);
    }
    
    // Regular command processing
    if (!cmd_node->args || !cmd_node->args[0])
    {
        printf("DEBUG: Invalid command node or missing arguments\n");
        return (1);
    }
    
    expand_command_args(cmd_node, vars);
    printf("DEBUG: Command: '%s' with %ld arguments\n", 
           cmd_node->args[0], ft_arrlen(cmd_node->args) - 1);
    i = 0;
    printf("DEBUG: Arguments: ");
    while (cmd_node->args[i])
    {
        printf("'%s'%s", cmd_node->args[i], cmd_node->args[i+1] ? ", " : "");
        i++;
    }
    printf("\n");
    
    if (is_builtin(cmd_node->args[0]))
    {
        printf("DEBUG: Executing builtin %s command: %s\n", 
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
    
    printf("DEBUG: Found command path: %s\n", cmd_path);
    pid = fork();
    if (pid == 0)
    {
        printf("DEBUG: Child process executing: %s\n", cmd_path);
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
