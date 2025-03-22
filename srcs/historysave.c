/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   historysave.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 14:35:22 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 17:59:50 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Copy lines to temporary file.
This is for safety in case anything goes wrong during
reading/writing to the HISTORY_FILE.
Returns 1 on success, 0 on failure.
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
		write(fd_write, "\n", 1);
		ft_safefree((void **)&line);
		line = get_next_line(fd_read);
	}
	close(fd_write);
	return (1);
}

/*
Skip specified number of lines in file.
Uses get_next_line to skip to the target line.
Works with trim_history.
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
		ft_safefree((void **)&line);
		i++;
	}
}

/*
Trim history file to maximum size by copying to temporary file
then only the required lines back using
1) skip_lines to skip to required point in the history file
2) copy_to_temp for temporary storage in case there is a simultaneous write
   that corrupts the data during the read process.
3) copy_file back from temporary storage for final product.
Works with save_history.
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
Save commands to history file. Main function controlling history saving.
It performs several steps:
1. Reads a command line from user input
2. Validates the input is not empty
3. Opens the history file for appending
4. Adds the command to both the file and readline's history
5. Trims the history file to maximum size limit (HISTORY_FILE_MAX)
Save readline history to the HISTORY_FILE.
Called when the shell is about to exit.
*/
void	save_history(void)
{
	int			fd;
	HIST_ENTRY	**hist_list;
	int			i;
	int			history_count;
	int			excess_lines;
	int			saved_count = 0;

	fd = open(HISTORY_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("bleshell: error opening history file");
		return ;
	}
	hist_list = history_list();
	if (!hist_list)
	{
		close(fd);
		return ;
	}
	history_count = history_length;
	excess_lines = history_count - HISTORY_FILE_MAX;
	if (excess_lines > 0)
		i = excess_lines;
	else
		i = 0;
	while (i < history_count && hist_list[i])
	{
		if (hist_list[i]->line && *hist_list[i]->line)
		{
			saved_count++;
			write(fd, hist_list[i]->line, strlen(hist_list[i]->line));
			write(fd, "\n", 1);
		}
		i++;
	}
	close(fd);
}
