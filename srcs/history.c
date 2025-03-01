/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 06:29:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/01 07:55:44 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Function to check and initialise the history fd. 
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
Appends a line to the history file.
*/
static int	append_history(int fd, const char *line)
{
    if (!line)
        return (0);
    if (write(fd, line, ft_strlen(line)) == -1)
        return (0);
    if (write(fd, "\n", 1) == -1)
        return (0);
    return (1);
}

/*
Saves command history to HISTORY_FILE.
*/
void	save_history(void)
{
    int		fd;
    char	*line;

    fd = init_history_fd(O_WRONLY | O_CREAT | O_APPEND);
    if (fd == -1)
        return ;
    line = readline(PROMPT);
    if (line && *line)
    {
        append_history(fd, line);
        add_history(line);
    }
    close(fd);
    if (line)
        free(line);
}

/*
Loads command history from HISTORY_FILE.
*/
void	load_history(void)
{
    int		fd;
    char	*line;

    fd = init_history_fd(O_RDONLY);
    if (fd == -1)
        return ;
    while (get_next_line(fd, &line) > 0)
    {
        if (*line)
            add_history(line);
        free(line);
    }
    close(fd);
}
