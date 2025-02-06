/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_builder.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 19:36:31 by bleow             #+#    #+#             */
/*   Updated: 2024/10/28 22:29:16 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

t_pipex	*init_pipex(int argc);
void	mem_alloc_cmdarr(t_pipex *pipex, int argc);
char	*find_cmd_path(char *cmd, char **env);
char	*build_path(const char *path, const char *cmd);

t_pipex	*init_pipex(int argc)
{
	t_pipex	*pipex;

	pipex = malloc(sizeof(t_pipex));
	if (!pipex)
		errormsg("structerr");
	pipex->pid_created = 0;
	pipex->exit_status = 0;
	pipex->c = 0;
	pipex->a = 0;
	pipex->split_cmd = NULL;
	pipex->args = argc;
	pipex->pids = malloc(sizeof(pid_t) * (argc - 3));
	if (!pipex->pids)
	{
		free(pipex);
		cleanup_pipex(2, pipex);
	}
	mem_alloc_cmdarr(pipex, argc);
	return (pipex);
}

void	mem_alloc_cmdarr(t_pipex *pipex, int argc)
{
	int	i;
	int	j;

	i = 0;
	pipex->cmdarr = (char ***)malloc(sizeof(char **) * (argc - 3));
	if (!pipex->cmdarr)
		cleanup_pipex(0, pipex);
	while (i < (argc - 3))
	{
		pipex->cmdarr[i] = (char **)malloc(sizeof(char *) * MAX_ARGS);
		if (!pipex->cmdarr[i])
			cleanup_pipex(1, pipex);
		j = 0;
		while (j < MAX_ARGS)
		{
			pipex->cmdarr[i][j] = NULL;
			j++;
		}
		i++;
	}
}

char	*find_cmd_path(char *cmd, char **env)
{
	char	**paths;
	char	*test_path;
	char	*full_path;
	int		i;

	i = 0;
	test_path = NULL;
	full_path = NULL;
	paths = find_env_path(env);
	while (paths[i] != NULL)
	{
		test_path = build_path(paths[i], cmd);
		if (access(test_path, F_OK | X_OK) == 0)
		{
			cleanup_paths(paths);
			full_path = test_path;
			return (full_path);
		}
		i++;
		free(test_path);
		test_path = NULL;
	}
	cleanup_paths(paths);
	return (NULL);
}

char	*build_path(const char *path, const char *cmd)
{
	char	*full_path;
	size_t	path_len;
	size_t	cmd_len;

	path_len = ft_strlen(path);
	cmd_len = ft_strlen(cmd);
	full_path = malloc(ft_strlen(path) + ft_strlen(cmd) + 2);
	if (!full_path)
		errormsg("mallocerr");
	ft_strlcpy(full_path, path, path_len + 1);
	ft_strlcat(full_path, "/", path_len + 2);
	ft_strlcat(full_path, cmd, path_len + cmd_len + 2);
	return (full_path);
}
