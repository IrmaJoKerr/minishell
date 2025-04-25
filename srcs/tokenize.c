/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/04/26 01:23:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Main entry point for tokenization and expansion. 
Tokenizes input and processes expansions.
Returns 1 on success, 0 on failure.
Example: For input "echo "hello
- Tokenizes the initial content
- Detects unclosed quotes and prompts for completion
- Returns completed command string with proper quotes
*/
int	process_input_tokens(char *command, t_vars *vars)
{
	if (!command || !*command)
		return (0);
	cleanup_token_list(vars);
	if (!tokenizer(command, vars))
		return (0);
	return (1);
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
