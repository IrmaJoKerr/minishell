/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 02:41:39 by bleow             #+#    #+#             */
/*   Updated: 2025/04/07 11:31:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void handle_input(char *input, t_vars *vars)
{
    char    **cmdarray;
    int     i;
    
    i = 0;
    if (!input || !*input)
        return;
        
    // Check if this is multi-line input
    if (ft_strchr(input, '\n'))
    {
        //DBG_PRINTF(1, "Multi-line input detected\n");
        
        // Split input into lines
        cmdarray = ft_split(input, '\n');
        if (!cmdarray)
            return;
            
        int line_count = ft_arrlen(cmdarray);
        
        // Check if the first line contains a heredoc marker
        if (line_count > 1 && ft_strnstr(cmdarray[0], "<<"
			,ft_strlen(cmdarray[0])))
        {
            // Clean up any previously stored heredoc lines
            if (vars->heredoc_lines)
    			ft_free_2d(vars->heredoc_lines, vars->heredoc_count);
            
            // Store the remaining lines for heredoc processing
            vars->heredoc_count = line_count - 1;
            vars->heredoc_lines = malloc(sizeof(char*) * vars->heredoc_count);
            vars->heredoc_index = 0;
            
            if (vars->heredoc_lines)
            {
                //DBG_PRINTF(1, "Storing %d lines for heredoc\n", vars->heredoc_count);
                for (i = 0; i < vars->heredoc_count; i++)
                {
                    vars->heredoc_lines[i] = ft_strdup(cmdarray[i+1]);
                    //DBG_PRINTF(1, "  Stored: '%s'\n", vars->heredoc_lines[i]);
                }
            }
            
            // Process the first command (with the heredoc marker)
            //DBG_PRINTF(1, "Processing heredoc command: '%s'\n", cmdarray[0]);
            add_history(cmdarray[0]);
            process_command(cmdarray[0], vars);
        }
        else
        {
            // Normal multi-line processing (each line as separate command)
            for (i = 0; i < line_count; i++)
            {
                if (*cmdarray[i])
                {
                    //DBG_PRINTF(1, "Processing command: '%s'\n", cmdarray[i]);
                    add_history(cmdarray[i]);
                    process_command(cmdarray[i], vars);
                }
            }
        }
        
        ft_free_2d(cmdarray, line_count);
    }
    else
    {
        // Single-line input
        //DBG_PRINTF(1, "Single-line input: '%s'\n", input);
        process_command(input, vars);
    }
}
