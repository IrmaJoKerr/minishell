/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:53 by bleow             #+#    #+#             */
/*   Updated: 2025/02/10 14:50:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Start point for the minishell program.
*/

static char	*reader(void)
{
    char	*line;

    line = readline("bleshell$ ");
    if (!line)
    {
        ft_putendl_fd("exit", STDOUT_FILENO);
        exit(0);
    }
    if (*line)
        add_history(line);
    return (line);
}

void save_history(void)
{
    int fd;
    HIST_ENTRY **hist_list;
    int i;

    fd = open(HISTORY_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return;
    hist_list = history_list();
    if (hist_list)
    {
        for (i = 0; hist_list[i]; i++)
        {
            write(fd, hist_list[i]->line, strlen(hist_list[i]->line));
            write(fd, "\n", 1);
        }
    }
    close(fd);
}

char *read_command(void)
{
    char *line;
    
    line = readline(PROMPT);
    if (line && *line)
        add_history(line);
    return (line);
}

void sigint_handler(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void sigquit_handler(int sig)
{
    (void)sig;
    rl_on_new_line();
    rl_redisplay();
}

void load_history(void)
{
    int fd;
    char buf[4096];
    char line[1024];
    ssize_t bytes;
    int i = 0;
    int j = 0;

    if (access(HISTORY_FILE, R_OK) == -1)
        return;
    fd = open(HISTORY_FILE, O_RDONLY);
    if (fd == -1)
        return;
    while ((bytes = read(fd, buf, sizeof(buf))) > 0)
    {
        while (i < bytes)
        {
            if (buf[i] == '\n' || j == sizeof(line) - 1)
            {
                line[j] = '\0';
                if (j > 0)
                    add_history(line);
                j = 0;
            }
            else
                line[j++] = buf[i];
            i++;
        }
        i = 0;
    }
    if (j > 0)
    {
        line[j] = '\0';
        add_history(line);
    }
    close(fd);
}

int main(int ac, char **av, char **envp)
{
    t_vars vars;
    char *command;
    
    ft_memset(&vars, 0, sizeof(t_vars));
    vars.env = envp;
    
    load_history();
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);

    while (1)
    {
        command = read_command();
        if (!command)
            break;
        // Add lexer integration
        lexerlist(command, &vars);
        free(command);
    }
    save_history();
    return (0);
}
