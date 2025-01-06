/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 10:27:06 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 03:49:46 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
get_next_line is a function that reads a file line by line.
*/

#include "get_next_line.h"

char	*ft_newline(char *last_pos)
{
	int		i;
	char	*str;

	i = 0;
	if (!last_pos[i])
		return (NULL);
	while (last_pos[i] && last_pos[i] != '\n')
		i++;
	str = (char *)malloc(sizeof(char) * (i + 2));
	if (!str)
		return (NULL);
	i = 0;
	while (last_pos[i] && last_pos[i] != '\n')
	{
		str[i] = last_pos[i];
		i++;
	}
	if (last_pos[i] == '\n')
	{
		str[i] = last_pos[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	*ft_trim(char *last_pos)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	while (last_pos[i] && last_pos[i] != '\n')
		i++;
	if (!last_pos[i])
	{
		free(last_pos);
		return (NULL);
	}
	str = (char *)malloc(sizeof(char) * (ft_strlen(last_pos) - i + 1));
	if (!str)
	{
		free(str);
		return (NULL);
	}
	i++;
	j = 0;
	while (last_pos[i])
		str[j++] = last_pos[i++];
	str[j] = '\0';
	free(last_pos);
	return (str);
}

char	*ft_read_fd(int fd, char *last_pos)
{
	char	*buffer;
	int		bytes_read;

	bytes_read = 1;
	buffer = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!buffer)
		return (NULL);
	if (NULL == last_pos)
		last_pos = ft_strdup("");
	while (!ft_strchr(last_pos, '\n') && bytes_read != 0)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1)
		{
			free(buffer);
			free(last_pos);
			return (NULL);
		}
		buffer[bytes_read] = '\0';
		last_pos = ft_gstrjoin(last_pos, buffer);
	}
	free(buffer);
	return (last_pos);
}

char	*get_next_line(int fd)
{
	static char		*last_pos[MAX_FD];
	char			*release_str;

	if (fd < 0 || BUFFER_SIZE <= 0 || fd > MAX_FD)
		return (NULL);
	last_pos[fd] = ft_read_fd(fd, last_pos[fd]);
	if (last_pos[fd] == NULL)
		return (NULL);
	release_str = ft_newline(last_pos[fd]);
	last_pos[fd] = ft_trim(last_pos[fd]);
	return (release_str);
}
