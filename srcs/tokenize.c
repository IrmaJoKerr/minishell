/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 00:04:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets appropriate token type based on position and context.
Determines if current token should be a command or argument.
Updates vars->curr_type accordingly.
*/
void	set_token_type(t_vars *vars, char *input)
{
	int			moves;
	t_tokentype	special_type;

	special_type = 0;
	vars->prev_type = vars->curr_type;
	if (input && input[0] == '$')
	{
		vars->curr_type = TYPE_EXPANSION;
		return ;
	}
	if (input && *input)
	{
		special_type = get_token_at(input, 0, &moves);
		if (special_type != 0)
		{
			vars->curr_type = special_type;
			return ;
		}
	}
	if (!vars->head || vars->prev_type == TYPE_PIPE)
		vars->curr_type = TYPE_CMD;
	else
		vars->curr_type = TYPE_ARGS;
}



/*
Process any accumulated text before a special character.
Creates a token from the text between vars->start and vars->pos.
*/
void	handle_text(char *input, t_vars *vars)
{
	char	*token_preview;

	token_preview = NULL;
	if (vars->pos > vars->start)
	{
		token_preview = ft_substr(input, vars->start, vars->pos - vars->start);
		set_token_type(vars, token_preview);
		handle_string(input, vars);
		if (token_preview)
		{
			free(token_preview);
		}
		vars->start = vars->pos;
	}
}

/*
Processes quoted text in the input string.
- Detects if quote is a redirection target based on previous token
- Handles any accumulated text before the quote
- Calls process_quote_char to extract and process quoted content
- Sets next_flag if processing succeeds
- Restores original position on failure
*/
void	tokenize_quote(char *input, t_vars *vars)
{
	int	is_redir_target;
	int	saved_pos;

	is_redir_target = 0;
	if (vars->prev_type == TYPE_IN_REDIRECT
		|| vars->prev_type == TYPE_OUT_REDIRECT
		|| vars->prev_type == TYPE_APPEND_REDIRECT
		|| vars->prev_type == TYPE_HEREDOC)
	{
		is_redir_target = 1;
	}
	handle_text(input, vars);
	saved_pos = vars->pos;
	if (process_quote_char(input, vars, is_redir_target))
	{
		vars->next_flag = 1;
	}
	else
	{
		vars->pos = saved_pos;
	}
}

/*
Processes variable expansion tokens in the input string.
- Handles any accumulated text before the $ character
- Extracts and processes the expanded variable token
- Sets next_flag if expansion processing succeeds
- Updates position markers for continued tokenization
Triggered when $ character is encountered outside quotes.
*/
void	tokenize_expan(char *input, t_vars *vars)
{
	handle_text(input, vars);
	if (make_exp_token(input, vars))
		vars->next_flag = 1;
}

/*
Handle whitespace in the input.
Skips consecutive whitespace characters.
Sets next_flag if processing should continue from the loop.
*/
void	tokenize_white(char *input, t_vars *vars)
{
	handle_text(input, vars);
	while (input[vars->pos] && input[vars->pos] <= ' ')
	{
		vars->pos++;
	}
	vars->start = vars->pos;
	vars->next_flag = 1;
}

/*
Processes right-adjacent text for the current token.
- Extracts text between vars->start and vars->pos
- Joins this text with the last argument of the current token
- Handles memory allocation and cleanup
- Updates token content to include adjacent characters
Example: 
For "echo hello"world, joins "world" to "hello"
    to create a single argument "helloworld"
*/
void	handle_right_adj(char *input, t_vars *vars)
{
	char	*adjacent_text;
	char	*joined;
	int		arg_idx;

	if (vars->pos <= vars->start)
		return ;
	adjacent_text = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!adjacent_text)
		return ;
	if (vars->current && vars->current->args && vars->current->args[0])
	{
		arg_idx = 0;
		while (vars->current->args[arg_idx + 1])
			arg_idx++;
		joined = ft_strjoin(vars->current->args[arg_idx], adjacent_text);
		if (joined)
		{
			free(vars->current->args[arg_idx]);
			vars->current->args[arg_idx] = joined;
		}
	}
	free(adjacent_text);
}

/*
Initialize tokenizer state
- Resets position counters
- Clears quote depth
- Resets heredoc state
*/
void	init_tokenizer(t_vars *vars)
{
	vars->pos = 0;
	vars->start = 0;
	vars->quote_depth = 0;
	if (vars->pipes->heredoc_delim)
	{
		free(vars->pipes->heredoc_delim);
		vars->pipes->heredoc_delim = NULL;
	}
	vars->pipes->hd_expand = 0;
}

/*
Extracts raw delimiter string from input
- Sets start position at current position
- Advances position until delimiter end is found
- Validates non-empty delimiter
- Handles memory allocation
Returns:
- Raw delimiter string on success
- NULL on error (with error_code set)
*/
char	*get_delim_str(char *input, t_vars *vars, int *error_code)
{
	int		moves;
	char	*ori_delim_str;

	vars->start = vars->pos;
	while (input[vars->pos] && !ft_isspace(input[vars->pos])
		&& !is_operator_token(get_token_at(input, vars->pos, &moves)))
	{
		vars->pos++;
	}
	if (vars->pos == vars->start)
	{
		tok_syntax_error_msg("newline", vars);
		*error_code = 1;
		return (NULL);
	}
	ori_delim_str = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!ori_delim_str)
	{
		vars->error_code = ERR_DEFAULT;
		*error_code = 1;
		return (NULL);
	}
	return (ori_delim_str);
}

/*
Process heredoc delimiter when expected
- Skips whitespace before delimiter
- Extracts and validates delimiter string
- Creates token for the delimiter
- Updates state after processing
Returns:
- 0 on error
- 1 on success with next_flag set (continue)
- 2 on waiting for more input (whitespace skipping)
*/
int	proc_hd_delim(char *input, t_vars *vars, int *hd_is_delim)
{
	char	*ori_delim_str;
	int		error_code;

	error_code = 0;
	if (ft_isspace(input[vars->pos]))
	{
		vars->pos++;
		vars->start = vars->pos;
		return (2);
	}
	ori_delim_str = get_delim_str(input, vars, &error_code);
	if (error_code || !ori_delim_str)
		return (0);
	if (!is_valid_delim(ori_delim_str, vars))
	{
		free(ori_delim_str);
		return (0);
	}
	maketoken(ori_delim_str, TYPE_ARGS, vars);
	free(ori_delim_str);
	*hd_is_delim = 0;
	vars->start = vars->pos;
	vars->next_flag = 1;
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

/*
Process operators and update token state
- Handles heredoc operators specially
- Processes other operators through process_operator_char
- Updates state for next token processing
Returns:
- 1 on success
- 0 on error
*/
int	proc_opr_token(char *input, t_vars *vars, int *hd_is_delim, t_tokentype token_type)
{
	handle_text(input, vars);
	if (token_type == TYPE_HEREDOC)
	{
		maketoken("<<", TYPE_HEREDOC, vars);
		vars->pos += 2;
		vars->start = vars->pos;
		*hd_is_delim = 1;
	}
	else
	{
		if (!process_operator_char(input, &vars->pos, vars))
			return (0);
	}
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
Check token position handling
- Increments position counter if not in heredoc mode
Returns:
-1 if next_flag is set (to continue main loop).
- 0 to continue normal processing.
*/
int	chk_move_pos(t_vars *vars, int hd_is_delim)
{
	if (vars->next_flag)
		return (1);
	if (!hd_is_delim)
		vars->pos++;
	return (0);
}


