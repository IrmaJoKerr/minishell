/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quoted_expansion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:47:36 by bleow             #+#    #+#             */
/*   Updated: 2025/05/16 03:58:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Expands variables in a double-quoted string
Takes a string that may contain $VAR and expands them
Handles:
 - Character-by-character processing.
 - Variable expansion.
Returns:
 - A newly allocated string with variables expanded.
 - NULL on error.
Example: Input "$USER lives in $HOME"
 - Expands to "john lives in /home/john"
Works with process_quote_char().
*/
// char	*expand_quoted_str(char *content, t_vars *vars) PRE ADDING DEBUG PRINTS
// {
// 	char	*expanded;
// 	int		pos;

// 	pos = 0;
// 	expanded = ft_strdup("");
// 	if (!expanded)
// 		return (NULL);
// 	while (content[pos])
// 	{
// 		if (content[pos] == '$')
// 		{
// 			expanded = expand_quoted_var(expanded, content, &pos, vars);
// 			if (!expanded)
// 				return (NULL);
// 		}
// 		else
// 		{
// 			expanded = append_basic_strs(expanded, content, &pos);
// 			if (!expanded)
// 				return (NULL);
// 		}
// 	}
// 	return (expanded);
// }
char	*expand_quoted_str(char *content, t_vars *vars)
{
	char	*expanded;
	int		pos;

	fprintf(stderr, "DEBUG-QEXPAND: Processing quoted string: '%s'\n", content ? content : "NULL");
	pos = 0;
	expanded = ft_strdup("");
	if (!expanded)
		return (NULL);
	while (content[pos])
	{
		if (content[pos] == '$')
		{
			fprintf(stderr, "DEBUG-QEXPAND: Found $ at position %d\n", pos);
			expanded = expand_quoted_var(expanded, content, &pos, vars);
			if (!expanded)
				return (NULL);
		}
		else
		{
			expanded = append_basic_strs(expanded, content, &pos);
			if (!expanded)
				return (NULL);
		}
	}
	fprintf(stderr, "DEBUG-QEXPAND: Final expanded result: '%s'\n", expanded ? expanded : "NULL");
	return (expanded);
}

/*
Expands a variable and appends it to the result string
Return :
 - Updated result string.
 - Original string unchanged if variable expansion fails.
 - NULL on error.
Works with expand_quoted_str().
*/
// char	*expand_quoted_var(char *expanded, char *content, int *pos, PRE ADDING DEBUG PRINTS
// 				t_vars *vars)
// {
// 	char	*var_value;
// 	char	*temp;

// 	var_value = expand_variable(content, pos, NULL, vars);
// 	if (!var_value)
// 		return (expanded);
// 	temp = ft_strjoin(expanded, var_value);
// 	free(expanded);
// 	free(var_value);
// 	if (!temp)
// 		return (NULL);
// 	return (temp);
// }
char	*expand_quoted_var(char *expanded, char *content, int *pos, t_vars *vars)
{
	char	*var_value;
	char	*temp;

	fprintf(stderr, "DEBUG-QVAR: Found $ at position %d in '%s'\n", *pos, content + *pos);
	var_value = expand_variable(content, pos, NULL, vars);
	fprintf(stderr, "DEBUG-QVAR: Expansion result: '%s'\n", var_value ? var_value : "NULL");
	if (!var_value)
		return (expanded);
	temp = ft_strjoin(expanded, var_value);
	free(expanded);
	free(var_value);
	if (!temp)
		return (NULL);
	return (temp);
}

/*
Appends non-variable text to the expanded result string
Processes characters until a $ variable marker is found
Return:
 - Updated result string.
 - NULL on error.
Works with expand_quoted_str().
*/
char	*append_basic_strs(char *expanded, char *content, int *pos)
{
	int	start;

	start = *pos;
	while (content[*pos] && content[*pos] != '$')
		(*pos)++;
	expanded = append_substr(expanded, content, start, *pos - start);
	if (!expanded)
		return (NULL);
	return (expanded);
}

/*
Appends a substring to an existing string.
Handles:
 - Memory allocation for substring extraction.
 - String joining with proper cleanup.
Returns:
 - New concatenated string (caller must free).
 - NULL on error
Works with append_basic_strs().
*/
char	*append_substr(char *dest, char *src, int start, int len)
{
	char	*chunk;
	char	*result;

	if (!dest || !src)
		return (NULL);
	chunk = ft_substr(src, start, len);
	if (!chunk)
	{
		free(dest);
		return (NULL);
	}
	result = ft_strjoin(dest, chunk);
	free(dest);
	free(chunk);
	return (result);
}
