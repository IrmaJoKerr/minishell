/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_2d.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 05:28:06 by bleow             #+#    #+#             */
/*   Updated: 2025/06/01 17:54:33 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_free_2d() function is a cleanup function to free a 2d char array.
*/

#include "libft.h"

void	ft_free_2d(char **array, size_t len)
{
	size_t	j;

	j = 0;
	while (j < len)
	{
		ft_safefree((void **)&array[j]);
		j++;
	}
	ft_safefree((void **)&array);
}
