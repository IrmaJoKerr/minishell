/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expansion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:30:56 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 23:37:38 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Expands variables in a heredoc line
Returns:
- Newly allocated string with variables expanded.
- Empty string on NULL input or if no expansions/error.
*/
char	*hd_expander(char *line, t_vars *vars)
{
	int		pos;
	char	*result;
	char	*segment;

	if (!line || !vars)
		return (ft_strdup(""));
	result = NULL;
	pos = 0;
	while (line[pos])
	{
		if (line[pos] == '$')
		{
			segment = expand_hd_var(line, &pos, vars);
			result = merge_and_free(result, segment);
		}
		else
		{
			segment = read_hd_str(line, &pos);
			result = merge_and_free(result, segment);
		}
		free(segment);
	}
	if (!result)
		return (ft_strdup(""));
	return (result);
}

/*
Prepares a line for heredoc by conditionally expanding variables.
- Expands variables if hd_expand flag is set
- Creates a simple copy otherwise
Returns:
- Newly allocated string with expanded or copied content.
- NULL on memory allocation failure.
*/
char	*prepare_hd_line(char *line, t_vars *vars)
{
	char	*result;

	if (!line || !vars || !vars->pipes)
		return (NULL);
	if (vars->pipes->hd_expand == 1)
		result = hd_expander(line, vars);
	else
		result = ft_strdup(line);
	return (result);
}

/*
Extracts and expands a variable from heredoc line.
- Processes a variable starting with $ character.
- Falls back to returning "$" if expansion fails.
Returns:
Newly allocated string with expanded variable.
"$" if variable can't be expanded.
Works with expand_one_line().

Example: At position of "$HOME" in a line
- Returns: "/Users/username" (expanded value)
- Updates position to after variable name
*/
char	*expand_hd_var(char *line, int *pos, t_vars *vars)
{
	char	*expanded;
	int		old_pos;

	old_pos = *pos;
	expanded = expand_variable(line, pos, NULL, vars);
	if (!expanded)
	{
		*pos = old_pos + 1;
		expanded = ft_strdup("$");
		return (expanded);
	}
	return (expanded);
}

/*
Extracts a regular text string up to $ character.
- Processes text segment from current position to next $.
- Creates substring of this text segment.
Returns:
- Newly allocated substring from start to next variable.
- NULL if no characters to extract.
Works with expand_one_line().

Example: At start of "Hello $USER"
- Returns: "Hello "
- Updates position to the $ character
*/
char	*read_hd_str(char *line, int *pos)
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
Writes line to heredoc fd (TMP_BUF) with variable expansion
based on vars->pipes->hd_expand.
Returns:
- 1 on success.
- 0 on failure.
*/
int	write_to_hd(int fd, char *line, t_vars *vars)
{
	char	*expanded_line;
	int		write_result;

	if (!line || fd < 0 || !vars || !vars->pipes)
		return (0);
	expanded_line = prepare_hd_line(line, vars);
	if (!expanded_line)
		return (0);
	write_result = write(fd, expanded_line, ft_strlen(expanded_line));
	if (write_result != -1)
		write(fd, "\n", 1);
	free(expanded_line);
	if (write_result == -1)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	else
		return (1);
}
