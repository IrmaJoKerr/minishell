/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:32:03 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 22:45:23 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles empty variable names in expansion.
- For non-NULL but empty var_name: Returns "$"
- For NULL var_name: Returns NULL
Returns:
- Newly allocated string with "$" when var_name exists but is empty.
- NULL when var_name is NULL.
Example:
empty_var("") -> "$"
empty_var(NULL) -> NULL
*/
char	*empty_var(char *var_name)
{
	if (var_name)
		return (ft_strdup("$"));
	else
		return (NULL);
}
