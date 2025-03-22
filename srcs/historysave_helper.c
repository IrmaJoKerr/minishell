/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   historysave_helper.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:22:41 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 01:50:24 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Helper function to perform actual file copy operation.
Takes file descriptors and returns 1 on success, 0 on failure.
Works with copy_file.
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
Copy file contents from src to dst.
Opens source and destination files, then calls copy_file_content.
Returns 1 on success, 0 on failure.
Used to save history to temporary file and then back to the actual
history file.
Works with save_history.
*/
int	copy_file(const char *src, const char *dst)
{
	int		fd_src;
	int		fd_dst;
	int		result;

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
