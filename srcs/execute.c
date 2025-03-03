/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 12:38:26 by bleow            ###   ########.fr       */
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

/*
Executes a command node by forking a child process and executing the command.
Example: ls -l
Searches for command in system path.
Forks a child process and executes the command "ls" with argument "-l".
If it can't find command anywhere in system it returns an error.
*/
int	execute_cmd(t_node *cmd_node, char **envp, t_vars *vars)
{
	char	*cmd_path;
	pid_t	pid;
	int		status;

	if (!cmd_node || !cmd_node->args || !cmd_node->args[0])
		return (1);
	/*
	Builtin to be implemented by lechan
	
	if (is_builtin(cmd_node->args[0]))
		return (execute_builtin(cmd_node->args[0], cmd_node->args, vars));
	*/
	cmd_path = get_cmd_path(cmd_node->args[0], envp);
	if (!cmd_path)
	{
		ft_putstr_fd("bleshell: command not found: ", 2);
		ft_putendl_fd(cmd_node->args[0], 2);
		vars->error_code = 127;
		return (vars->error_code);
	}
	pid = fork();
	if (pid == 0)
	{
		execve(cmd_path, cmd_node->args, envp);
		exit(1);
	}
	free(cmd_path);
	waitpid(pid, &status, 0);
	return (handle_cmd_status(status, vars));
}
