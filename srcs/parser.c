/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 01:14:22 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 10:53:42 by bleow            ###   ########.fr       */
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
	t_tokentype	type;

	type = 0;
	vars->prev_type = vars->curr_type;
	if (input && input[0] == '$')
	{
		vars->curr_type = TYPE_EXPANSION;
		return ;
	}
	if (input && *input)
	{
		type = get_token_at(input, 0, &moves);
		if (type != 0)
		{
			vars->curr_type = type;
			return ;
		}
	}
	if (!vars->head || vars->prev_type == TYPE_PIPE)
	// if (!vars->head || vars->prev_type == TYPE_PIPE || (vars->pipes->in_pipe == 1 && is_redirection(vars->prev_type)))
		vars->curr_type = TYPE_CMD;
	else
		vars->curr_type = TYPE_ARGS;
}
// void	set_token_type(t_vars *vars, char *input)
// {
// 	int			moves;
// 	t_tokentype	special_type;
// 	t_tokentype	determined_type;
// 	static int	token_position = 0;

// 	special_type = 0;
// 	vars->prev_type = vars->curr_type;
	
// 	if (input && input[0] == '$')
// 	{
// 		vars->curr_type = TYPE_EXPANSION;
// 		determined_type = TYPE_EXPANSION;
// 	}
// 	else if (input && *input)
// 	{
// 		special_type = get_token_at(input, 0, &moves);
// 		if (special_type != 0)
// 		{
// 			vars->curr_type = special_type;
// 			determined_type = special_type;
// 		}
// 		else if (!vars->head || vars->prev_type == TYPE_PIPE)
// 		{
// 			vars->curr_type = TYPE_CMD;
// 			determined_type = TYPE_CMD;
// 		}
// 		else
// 		{
// 			vars->curr_type = TYPE_ARGS;
// 			determined_type = TYPE_ARGS;
// 		}
// 	}
// 	else
// 	{
// 		if (!vars->head || vars->prev_type == TYPE_PIPE)
// 			vars->curr_type = TYPE_CMD;
// 		else
// 			vars->curr_type = TYPE_ARGS;
// 		determined_type = vars->curr_type;
// 	}
	
// 	// DEBUG CALL #1: Token type determination with debug
// 	fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: Analyzing token '%s' at position %d\n", 
// 			input ? input : "NULL", token_position);
// 	fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: State: prev_type=%s, head=%s\n",
// 			get_token_str(vars->prev_type), vars->head ? "exists" : "NULL");
// 	fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: Final classification: '%s' -> %s\n", 
// 			input ? input : "NULL", get_token_str(determined_type));
	
// 	// DEBUG CALL #2: Pipeline awareness
// 	debug_pipeline_awareness(vars, token_position);
	
// 	token_position++;
// }

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
