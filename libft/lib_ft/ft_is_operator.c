/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_is_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 08:17:47 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 14:32:28 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_is_operator function checks if the given character is a valid operator
*/

#include "libft.h"

int	ft_is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	else
		return (0);
}
