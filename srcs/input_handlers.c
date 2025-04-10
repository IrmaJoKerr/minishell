/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 02:41:39 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 19:39:42 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// void	handle_input(char *input, t_vars *vars)
// {
//     char	**cmdarr;
//     int		i;
//     int		line_count;
	
//     i = 0;
//     if (!input || !*input)
//         return;
//     if (ft_strchr(input, '\n'))
//     {
//         DBG_PRINTF(1, "Multi-line input detected\n");
//         cmdarr = ft_split(input, '\n');
//         if (!cmdarr)
//             return;
//         line_count = ft_arrlen(cmdarr);
//         if (line_count > 1 && ft_strnstr(cmdarr[0], "<<", ft_strlen(cmdarr[0])))
//         {
//             if (vars->heredoc_lines)
//                 ft_free_2d(vars->heredoc_lines, vars->heredoc_count);
//             vars->heredoc_count = line_count - 1;
//             vars->heredoc_lines = malloc(sizeof(char*) * vars->heredoc_count);
//             vars->heredoc_index = 0;
//             if (vars->heredoc_lines)
//             {
//                 DBG_PRINTF(1, "Storing %d lines for heredoc\n", vars->heredoc_count);
//                 i = 0;
//                 while (i < vars->heredoc_count)
//                 {
//                     vars->heredoc_lines[i] = ft_strdup(cmdarr[i+1]);
//                     DBG_PRINTF(1, "  Stored: '%s'\n", vars->heredoc_lines[i]);
//                     i++;
//                 }
//             }
//             DBG_PRINTF(1, "Processing heredoc command: '%s'\n", cmdarr[0]);
//             add_history(cmdarr[0]);
//             process_command(cmdarr[0], vars);
//         }
//         else
//         {
//             i = 0;
//             while (i < line_count)
//             {
//                 if (*cmdarr[i])
//                 {
//                     DBG_PRINTF(1, "Processing command: '%s'\n", cmdarr[i]);
//                     add_history(cmdarr[i]);
//                     process_command(cmdarr[i], vars);
//                 }
//                 i++;
//             }
//         }
//         ft_free_2d(cmdarr, line_count);
//     }
//     else
//     {
//         DBG_PRINTF(1, "Single-line input: '%s'\n", input);
//         process_command(input, vars);
//     }
// }

/*
Processes heredoc input from command array
Stores heredoc lines in vars->heredoc_lines
Executes the heredoc command
*/
void	proc_heredoc_input(char **cmdarr, int line_count, t_vars *vars)
{
	int	i;

	// Free existing heredoc lines if any
	if (vars->heredoc_lines)
		ft_free_2d(vars->heredoc_lines, vars->heredoc_count);
	// Set up new heredoc storage
	vars->heredoc_count = line_count - 1;
	vars->heredoc_lines = malloc(sizeof(char*) * vars->heredoc_count);
	vars->heredoc_index = 0;
	// Store heredoc lines
	if (vars->heredoc_lines)
	{
		DBG_PRINTF(1, "Storing %d lines for heredoc\n", vars->heredoc_count);
		i = 0;
		while (i < vars->heredoc_count)
		{
			vars->heredoc_lines[i] = ft_strdup(cmdarr[i+1]);
			DBG_PRINTF(1, "  Stored: '%s'\n", vars->heredoc_lines[i]);
			i++;
		}
	}
	// Process the heredoc command
	DBG_PRINTF(1, "Processing heredoc command: '%s'\n", cmdarr[0]);
	add_history(cmdarr[0]);
	process_command(cmdarr[0], vars);
}

/*
Processes multiple command lines from command array
Adds commands to history and processes them
*/
void	proc_multiline_input(char **cmdarr, int line_count, t_vars *vars)
{
	int	i;
	
	i = 0;
	while (i < line_count)
	{
		if (*cmdarr[i])
		{
			DBG_PRINTF(1, "Processing command: '%s'\n", cmdarr[i]);
			add_history(cmdarr[i]);
			process_command(cmdarr[i], vars);
		}
		i++;
	}
}

/*
Handles user input and processes commands
Manages both single-line and multi-line inputs
Detects and processes heredoc inputs
*/
void	handle_input(char *input, t_vars *vars)
{
	char	**cmdarr;
	int		count;
	
	if (!input || !*input)
		return;
	if (ft_strchr(input, '\n'))
	{
		DBG_PRINTF(1, "Multi-line input detected\n");
		cmdarr = ft_split(input, '\n');
		if (!cmdarr)
			return ;
		count = ft_arrlen(cmdarr);
		if (count > 1 && ft_strnstr(cmdarr[0], "<<" , ft_strlen(cmdarr[0])))
			proc_heredoc_input(cmdarr, count, vars);
		else
		{
			proc_multiline_input(cmdarr, count, vars);
		}
		ft_free_2d(cmdarr, count);
	}
	else
	{
		DBG_PRINTF(1, "Single-line input: '%s'\n", input);
		process_command(input, vars);
	}
}
