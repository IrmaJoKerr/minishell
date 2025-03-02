/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   historysave.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 14:35:22 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:05 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

/*
Copy lines to temporary file.
This is for safety in case anything goes wrong during
reading/writing to the HISTORY_FILE.
*/
int	copy_to_temp(int fd_read)
{
	int		fd_write;
	char	*line;

	fd_write = open(HISTORY_FILE_TMP, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_write == -1)
		return (0);
	while ((line = get_next_line(fd_read)))
	{
		write(fd_write, line, ft_strlen(line));
		write(fd_write, "\n", 1);
		free(line);
	}
	close(fd_write);
	return (1);
}

/*
Skip specified number of lines in file
*/
void	skip_lines(int fd, int count)
{
	char	*line;

	while (count > 0 && (line = get_next_line(fd)))
	{
		free(line);
		count--;
	}
}

/*
Copy file contents from src to dst
*/
int	copy_file(const char *src, const char *dst)
{
	int		fd_src;
	int		fd_dst;
	char	buffer[4096];
	ssize_t	bytes;

	fd_src = open(src, O_RDONLY);
	if (fd_src == -1)
		return (0);
	fd_dst = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_dst == -1)
	{
		close(fd_src);
		return (0);
	}
	while ((bytes = read(fd_src, buffer, 4096)) > 0)
		if (write(fd_dst, buffer, bytes) == -1)
		{
			close(fd_src);
			close(fd_dst);
			return (0);
		}
	close(fd_src);
	close(fd_dst);
	return (bytes >= 0);
}

/*
Trim history file to maximum size by copying to temporary file
then only the required lines back. Works with save_history.
*/
void	trim_history(int excess_lines)
{
	int     fd;

	fd = init_history_fd(O_RDONLY);
	if (fd == -1)
		return;
	skip_lines(fd, excess_lines);
	if (!copy_to_temp(fd))
	{
		close(fd);
		return;
	}
	close(fd);
	if (copy_file(HISTORY_FILE_TMP, HISTORY_FILE))
		unlink(HISTORY_FILE_TMP);
}

/*
Save commands to history file. Main function controlling history saving.
*/
void	save_history(void)
{
	int		fd;
	char	*line;
	int		count;

	line = readline(PROMPT);
	if (!line || !*line)
	{
		free(line);
		return;
	}
	fd = init_history_fd(O_WRONLY | O_CREAT | O_APPEND);
	if (fd == -1)
	{
		free(line);
		return;
	}
	append_history(fd, line);
	close(fd);
	free(line);
	count = get_history_count();
	if (count > HISTORY_FILE_MAX)
		trim_history(count - HISTORY_FILE_MAX);
}
