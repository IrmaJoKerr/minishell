/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_save.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 14:35:22 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 06:54:27 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Prepares history entries for saving, handling excess lines.
- Retrieves history entries from readline's memory.
- Calculates start index based on history limit.
- Returns all info needed for saving entries.
Returns:
- Start index for entry saving
- History list via parameter
- Total count via parameter
- 1 on success, 0 on failure
Works with save_history().
*/
int	prepare_history_entries(HIST_ENTRY ***hist_list, int *history_count
					, int *start_idx)
{
	int	excess_lines;

	*hist_list = history_list();
	if (!*hist_list)
		return (0);
	*history_count = history_length;
	excess_lines = *history_count - HISTORY_FILE_MAX;
	*start_idx = 0;
	if (excess_lines > 0)
		*start_idx = excess_lines;
	return (1);
}

/*
Saves readline history entries to history file.
- Opens history file for writing.
- Retrieves history entries from readline's memory.
- Skips excess entries if count exceeds HISTORY_FILE_MAX.
- Writes valid entries to history file with newlines.
- Logs details of the save operation for debugging.
Works with cleanup_exit() during shell termination.

Example: When shell exits with 1500 history entries and HISTORY_FILE_MAX=1000
- Opens history file for writing (creating if needed)
- Calculates excess entries (500)
- Skips oldest 500 entries
- Writes newest 1000 entries to history file
- Logs success with number of entries saved
*/
void	save_history(void)
{
	int			fd;
	HIST_ENTRY	**hist_list;
	int			history_count;
	int			start_idx;
	int			saved_count;

	if (!chk_and_make_folder("temp"))
		return ;
	fd = open(HISTORY_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return ;
	if (!prepare_history_entries(&hist_list, &history_count, &start_idx))
	{
		close(fd);
		return ;
	}
	saved_count = save_history_entries(fd, hist_list, start_idx,
			history_count);
	close(fd);
	if (saved_count > HISTORY_FILE_MAX)
	{
		trim_history(saved_count - HISTORY_FILE_MAX);
	}
}

/*
Writes history entries to the history file.
- Iterates through history entries starting at specified index.
- Writes valid entries to file with newlines.
- Tracks number of entries saved for debugging.
- Logs detailed save information for troubleshooting.
Returns:
Number of entries successfully saved.
Works with save_history().

Example: For 1000 history entries after skipping 200
- Iterates through entries 200-999
- Writes each valid entry with a newline
- Logs progress during save operation
- Returns total count of entries saved (should be 800)
*/
int	save_history_entries(int fd, HIST_ENTRY **hist_list, int start, int total)
{
	int	i;
	int	saved_count;

	saved_count = 0;
	i = start;
	while (i < total && hist_list[i])
	{
		if (hist_list[i]->line && *(hist_list[i]->line))
		{
			saved_count++;
			write(fd, hist_list[i]->line, strlen(hist_list[i]->line));
			write(fd, "\n", 1);
		}
		i++;
	}
	return (saved_count);
}
