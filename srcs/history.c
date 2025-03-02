/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 06:29:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:01 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Checks and initialise the history fd. 
If no HISTORY_FILE it will make one. Works with load_history and
save_history.
*/
int	init_history_fd(int mode)
{
	int	fd;

	if (access(HISTORY_FILE, F_OK) == -1)
	{
		fd = open(HISTORY_FILE, O_WRONLY | O_CREAT, 0644);
		if (fd == -1)
			return (-1);
		close(fd);
	}
	if (mode == O_RDONLY && access(HISTORY_FILE, R_OK) == -1)
		return (-1);
	if (mode == O_WRONLY && access(HISTORY_FILE, W_OK) == -1)
		return (-1);
	fd = open(HISTORY_FILE, mode);
	return (fd);
}

/*
Append line to both history file and memory. Works with save_history.
*/
int	append_history(int fd, const char *line)
{
	ssize_t	write_ret;

	if (!line)
		return (0);
	write_ret = write(fd, line, ft_strlen(line));
	if (write_ret == -1)
		return (0);
	write_ret = write(fd, "\n", 1);
	if (write_ret == -1)
		return (0);
	add_history(line);
	return (1);
}

/*
Count lines in history file. Works with load_history.
*/
int	get_history_count(void)
{
	int		fd;
	int		count;
	char	*line;

	count = 0;
	fd = init_history_fd(O_RDONLY);
	if (fd == -1)
		return (0);
	while ((line = get_next_line(fd)))
	{
		count++;
		free(line);
	}
	close(fd);
	return (count);
}

/*
Load history into memory with limit. Works with init_history_fd.
*/
void    load_history(void)
{
	int     fd;
	char    *line;
	int     count;
	int     skip;

	count = get_history_count();
	if (count == 0)
		return;
	fd = init_history_fd(O_RDONLY);
	if (fd == -1)
		return;
	if (count > HIST_MEM_MAX)
		skip = count - HIST_MEM_MAX;
	else
		skip = 0;
	while (skip-- > 0 && (line = get_next_line(fd)))
		free(line);
	while ((line = get_next_line(fd)))
	{
		if (*line)
			add_history(line);
		free(line);
	}
	close(fd);
}
