/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:34:57 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

// /*
// Handle the heredoc redirection. Returns file descriptor for reading,
// or -1 on error. Works with run_heredoc.
// */
// int	handle_heredoc(t_node *node, t_vars *vars)
// {
//     char	*line;
//     int		fd[2];
//     int		heredoc_fd;

//     if (pipe(fd) == -1)
//     {
//         vars->error_code = 1;
//         return (-1);
//     }
//     while (1)
//     {
//         line = readline("> ");
//         if (!line || ft_strcmp(line, node->data) == 0)
//         {
//             free(line);
//             break;
//         }
//         write(fd[1], line, ft_strlen(line));
//         write(fd[1], "\n", 1);
//         free(line);
//     }
//     close(fd[1]);
//     heredoc_fd = fd[0];
//     return (heredoc_fd);
// }

// /*
// Run the heredoc redirection.
// */
// int	run_heredoc(t_node *node, t_vars *vars)
// {
// 	int	fd;

// 	fd = handle_heredoc(node, vars);
//     if (fd == -1)
//         return (0);
//     dup2(fd, STDIN_FILENO);
//     close(fd);
//     return (1);
// }

/*
Write line to heredoc pipe. Works with read_heredoc.
*/
int	write_to_heredoc(int fd, char *line)
{
	if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1)
		return (0);
	return (1);
}

/*
Process input for heredoc. Works with handle_heredoc.
*/
int	read_heredoc(int *fd, char *delimiter)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break;
		}
		if (!write_to_heredoc(fd[1], line))
		{
			free(line);
			return (0);
		}
		free(line);
	}
	return (1);
}

/*
Handle the heredoc redirection. Returns file descriptor for reading,
or -1 on error. Works with run_heredoc.
*/
int	handle_heredoc(t_node *node, t_vars *vars)
{
	int	fd[2];

	if (!node || !node->args || !node->args[0])
	{
		vars->error_code = 1;
		return (-1);
	}
	if (pipe(fd) == -1)
	{
		vars->error_code = 1;
		return (-1);
	}
	if (!read_heredoc(fd, node->args[0]))
	{
		close(fd[0]);
		close(fd[1]);
		vars->error_code = 1;
		return (-1);
	}
	close(fd[1]);
	return (fd[0]);
}

/*
Runs and controls the heredoc redirection.
*/
int	run_heredoc(t_node *node, t_vars *vars)
{
	int	fd;

	fd = handle_heredoc(node, vars);
	if (fd == -1)
		return (0);
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		close(fd);
		vars->error_code = 1;
		return (0);
	}
	close(fd);
	return (1);
}
