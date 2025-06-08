/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_void_2d.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 00:21:47 by bleow             #+#    #+#             */
/*   Updated: 2025/06/07 01:31:57 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_free_void_2d() function frees a 2D array of void pointers.
It iterates through the array, freeing each element, and then frees the array
itself.
It is a generic cleanup function for freeing dynamically allocated 2D arrays.
(meaning it can be used with all data types eg char, int, etc.)
It is important to note that the function does not check for NULL pointers,
so it is assumed that the caller has already checked for NULL before calling
this function.
Example of use syntax:
ft_free_void_2d((void **)arrays, length);
*/
#include "libft.h"

void	ft_free_void_2d(void **array, size_t length)
{
	size_t	i;

	if (!array)
		return ;
	i = 0;
	while (i < length)
	{
		if (array[i])
			ft_safefree((void **)&array[i]);
		i++;
	}
	ft_safefree((void **)&array);
}
