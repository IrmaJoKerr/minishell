/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 22:59:29 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check if a command is complete.
Returns 1 if complete, 0 if more input is needed.
Complete means no unclosed quotes and no dangling pipes.
*/
int	is_input_complete(t_vars *vars)
{
	t_node	*current;
	int		expecting_command;

	expecting_command = 0;
	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			expecting_command = 1;
		}
		else if (expecting_command && current->type != TYPE_ARGS
			&& current->type != TYPE_CMD)
		{
			expecting_command = 1;
		}
		else if (expecting_command)
		{
			expecting_command = 0;
		}
		current = current->next;
	}
	return (!expecting_command);
}

/*
Combines original input with continuation input
Returns a newly allocated string with both inputs joined
*/
char *append_input(char *original, char *additional)
{
	char *temp;
	char *result;
	
	// First join with a newline
	temp = ft_strjoin(original, "\n");
	if (!temp)
		return (NULL);
	
	// Then add the additional input
	result = ft_strjoin(temp, additional);
	free(temp);
	
	return (result);
}
