/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_2d.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 05:28:06 by bleow             #+#    #+#             */
/*   Updated: 2025/06/07 01:31:57 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_free_2d() function is a cleanup function to free a 2d char array.
*/

#include "libft.h"

void	ft_free_2d(char **arr, size_t count)
{
	size_t	i;

	if (!arr)
		return ;
	i = 0;
	while (i < count && arr[i])
	{
		ft_safefree((void **)&arr[i]);
		i++;
	}
	ft_safefree((void **)&arr);
}
