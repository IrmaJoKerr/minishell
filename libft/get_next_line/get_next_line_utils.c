/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 10:27:56 by bleow             #+#    #+#             */
/*   Updated: 2024/09/25 00:42:25 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
/*
char	*ft_strdup(const char *str)
{
	char	*dup;
	size_t	len;

	len = ft_strlen((char *)str);
	dup = malloc(len + 1);
	if (!dup || !str)
		return (NULL);
	ft_memcpy(dup, str, len);
	dup[len] = '\0';
	return (dup);
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	const char	*p_src;
	char		*p_dest;
	size_t		i;

	p_dest = (char *)dest;
	p_src = (const char *)src;
	i = 0;
	if (!p_dest && !p_src)
		return (NULL);
	while (i < n)
	{
		p_dest[i] = p_src[i];
		i++;
	}
	return (dest);
}

size_t	ft_strlen(char *str)
{
	char	*s;

	s = str;
	while (*s)
		s++;
	return (s - str);
}

char	*ft_strchr(const char *str, int character)
{
	unsigned char	c;

	c = (unsigned char)character;
	while (*str != '\0')
	{
		if (*str == c)
			return ((char *)str);
		str++;
	}
	if (c == '\0')
		return ((char *)str);
	return (NULL);
}
*/
char	*ft_gstrjoin(char *s1, char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*s3;

	if (!s1)
	{
		s1 = (char *)malloc(1 * sizeof(char));
		if (!s1)
			return (NULL);
		s1[0] = '\0';
	}
	if (!s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	s3 = malloc((len1 + len2 + 1) * sizeof(char));
	if (!s3)
		return (NULL);
	ft_memcpy(s3, s1, len1);
	ft_memcpy(s3 + len1, s2, len2);
	s3[len1 + len2] = '\0';
	free(s1);
	return (s3);
}
