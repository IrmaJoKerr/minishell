/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiline_validation.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:32:42 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 00:36:34 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Tokenizes the first line of input.
- Temporarily null-terminates the input at nl_ptr.
- Tokenizes and restores the newline.
Returns:
- 1 on successful tokenization (syntax check separate).
- 0 on failure.
*/
int	tokenize_first_line(char *input, char *nl_ptr, t_vars *vars)
{
	int		result;
	char	original_char;

	original_char = *nl_ptr;
	*nl_ptr = '\0';
	result = tokenizer(input, vars);
	*nl_ptr = original_char;
	return (result);
}

/*
Checks for non-whitespace characters immediately after the raw delimiter.
Returns:
- 1 if only whitespace (or end of string) follows.
- 0 otherwise.
*/
int	chk_hd_tail(char *line_start, char *raw_delim_ptr, char *delim,
				t_vars *vars)
{
	int		pos_after;
	char	current_char;

	pos_after = (raw_delim_ptr - line_start) + ft_strlen(delim);
	current_char = line_start[pos_after];
	while (current_char != '\0' && current_char != '\n')
	{
		if (!ft_isspace(current_char))
		{
			tok_syntax_error_msg("newline", NULL);
			return (0);
		}
		pos_after++;
		current_char = line_start[pos_after];
		vars->error_code = ERR_SYNTAX;
	}
	return (1);
}

/*
Finds the raw delimiter string in the line and handles the "not found" error.
Returns
- Pointer to the raw delimiter if found.
- NULL otherwise (error handled).
*/
char	*chk_raw_delim(char *line_start, int len, char *delim_arg, t_vars *vars)
{
	char	*raw_delim_ptr;
	char	original_char;

	raw_delim_ptr = find_raw_delim(line_start, len, delim_arg);
	if (!raw_delim_ptr)
	{
		original_char = line_start[len];
		line_start[len] = '\n';
		line_start[len] = original_char;
		vars->error_code = ERR_DEFAULT;
		return (NULL);
	}
	return (raw_delim_ptr);
}
