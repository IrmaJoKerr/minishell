/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 07:13:09 by bleow            ###   ########.fr       */
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

/*
Prompts for additional input when input is incomplete.
- Used when quotes are unclosed or pipes need more input.
- Provides visual indicator of input continuation.
Returns:
Allocated string with user input or NULL if readline fails.
Works with handle_unclosed_quotes() and handle_unfinished_pipes().

Example: read_added_input("quote> ")
- Shows "quote> " prompt to user
- Returns user input as newly allocated string
*/
char	*read_added_input(char *prompt)
{
	char	*line;

	line = readline(prompt);
	if (!line)
		return (NULL);
	return (line);
}

/*
Processes text and adds to token list with optional type override.
- Creates token from current text segment.
- Applies standard or override token type.
- Updates token list and position markers.
Returns:
Nothing (void function).

Example: With first_token=1 and TYPE_CMD type
- Creates command token for current text segment
- Sets first_token to 0 after processing
*/
void	process_text(char *str, t_vars *vars
		, int *first_token, t_tokentype override_type)
{
	char	*token;
	int		len;

	len = vars->pos - vars->start;
	token = ft_substr(str, vars->start, len);
	if (!token)
		return ;
	if (override_type != TYPE_NULL)
		vars->curr_type = override_type;
	else if (*first_token || vars->prev_type == TYPE_PIPE)
	{
		vars->curr_type = TYPE_CMD;
		*first_token = 0;
	}
	else
		vars->curr_type = TYPE_ARGS;
	maketoken_with_type(token, vars->curr_type, vars);
	vars->start = vars->pos;
	vars->prev_type = vars->curr_type;
	free(token);
}

/*
Creates an operator token and adds to token list.
- Handles different operator types (pipe, redirections).
- Creates appropriate node based on operator type.
Returns:
Nothing (void function).
Works with handle_token().

Example: For | operator
- Creates TYPE_PIPE node
- Adds to token linked list
*/
void create_operator_token(t_vars *vars, t_tokentype type, char *symbol)
{
	t_node	*operator_node;

	operator_node = initnode(type, symbol);
	if (!operator_node)
		return ;
	vars->curr_type = type;
	build_token_linklist(vars, operator_node);
}
