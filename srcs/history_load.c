/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_load.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:48:27 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 17:17:44 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Reads history lines from file descriptor and adds to readline history.
- Gets lines one by one using get_next_line().
- Removes trailing newlines from each history entry.
- Adds non-empty lines to readline's history memory.
- Properly frees memory for each processed line.
Returns:
Nothing (void function).
Works with load_history() after skip_history_lines().

Example: After skipping excess lines
- Reads remaining lines like "ls -la", "cd .."
- Adds each valid entry to readline history
- Stops at EOF, leaving history populated with latest commands
*/
void	read_history_lines(int fd)
{
	char	*line;
	size_t	len;

	line = get_next_line(fd);
	while (line)
	{
		if (*line)
		{
			len = ft_strlen(line);
			if (len > 0 && line[len - 1] == '\n')
				line[len - 1] = '\0';
			add_history(line);
		}
		free(line);
		line = get_next_line(fd);
	}
}

/*
Loads command history from file into readline's history memory.
- Counts total history entries to determine if skipping is needed.
- Skips older entries if count exceeds HIST_MEM_MAX limit.
- Reads remaining entries into readline history memory.
- Handles file opening/closing and empty history cases.
Returns:
Nothing (void function).
Works with init_shell() during program startup.

Example: For a shell with HIST_MEM_MAX=1000
- Checks history file size
- If 1500 entries exist, skips first 500
- Loads most recent 1000 entries into memory
- Returns without action if history is empty or inaccessible
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
	skip_lines(fd, skip);
	read_history_lines(fd);
	close(fd);
}
