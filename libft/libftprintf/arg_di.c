/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_di.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/26 21:44:54 by bleow             #+#    #+#             */
/*   Updated: 2024/09/24 21:11:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/ft_printf.h"

void	ft_arg_di(int num, unsigned int *printed)
{
	char	*convert;

	convert = ft_itoa(num);
	if (convert == NULL)
	{
		*printed += write(1, "(null)", 6);
		return ;
	}
	*printed += ft_putstr_rtn(convert, 1);
	free (convert);
}
