/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:13:52 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 18:08:39 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes operators and surrounding text in command input.
- Handles text before operators by creating string/command tokens.
- Detects and processes both single and double operators.
- Updates token start/end positions for proper tokenization.
Returns:
-New position index after operator processing.
Works with handle_token() during input tokenization.

Example: For input "echo hello > file"
- Processes "echo hello" as string/command when '>' is encountered
- Then processes '>' as an output redirection operator
- Returns position after '>' for continued processing
*/
int	operators(char *input, int i, int token_start, t_vars *vars)
{
	int	next_pos;

	if (i > token_start)
	{
		vars->start = token_start;
		vars->pos = i;
		vars->curr_type = TYPE_STRING;
		maketoken(input, vars);
		vars->start = i;
	}
	next_pos = handle_double_operator(input, i, vars);
	if (next_pos != i)
		return (next_pos);
	return (handle_single_operator(input, i, vars));
}

/*
Creates string token for text preceding an operator.
- Checks if current position contains operator character.
- Creates a string token from token_start to current position.
- Updates token tracking in vars structure.
Returns:
- Current position (unchanged).
Works with operators() for text segment tokenization.

Example: For "echo | grep" at position of '|'
- Creates string token "echo" before processing pipe
- Sets up vars for next token processing
*/
int	handle_string(char *input, int i, int token_start, t_vars *vars)
{
	if ((input[i] == '>' || input[i] == '<' || input[i] == '|')
		&& i > token_start)
	{
		vars->start = token_start;
		vars->pos = i;
		vars->curr_type = TYPE_STRING;
		maketoken(input, vars);
	}
	return (i);
}

/*
Maps operator characters to their token types.
- '|' == TYPE_PIPE
- '>' == TYPE_OUT_REDIRECT
- '<' == TYPE_IN_REDIRECT
Returns:
- Token type corresponding to operator character.
- TYPE_STRING as fallback for unrecognized characters.
Works with handle_single_operator() for type determination.

Example: For '|' character
- Returns TYPE_PIPE enum value
- Allows consistent token type assignment
Used with handle_single_operator().
*/
t_tokentype	get_operator_type(char op)
{
	if (op == '|')
		return (TYPE_PIPE);
	else if (op == '>')
		return (TYPE_OUT_REDIRECT);
	else if (op == '<')
		return (TYPE_IN_REDIRECT);
	else if (op == '$')
		return (TYPE_EXPANSION);
	return (TYPE_STRING);
}

/*
Processes single-character operators (|, >, <).
- Creates text token for content before operator if needed.
- Sets appropriate token type using get_operator_type().
- Creates operator token and updates position tracking.
Returns:
- Position after operator (i+1).
- Unchanged position if no operator.
Works with operators() during tokenization.

Example: For '>' character in input
- Creates TYPE_OUT_REDIRECT token
- Updates position to character after '>'
- Returns updated position
*/
int	handle_single_operator(char *input, int i, t_vars *vars)
{
	char	*token;

	if (i > vars->start)
		handle_string(input, i, vars->start, vars);
	vars->curr_type = get_operator_type(input[i]);
	token = ft_substr(input, i, 1);
	if (!token)
		return (i);
	maketoken(token, vars);
	vars->start = i + 1;
	return (i + 1);
}

/*
Processes two-character operators (>>, <<).
- Detects append redirect (>>) and heredoc (<<) operators.
- Creates appropriate token with TYPE_APPEND_REDIRECT or TYPE_HEREDOC.
- Updates token position tracking for continued processing.
Returns:
- Position after operator (i+2).
- Unchanged position if no match.
Works with operators() during tokenization.

Example: For ">>" in input
- Creates TYPE_APPEND_REDIRECT token
- Updates position to skip both characters
- Returns position after ">>"
*/
int	handle_double_operator(char *input, int i, t_vars *vars)
{
	char	*token;

	if (i > vars->start)
	{
		handle_string(input, i, vars->start, vars);
		vars->start = i;
	}
	token = ft_substr(input, i, 2);
	if (!token)
		return (i);
	if (input[i] == '>' && input[i + 1] == '>')
		vars->curr_type = TYPE_APPEND_REDIRECT;
	else if (input[i] == '<' && input[i + 1] == '<')
		vars->curr_type = TYPE_HEREDOC;
	else
		vars->curr_type = TYPE_STRING;
	maketoken(token, vars);
	ft_safefree((void **)&token);
	vars->start = i + 2;
	return (i + 2);
}

/*
Checks for double operators (>> or <<)
Returns 1 if found, 0 otherwise.
*/
int	is_double_operator(char *input, int i)
{
	if (!input || !input[i] || !input[i + 1])
		return (0);
	if ((input[i] == '>' && input[i + 1] == '>')
		|| (input[i] == '<' && input[i + 1] == '<'))
		return (1);
	return (0);
}

/*
Checks if a character is an operator (|, >, <)
Returns 1 if it is, 0 otherwise.
*/
int	is_operator(char c)
{
	if (c == '|' || c == '>' || c == '<')
		return (1);
	return (0);
}
