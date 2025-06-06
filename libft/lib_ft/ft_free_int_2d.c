/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_int_2d.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 00:24:49 by bleow             #+#    #+#             */
/*   Updated: 2025/06/02 13:20:45 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_free_2d_int() function is a cleanup function to free a 2D int array.
It iterates through the array, freeing each element, and then frees the array
itself.
*/
#include "libft.h"

/*
void	ft_free_int_2d(int **array, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		if (array[i])
			free(array[i]);
		i++;
	}
	free(array);
}
*/
void	ft_free_int_2d(int **arr, size_t count)
{
	size_t	i;

	if (!arr)
		return ;
	i = 0;
	while (i < count && arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
