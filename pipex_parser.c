/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/26 13:29:15 by bleow             #+#    #+#             */
/*   Updated: 2024/10/28 22:22:18 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

char	**find_env_path(char **env);
void	cmdspaces(int i, char **argv, t_pipex *pipex);
void	cmdparser(char **argv, t_pipex *pipex);
void	execute_command(char **cmd, char **env, t_pipex *pipex);

/*
Function to find the PATH environment variable in the envp array.
*/
char	**find_env_path(char **env)
{
	int		i;
	char	*temp;
	char	**paths;

	i = 0;
	temp = NULL;
	paths = NULL;
	while (env[i] != NULL)
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
		{
			temp = env[i] + 5;
			break ;
		}
		i++;
	}
	if (temp != NULL)
		paths = ft_split(temp, ':');
	if (!paths)
		errormsg("mallocerr");
	return (paths);
}

void	cmdspaces(int i, char **argv, t_pipex *pipex)
{
	int	k;

	k = 0;
	pipex->split_cmd = ft_split(argv[i], ' ');
	if (!pipex->split_cmd)
	{
		cleanup_pipex(3, pipex);
		errormsg("spliterr");
	}
	while (pipex->split_cmd[k] != NULL)
	{
		if (pipex->a < MAX_ARGS - 1)
		{
			pipex->cmdarr[pipex->c][pipex->a++] = pipex->split_cmd[k++];
		}
	}
	pipex->cmdarr[pipex->c][pipex->a] = NULL;
	pipex->c++;
	pipex->a = 0;
}

void	cmdparser(char **argv, t_pipex *pipex)
{
	int	i;

	i = 2;
	while (i < (pipex->args - 1))
	{
		if (ft_strchr(argv[i], ' ') != NULL)
		{
			cmdspaces(i, argv, pipex);
		}
		else
		{
			pipex->cmdarr[pipex->c][0] = argv[i];
			pipex->cmdarr[pipex->c][1] = NULL;
			pipex->c++;
			pipex->a = 0;
		}
		i++;
	}
}

void	execute_command(char **cmd, char **env, t_pipex *pipex)
{
	char	*path;

	if (cmd == NULL || cmd[0] == NULL)
	{
		cleanup_pipex(4, pipex);
		errormsg("spliterr");
	}
	path = find_cmd_path(*cmd, env);
	if (!path)
	{
		perror("Error: Failed to find a valid command path");
		cleanup_paths(*pipex->cmdarr);
		exit(EXIT_FAILURE);
	}
	if (execve(path, cmd, env) == -1)
	{
		perror("Error: Failed to execute command with execve");
		cleanup_paths(*pipex->cmdarr);
		exit(EXIT_FAILURE);
	}
	free(path);
}
