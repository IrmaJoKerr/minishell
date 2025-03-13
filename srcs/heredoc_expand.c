/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:26:48 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 02:54:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Add chunk to string and frees old string. Works with expand_heredoc_line().
Example: str = "Hello", chunk = " World" -> new_str = "Hello World"
*/
char	*join_and_free(char *str, char *chunk)
{
	char	*new_str;

	if (!chunk)
		return (str);
	if (!str)
		return (ft_strdup(chunk));
	new_str = ft_strjoin(str, chunk);
	ft_safefree((void **)&str);
	return (new_str);
}

/*
Extract and expand a variable from the line in heredoc.
Remembers the position of the variable in the line.
Continues expanding until end of variable name.
Example: $HOME -> /Users/bleow
*/
char	*expand_var_heredoc(char *line, int *pos, t_vars *vars)
{
	char	*expanded;
	int		old_pos;

	old_pos = *pos;
	expanded = handle_expansion(line, pos, vars);
	if (!expanded)
	{
		*pos = old_pos + 1;
		return (ft_strdup("$"));
	}
	return (expanded);
}

/*
Reads a string until $ or end of string in heredoc.
Returns substring minus the expansion variable.
Example: Input: "Hello $HOME"
		 Output:"Hello "
*/
char	*read_string(char *line, int *pos)
{
	int	start;

	start = *pos;
	while (line[*pos] && line[*pos] != '$')
		(*pos)++;
	if (*pos > start)
		return (ft_substr(line, start, *pos - start));
	return (NULL);
}

/*
Process a single segment of a heredoc line - either a variable expansion
or a regular text segment. Updates position pointer and appends the 
processed segment to the result string.
This function is called in a loop by expand_heredoc_line() to process
the entire line segment by segment.
Example with input "Hello $HOME" (11 characters):
- 1st call: Reads "Hello " until '$'. Updates position from 0 to 6.
- 2nd call: At '$', expands "$HOME" to "/Users/bleow". Updates position 
  from 6 to 11.
Each call processes just one segment and returns the result with that segment
appended.
*/
char	*expand_one_line(char *line, int *pos, t_vars *vars, char *result)
{
	char	*segment;

	if (line[*pos] == '$')
	{
		segment = expand_var_heredoc(line, pos, vars);
		result = join_and_free(result, segment);
		ft_safefree((void **)&segment);
	}
	else
	{
		segment = read_string(line, pos);
		result = join_and_free(result, segment);
		ft_safefree((void **)&segment);
	}
	return (result);
}

/*
Main function for expanding environment variables in heredoc.
Processes a heredoc line character by character, expanding any variables.
Works by:
1) Allocating an empty char *result string
2) Calling expand_one_line() in while loop to process each segment
3) Building the final expanded string segment by segment
Each call processes either:
- A variable (starting with $)
- A regular text segment (up to the next $ or end of line)
Example with input "Hello $USER world":
- Start: result = NULL, pos = 0
- 1st segment: "Hello ", appends to result.
- 2nd segment: "$USER", expands to /Users/bleow, appends to result.
- 3rd segment: " world", appends to result
- Returns: "Hello /Users/bleow world"
Returns empty string for NULL inputs or if no successful expansions.
All returned strings are newly allocated and must be freed by caller.
*/
char	*expand_heredoc_line(char *line, t_vars *vars)
{
	int		pos;
	char	*result;

	if (!line || !vars)
		return (ft_strdup(""));
	result = NULL;
	pos = 0;
	while (line[pos])
		result = expand_one_line(line, &pos, vars, result);
	if (!result)
		return (ft_strdup(""));
	return (result);
}
