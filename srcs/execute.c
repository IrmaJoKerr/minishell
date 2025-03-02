/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:38:31 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

/*
Handle command execution status and
updates vars->error code with the exit status
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
*/
int execute_cmd(t_node *cmd_node, char **envp, t_vars *vars)
{
	char	*cmd_path;
	pid_t	pid;
	int		status;

	if (!cmd_node || !cmd_node->args || !cmd_node->args[0])
		return (1);
	if (is_builtin(cmd_node->args[0]))
		return (execute_builtin(cmd_node->args[0], cmd_node->args, vars));
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
