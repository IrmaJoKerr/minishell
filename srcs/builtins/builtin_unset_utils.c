/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 12:12:38 by lechan            #+#    #+#             */
/*   Updated: 2025/06/01 07:07:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Copy environment entries up to a specific position.
Works with realloc_until_var.
*/
void	copy_env_front(char **src, char **dst, int pos)
{
	int	i;

	i = 0;
	while (i < pos)
	{
		dst[i] = ft_strdup(src[i]);
		i++;
	}
}

void	copy_env_add(char **src, char **dst, int pos)
{
	int	i;
	int	j;

	i = pos;
	j = i + 1;
	while (src[i])
		dst[j++] = ft_strdup(src[i++]);
	dst[j] = NULL;
}

void	copy_env_change(char **src, char **dst, int pos)
{
	int	i;
	int	j;

	i = pos;
	j = i;
	while (src[i])
		dst[j++] = ft_strdup(src[i++]);
	dst[j] = NULL;
}

void	copy_env_unset(char **src, char **dst, int pos)
{
	int	i;
	int	j;

	i = pos;
	j = i - 1;
	while (src[i])
		dst[j++] = ft_strdup(src[i++]);
	dst[j] = NULL;
}
