/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 09:52:41 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 13:47:34 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Executes commands connected by a pipe.
- Sets up and launches pipeline child processes.
- Waits for both processes to complete.
- Captures status of command execution.
Returns:
- Status of right command.
- 1 on error.
Works with execute_cmd() for pipeline execution.

Example: For "ls -l | grep txt"
- Sets up pipeline processes
- Waits for both commands to complete
- Returns final execution status
*/
int execute_pipes(t_node *pipe_node, t_vars *vars)
{
	int     pipe_fd[2];
	pid_t   left_pid;
	pid_t   right_pid;
	int     status;
	int     left_status = 0;
	
	if (pipe(pipe_fd) == -1) {
		ft_putendl_fd("pipe: Creation failed", 2);
		return 1;
	}
	left_pid = fork();
	if (left_pid == 0)
	{
		close(pipe_fd[0]);  // Close read end in left process
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
			exit(1);
		close(pipe_fd[1]);
		exit(execute_cmd(pipe_node->left, vars->env, vars));
	}
	else if (left_pid > 0)
	{
		close(pipe_fd[1]);  // Close write end in parent
		right_pid = fork();
		if (right_pid == 0)
		{
			if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
				exit(1);
			close(pipe_fd[0]);
			exit(execute_cmd(pipe_node->right, vars->env, vars));
		}
		else if (right_pid > 0)
		{
			close(pipe_fd[0]);
			waitpid(left_pid, &left_status, 0);
			waitpid(right_pid, &status, 0);
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
				return left_status;
			else
				return (handle_cmd_status(status, vars));
		}
	}
	ft_putendl_fd("fork: Creation failed", 2);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	return 1;
}

/*
Check for unfinished pipes in input and handle them.
Prompts for additional input as needed.
Returns:
- 1 if pipes were handled and modifications were made
- 0 if no unfinished pipes found
- -1 if an error occurred
*/
int handle_unfinished_pipes(char **processed_cmd, t_vars *vars)
{
	char *addon_input;
	char *tmp;
	char *combined;

	addon_input = NULL;
	tmp = NULL;
	
	addon_input = readline("> ");
	if (!addon_input)
		return (-1);
	tmp = ft_strtrim(addon_input, " \t\n");
	free(addon_input);
	addon_input = tmp;
	if (!addon_input || addon_input[0] == '\0')
	{
		free(addon_input);
		return handle_unfinished_pipes(processed_cmd, vars);
	}
	tmp = ft_strjoin(*processed_cmd, " ");
	if (!tmp)
	{
		free(addon_input);
		return (-1);
	}
	combined = ft_strjoin(tmp, addon_input);
	free(tmp);
	free(addon_input);
	if (!combined)
		return (-1);
	free(*processed_cmd);
	*processed_cmd = combined;
	if (!process_input_tokens(*processed_cmd, vars))
    	return (-1);
	if (analyze_pipe_syntax(vars) == 2)
		return handle_unfinished_pipes(processed_cmd, vars);
	return (1);
}
