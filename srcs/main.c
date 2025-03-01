/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:53 by bleow             #+#    #+#             */
/*   Updated: 2025/03/01 08:03:28 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Function to read a line from the user.
*/
char	*reader(void)
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

/*
Start point for the minishell program.
*/
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
