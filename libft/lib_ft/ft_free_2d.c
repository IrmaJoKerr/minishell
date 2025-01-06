/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_2d.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 05:28:06 by bleow             #+#    #+#             */
/*   Updated: 2024/11/08 12:15:22 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
Generic cleanup function to free a 2d array.
*/

#include "libft.h"

void	ft_free_2d(char **splits, size_t len)
{
	size_t	j;

	j = 0;
	while (j < len)
	{
		free(splits[j]);
		j++;
	}
	free(splits);
}
