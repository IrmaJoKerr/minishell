/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/04/06 09:40:30 by bleow            ###   ########.fr       */
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
Works with handle_unclosed_quotes().

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
