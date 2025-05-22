/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 09:52:41 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 17:29:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Reads and validates input from the continuation prompt.
- Displays the secondary prompt ("> ")
- Reads user input
- Trims whitespace
- Handles empty inputs by recursing
Returns:
- A valid, non-empty string that the caller must free
- NULL on critical error (like EOF)
*/
char	*read_until_complete(void)
{
	char	*input;
	char	*trimmed;

	input = readline("> ");
	if (!input)
		return (NULL);
	trimmed = ft_strtrim(input, " \t\n");
	free(input);
	if (!trimmed || trimmed[0] == '\0')
	{
		free(trimmed);
		return (read_until_complete());
	}
	return (trimmed);
}

/*
Appends new input to an existing command with a space separator.
- Joins the original command with a space
- Joins the result with the new input
- Frees original string and replaces it with combined result
Returns:
- 0 on success (*cmd_ptr will point to the new string)
- -1 on allocation error (original cmd_ptr preserved)
*/
int	append_to_cmdline(char **cmd_ptr, const char *addition)
{
	char	*tmp;
	char	*combined;

	tmp = ft_strjoin(*cmd_ptr, " ");
	if (!tmp)
		return (-1);
	combined = ft_strjoin(tmp, addition);
	free(tmp);
	if (!combined)
		return (-1);
	free(*cmd_ptr);
	*cmd_ptr = combined;
	return (0);
}
