/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 06:15:43 by bleow             #+#    #+#             */
/*   Updated: 2025/04/24 06:18:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Trims history file to maximum allowed size.
- Opens history file for reading.
- Skips oldest entries if file exceeds size limit.
- Copies remaining entries to temporary file.
- Replaces original file with trimmed version.
- Removes temporary file after successful operation.
Returns:
Nothing (void function).
Works with save_history().

Example: If HISTORY_FILE_MAX=1000 and file has 1200 entries
- Skips first 200 entries (oldest commands)
- Copies newest 1000 entries to temporary file
- Replaces original history file with trimmed version
- Maintains history file within configured size limit
*/
void	trim_history(int excess_lines)
{
	int	fd;

	fd = init_history_fd(O_RDONLY);
	if (fd == -1)
		return ;
	skip_lines(fd, excess_lines);
	if (!copy_to_temp(fd))
	{
		close(fd);
		return ;
	}
	close(fd);
	if (copy_file(HISTORY_FILE_TMP, HISTORY_FILE))
		unlink(HISTORY_FILE_TMP);
}

/*
Ensures a directory exists, creating it if needed.
Returns 1 on success, 0 if directory creation failed.
*/
int	chk_and_make_folder(const char *path)
{
    struct stat	st = {0};
    
    if (stat(path, &st) == -1)
    {
        return (mkdir(path, 0755) == 0);
    }
    return (1);
}

/*
Helper function to perform actual file copy operation.
- Reads from source file in chunks of 4096 bytes.
- Writes each chunk to destination file.
- Continues until entire file is copied.
Returns:
1 on successful copy, 0 on any read/write error.
Works with copy_file().

Example: For a 10KB history file
- Reads file in ~3 chunks of 4096 bytes
- Writes each chunk to destination
- Returns 1 when complete copy is successful
*/
int	copy_file_content(int fd_src, int fd_dst)
{
	char	buffer[4096];
	ssize_t	bytes;

	bytes = read(fd_src, buffer, 4096);
	while (bytes > 0)
	{
		if (write(fd_dst, buffer, bytes) == -1)
			return (0);
		bytes = read(fd_src, buffer, 4096);
	}
	return (bytes >= 0);
}

/*
Copies file contents from source to destination.
- Opens source file for reading.
- Opens destination file for writing (creates if needed).
- Calls copy_file_content to perform the actual copy.
- Ensures proper cleanup of file descriptors.
Returns:
1 on successful copy, 0 on any error.
Works with trim_history().

Example: copy_file(HISTORY_FILE_TMP, HISTORY_FILE)
- Copies from temporary history file to main history file
- Creates or overwrites destination file with source content
- Returns 1 on successful operation
*/
int	copy_file(const char *src, const char *dst)
{
	int	fd_src;
	int	fd_dst;
	int	result;

	fd_src = open(src, O_RDONLY);
	if (fd_src == -1)
		return (0);
	fd_dst = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_dst == -1)
	{
		close(fd_src);
		return (0);
	}
	result = copy_file_content(fd_src, fd_dst);
	close(fd_src);
	close(fd_dst);
	return (result);
}

/*
Copies lines to temporary history file for safe operations.
- Reads lines one by one from source file descriptor.
- Writes each line to temporary file with newline.
- Provides safety during history file manipulation.
Returns:
1 on successful copy, 0 on any error.
Works with trim_history().

Example: During history trimming
- Copies newer history entries to temporary file
- Protects against data corruption during manipulation
- Returns 1 when temporary file is ready for use
*/
int	copy_to_temp(int fd_read)
{
	int		fd_write;
	char	*line;

	fd_write = open(HISTORY_FILE_TMP, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_write == -1)
		return (0);
	line = get_next_line(fd_read);
	while (line)
	{
		write(fd_write, line, ft_strlen(line));
		free(line);
		line = get_next_line(fd_read);
	}
	close(fd_write);
	return (1);
}
