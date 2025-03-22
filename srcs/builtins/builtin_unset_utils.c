/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 12:12:38 by lechan            #+#    #+#             */
/*   Updated: 2025/03/22 17:13:00 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Copy environment entries up to a specific positio.
Works with realloc_until_var.
*/
void	copy_env_front(char **src, char **dst, int pos)
{
	int	i;

	i = 0;
	while (i < pos)
	{
		dst[i] = src[i];
		i++;
	}
}

/*
Copy remaining environment entries with offset.
Works with realloc_until_var.
*/
void	copy_env_back(char **src, char **dst, int idx, int offset)
{
	int	i;
	int	j;

	i = idx;
	j = idx - offset;
	while (src[j])
	{
		dst[i] = src[j];
		i++;
		j++;
	}
	dst[i] = NULL;
}
