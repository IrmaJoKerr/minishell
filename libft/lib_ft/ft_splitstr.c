/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_splitstr.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 00:23:51 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 03:36:57 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_split allocates (with malloc(3)) and returns an array of strings obtained
by splitting ’s’ using the string of characters ’delimiters’ as a delimiter.
The function loops through the delimiter string, checking if any of the
characters in the string matches the character in the string ’s’. If a match
occurs for any of the characters in the delimiter string, the function will
consider the character in ’s’ as a delimiter.The array ends with a 
NULL pointer.
*/

#include "libft.h"

static int	is_delimiter(char c, char *delimiters)
{
	while (*delimiters)
	{
		if (c == *delimiters)
			return (1);
		delimiters++;
	}
	return (0);
}

static size_t	count_substr(char const *s, char *delimiters)
{
	size_t	count;
	int		in_substr;

	count = 0;
	in_substr = 0;
	while (*s != '\0')
	{
		if (is_delimiter(*s, delimiters))
			in_substr = 0;
		else if (in_substr == 0)
		{
			in_substr = 1;
			count++;
		}
		s++;
	}
	return (count);
}

static void	free_splits(char **splits, size_t i)
{
	while (i > 0)
	{
		free(splits[--i]);
	}
	free(splits);
}

static char	*find_next_substring(char **start, char *delimiters)
{
	char	*end;
	char	*substring;
	size_t	len;
	size_t	i;

	while (is_delimiter(**start, delimiters))
		(*start)++;
	if (**start == '\0')
		return (NULL);
	end = *start;
	while (*end && !is_delimiter(*end, delimiters))
		end++;
	len = end - *start;
	substring = malloc(len + 1);
	if (!substring)
		return (NULL);
	i = 0;
	while (i < len)
	{
		substring[i] = (*start)[i];
		i++;
	}
	substring[len] = '\0';
	*start = end;
	return (substring);
}

char	**ft_splitstr(char const *s, char *delimiters)
{
	char	**splits;
	char	*start;
	size_t	substr_count;
	size_t	i;

	if (!s || !delimiters)
		return (NULL);
	substr_count = count_substr(s, delimiters);
	splits = malloc((substr_count + 1) * sizeof(char *));
	if (!splits)
		return (NULL);
	i = 0;
	start = (char *)s;
	while (i < substr_count)
	{
		splits[i] = find_next_substring(&start, delimiters);
		if (!splits[i])
		{
			free_splits(splits, i);
			return (NULL);
		}
		i++;
	}
	splits[i] = NULL;
	return (splits);
}
