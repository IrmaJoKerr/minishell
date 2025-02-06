/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/26 23:12:57 by bleow             #+#    #+#             */
/*   Updated: 2024/10/28 22:28:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

char	*errormsg(char *msg);
void	cleanup_pipex(int type, t_pipex *pipex);
void	cleanup_paths(char **paths);

char	*errormsg(char *msg)
{
	if (ft_strcmp(msg, "syntaxerr") == 0)
		perror("Correct Syntax: ./pipex file1 cmd1 cmd2 file2\n");
	else if (ft_strcmp(msg, "pipeerr") == 0)
		perror("Error: Failed to create pipe");
	else if (ft_strcmp(msg, "pidserr") == 0)
		perror("Error: Failed to allocate memory for pids");
	else if (ft_strcmp(msg, "mallocerr") == 0)
		perror("Error: Failed to allocate memory");
	else if (ft_strcmp(msg, "spliterr") == 0)
		perror("Error: Failed to split command");
	else if (ft_strcmp(msg, "structerr") == 0)
		perror("Error: Failed to allocate memory for t_pipex");
	else if (ft_strcmp(msg, "forkerr") == 0)
		perror("Error: Failed to create fork");
	else if (ft_strcmp(msg, "openerr") == 0)
		perror("Error: Failed to open file");
	else if (ft_strcmp(msg, "accesserr") == 0)
		perror("Error: Failed to access file");
	else
		perror("Error: Unknown error");
	exit(EXIT_FAILURE);
	return (NULL);
}

void	cleanup_pipex(int type, t_pipex *pipex)
{
	int	i;

	if (type == 0 || type == 1 || type == 4)
	{
		perror("Error: Memory allocation for cmdarr failed\n");
		i = 0;
		while (i < pipex->args - 3)
		{
			free(pipex->cmdarr[i]);
			i++;
		}
		free(pipex->cmdarr);
	}
	if (type == 2 || type == 4)
	{
		perror("Error: Memory allocation for pids failed\n");
		free(pipex->pids);
	}
	if (type == 3 || type == 4)
	{
		perror("Error: Failed to split command\n");
		free(pipex->split_cmd);
	}
	free(pipex);
}

void	cleanup_paths(char **paths)
{
	int	i;

	i = 0;
	while (paths[i] != NULL)
	{
		free(paths[i]);
		i++;
	}
	free(paths);
}
