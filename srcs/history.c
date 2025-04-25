/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 06:29:46 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 14:52:33 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Skips specified number of lines in an open file.
- Reads and discards requested number of lines.
- Uses get_next_line to properly handle line endings.
- Ensures proper memory management during skipping.
- Stops if end of file is reached before count is complete.
Works with trim_history().
Example: For a history with 1000 lines
- skip_lines(fd, 500) skips first 500 entries
- Positions file pointer at 501st entry
- Properly frees all memory used during skipping
*/
void	skip_lines(int fd, int count)
{
	char	*line;
	int		i;

	i = 0;
	while (i < count)
	{
		line = get_next_line(fd);
		if (!line)
			break ;
		free(line);
		i++;
	}
}

/*
Opens history file with appropriate access mode.
- Checks if history file exists and has required permissions.
- Creates new history file with proper permissions if needed.
- Opens file with requested mode (read or write).
Returns:
File descriptor for history file or -1 on error.
Works with load_history() and save_history().
Example: init_history_fd(O_RDONLY)
- Checks if HISTORY_FILE exists and is readable
- Opens it for reading if exists and accessible
- Returns file descriptor or -1 on failure
*/
int	init_history_fd(int mode)
{
	int			fd;
	struct stat	st;

	ft_memset(&st, 0, sizeof(st));
	if (stat("temp", &st) == -1)
	{
		mkdir("temp", 0755);
	}
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
Counts the number of lines in the history file.
- Opens history file for reading.
- Reads each line using get_next_line() until EOF.
- Counts each line and properly frees memory.
Returns:
Total count of history entries in the file.
0 if file cannot be opened or is empty.
Works with load_history() and trim_history().

Example: If history file contains 100 commands
- Returns 100 after counting all lines
- Returns 0 if file doesn't exist or can't be opened
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
	line = get_next_line(fd);
	while (line)
	{
		count++;
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (count);
}
