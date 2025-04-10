/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 23:05:01 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Skips whitespace characters in the input string.
- Moves past spaces, tabs, and newlines.
- Updates vars->pos to point to the next non-whitespace.
Returns:
Nothing (void function).
Works with lexerlist() to handle token separation.

Example: Input "  echo hello"
- Skips the two spaces at the beginning
- Sets vars->pos to 2 (pointing at 'e')
*/
void	skip_whitespace(char *str, t_vars *vars)
{
	while (str[vars->pos] && (str[vars->pos] == ' '
			|| str[vars->pos] == '\t' || str[vars->pos] == '\n'))
		vars->pos++;
}
