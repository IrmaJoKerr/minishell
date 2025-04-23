/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 19:09:31 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Combines original input with continuation input
Returns:
- A newly allocated string with both inputs joined.
- Null on error.
*/
char	*append_input(char *original, char *additional)
{
	char	*temp;
	char	*result;
	
	temp = ft_strjoin(original, "\n");
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, additional);
	free(temp);
	return (result);
}
