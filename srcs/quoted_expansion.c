/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quoted_expansion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:47:36 by bleow             #+#    #+#             */
/*   Updated: 2025/11/17 14:54:44 by bleow            ###   ########.fr       */
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
char	*expand_quoted_str(char *content, t_vars *vars)
{
	char	*expanded;
	int		pos;

	pos = 0;
	expanded = ft_strdup("");
	if (!expanded)
		return (NULL);
	/* Debug: initial state */
	fprintf(stderr, "DEBUG QE: expand_quoted_str content='%s' quote_depth=%d\n", content, vars->quote_depth);
	while (content[pos])
	{
		if (content[pos] == '$')
		{
			fprintf(stderr, "DEBUG QE: expand_quoted_str encountering $ at pos=%d\n", pos);
			expanded = expand_quoted_var(expanded, content, &pos, vars);
			if (!expanded)
				return (NULL);
			fprintf(stderr, "DEBUG QE: after var expansion expanded='%s'\n", expanded);
		}
		else
		{
			expanded = append_basic_strs(expanded, content, &pos);
			if (!expanded)
				return (NULL);
			fprintf(stderr, "DEBUG QE: after append_basic_strs expanded='%s'\n", expanded);
		}
	}
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
char	*expand_quoted_var(char *expanded, char *content, int *pos,
			t_vars *vars)
{
	char	*var_value;
	char	*temp;

	var_value = expand_variable(content, pos, NULL, vars);
	if (!var_value)
		return (expanded);
	fprintf(stderr, "DEBUG QE: expand_quoted_var got var_value='%s' at pos=%d\n", var_value, *pos);
	temp = ft_strjoin(expanded, var_value);
	if (temp)
		fprintf(stderr, "DEBUG QE: expand_quoted_var combined expanded='%s'\n", temp);
	ft_safefree((void **)&expanded);
	ft_safefree((void **)&var_value);
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
	if (*pos > start)
	{
		fprintf(stderr, "DEBUG QE: append_basic_strs start=%d end=%d len=%d\n", start, *pos, (*pos - start));
		return (append_substr(expanded, content, start, *pos - start));
	}
	return (NULL);
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
		ft_safefree((void **)&dest);
		return (NULL);
	}
	result = ft_strjoin(dest, chunk);
	ft_safefree((void **)&dest);
	ft_safefree((void **)&chunk);
	return (result);
}
