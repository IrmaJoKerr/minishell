/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 01:14:22 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 13:04:49 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Determines if current token should be a command or argument.
- Sets token as command if it's first in the list
- Sets token as command if it follows a pipe
- Sets token as command if it follows a redirection in a pipe
- Otherwise sets token as argument
Called by set_token_type() during tokenization.
*/
void	setpipe(t_vars *vars)
{
	if (!vars->head || vars->prev_type == TYPE_PIPE)
		vars->curr_type = TYPE_CMD;
	else if (vars->pipes && vars->pipes->in_pipe == 1
		&& is_redirection(vars->prev_type))
		vars->curr_type = TYPE_CMD;
	else
		vars->curr_type = TYPE_ARGS;
}

/*
Sets appropriate token type based on position and context.
Determines if current token should be a command or argument.
Updates vars->curr_type accordingly.
*/
void	set_token_type(t_vars *vars, char *input)
{
	int			moves;
	t_tokentype	special_type;
	static int	token_position;

	token_position = 0;
	special_type = 0;
	if (input && input[0] == '$')
		vars->curr_type = TYPE_EXPANSION;
	else if (input && *input)
	{
		special_type = get_token_at(input, 0, &moves);
		if (special_type != 0)
			vars->curr_type = special_type;
		else
			setpipe(vars);
	}
	else
	{
		if (!vars->head || vars->prev_type == TYPE_PIPE)
			vars->curr_type = TYPE_CMD;
		else
			vars->curr_type = TYPE_ARGS;
	}
	token_position++;
}

/*
Process operators and update token state
- Handles heredoc operators specially
- Processes other operators through process_operator_char
- Updates state for next token processing
Returns:
- 1 on success
- 0 on error
*/
int	proc_opr_token(char *input, t_vars *vars, int *hd_is_delim,
		t_tokentype token_type)
{
	handle_text(input, vars);
	if (token_type == TYPE_HEREDOC)
	{
		maketoken("<<", TYPE_HEREDOC, vars);
		vars->pos += 2;
		vars->start = vars->pos;
		*hd_is_delim = 1;
	}
	else if (!process_operator_char(input, &vars->pos, vars))
		return (0);
	vars->next_flag = 1;
	return (1);
}

/*
Finalize tokenization process
- Check for unterminated heredoc
- Process any remaining text
- Clean up and return status
Returns:
- 1 on success
- 0 on error
*/
int	finish_tokenizing(char *input, t_vars *vars, int hd_is_delim)
{
	if (hd_is_delim)
	{
		tok_syntax_error_msg("newline", vars);
		return (0);
	}
	handle_text(input, vars);
	return (1);
}

/*
Handle quote tokens preserving adjacency state
- Restores saved adjacency state before quote processing
- Processes quoted text through tokenize_quote
Returns:
- 1 on success
*/
int	handle_quotes(char *input, t_vars *vars, int *adj_saved)
{
	ft_memcpy(vars->adj_state, adj_saved, sizeof(int) * 3);
	tokenize_quote(input, vars);
	return (1);
}
