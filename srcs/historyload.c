/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   historyload.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:48:27 by bleow             #+#    #+#             */
/*   Updated: 2025/03/04 11:54:37 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Skip specified number of lines from history file using get_next_line.
Used by load_history to skip excess history entries.
*/
void	skip_history_lines(int fd, int skip_count)
{
	char	*line;

	while (skip_count > 0)
	{
		line = get_next_line(fd);
		if (!line)
			break ;
		free(line);
		skip_count--;
	}
}

/*
Read history lines from file and add them to memory using add_history.
Helper function for load_history.
*/
void	read_history_lines(int fd)
{
	char	*line;

	line = get_next_line(fd);
	while (line)
	{
		if (*line)
			add_history(line);
		free(line);
		line = get_next_line(fd);
	}
}

/*
Load history from HISTORY_FILE into memory with limit (HIST_MEM_MAX).
Counts how many lines are in history file and skips excess lines.
Then it reads the history lines into memory.
Works with init_history_fd().
*/
void	load_history(void)
{
	int		fd;
	int		count;
	int		skip;

	count = get_history_count();
	if (count == 0)
		return ;
	fd = init_history_fd(O_RDONLY);
	if (fd == -1)
		return ;
	skip = 0;
	if (count > HIST_MEM_MAX)
		skip = count - HIST_MEM_MAX;
	skip_history_lines(fd, skip);
	read_history_lines(fd);
	close(fd);
}
