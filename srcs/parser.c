/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 01:14:22 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 17:21:03 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets appropriate token type based on position and context.
Determines if current token should be a command or argument.
Updates vars->curr_type accordingly.
*/
// void	set_token_type(t_vars *vars, char *input)
// {
//     int			moves;
//     t_tokentype	special_type;
//     t_tokentype	determined_type;
//     static int	token_position = 0;

//     special_type = 0;
	
//     // DEBUG: Enhanced pipeline state tracking
//     fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: === TOKEN CLASSIFICATION START ===\n");
//     fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Position %d, analyzing token '%s'\n", 
//             token_position, input ? input : "NULL");
//     fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Current state - prev_type=%s, head=%s, in_pipe=%d\n",
//             get_token_str(vars->prev_type), 
//             vars->head ? "exists" : "NULL",
//             vars->pipes ? vars->pipes->in_pipe : -1);
	
//     // Store previous type before updating
//     vars->prev_type = vars->curr_type;
//     fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Updated prev_type to %s\n", 
//             get_token_str(vars->prev_type));
	
//     if (input && input[0] == '$')
//     {
//         vars->curr_type = TYPE_EXPANSION;
//         determined_type = TYPE_EXPANSION;
//         fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Detected expansion token\n");
//     }
//     else if (input && *input)
//     {
//         special_type = get_token_at(input, 0, &moves);
//         if (special_type != 0)
//         {
//             vars->curr_type = special_type;
//             determined_type = special_type;
//             fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Detected special token type=%s\n", 
//                     get_token_str(special_type));
//         }
//         else
//         {
//             // CRITICAL: Enhanced command detection logic
//             fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Analyzing regular token for CMD vs ARGS\n");
//             fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Conditions check:\n");
//             fprintf(stderr, "  - !vars->head: %s\n", !vars->head ? "true" : "false");
//             fprintf(stderr, "  - prev_type == TYPE_PIPE: %s\n", 
//                     vars->prev_type == TYPE_PIPE ? "true" : "false");
//             fprintf(stderr, "  - in_pipe: %d\n", vars->pipes ? vars->pipes->in_pipe : -1);
//             fprintf(stderr, "  - is_redirection(prev_type): %s\n", 
//                     is_redirection(vars->prev_type) ? "true" : "false");
			
//             // Check if we should classify as CMD
//             if (!vars->head || vars->prev_type == TYPE_PIPE)
//             {
//                 vars->curr_type = TYPE_CMD;
//                 determined_type = TYPE_CMD;
//                 fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Classified as CMD (head or after pipe)\n");
//             }
//             else if (vars->pipes && vars->pipes->in_pipe == 1 && 
//                      is_redirection(vars->prev_type))
//             {
//                 // CRITICAL FIX: In pipeline, after redirection, next token should be CMD
//                 vars->curr_type = TYPE_CMD;
//                 determined_type = TYPE_CMD;
//                 fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Classified as CMD (pipeline + after redirection)\n");
//             }
//             else
//             {
//                 vars->curr_type = TYPE_ARGS;
//                 determined_type = TYPE_ARGS;
//                 fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Classified as ARGS (default)\n");
//             }
//         }
//     }
//     else
//     {
//         if (!vars->head || vars->prev_type == TYPE_PIPE)
//         {
//             vars->curr_type = TYPE_CMD;
//             determined_type = TYPE_CMD;
//             fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Empty token classified as CMD\n");
//         }
//         else
//         {
//             vars->curr_type = TYPE_ARGS;
//             determined_type = TYPE_ARGS;
//             fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Empty token classified as ARGS\n");
//         }
//     }
	
//     fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Final classification: '%s' -> %s\n", 
//             input ? input : "NULL", get_token_str(determined_type));
//     fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: === TOKEN CLASSIFICATION END ===\n\n");
	
//     token_position++;
// }
void	set_token_type(t_vars *vars, char *input)
{
	int			moves;
	t_tokentype	special_type;
	t_tokentype	determined_type;
	static int	token_position = 0;

	special_type = 0;
	
	// DEBUG: Enhanced pipeline state tracking
	fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: === TOKEN CLASSIFICATION START ===\n");
	fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Position %d, analyzing token '%s'\n", 
			token_position, input ? input : "NULL");
	fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Current state - prev_type=%s, head=%s, in_pipe=%d\n",
			get_token_str(vars->prev_type), 
			vars->head ? "exists" : "NULL",
			vars->pipes ? vars->pipes->in_pipe : -1);
	
	// CRITICAL FIX: DON'T update prev_type here - it corrupts the state
	// vars->prev_type = vars->curr_type;  // REMOVED THIS LINE
	fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Keeping prev_type as %s (not updating yet)\n", 
			get_token_str(vars->prev_type));
	
	if (input && input[0] == '$')
	{
		vars->curr_type = TYPE_EXPANSION;
		determined_type = TYPE_EXPANSION;
		fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Detected expansion token\n");
	}
	else if (input && *input)
	{
		special_type = get_token_at(input, 0, &moves);
		if (special_type != 0)
		{
			vars->curr_type = special_type;
			determined_type = special_type;
			fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Detected special token type=%s\n", 
					get_token_str(special_type));
		}
		else
		{
			// CRITICAL: Enhanced command detection logic
			fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Analyzing regular token for CMD vs ARGS\n");
			fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Conditions check:\n");
			fprintf(stderr, "  - !vars->head: %s\n", !vars->head ? "true" : "false");
			fprintf(stderr, "  - prev_type == TYPE_PIPE: %s\n", 
					vars->prev_type == TYPE_PIPE ? "true" : "false");
			fprintf(stderr, "  - in_pipe: %d\n", vars->pipes ? vars->pipes->in_pipe : -1);
			fprintf(stderr, "  - is_redirection(prev_type): %s\n", 
					is_redirection(vars->prev_type) ? "true" : "false");
			
			// Check if we should classify as CMD
			if (!vars->head || vars->prev_type == TYPE_PIPE)
			{
				vars->curr_type = TYPE_CMD;
				determined_type = TYPE_CMD;
				fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Classified as CMD (head or after pipe)\n");
			}
			else if (vars->pipes && vars->pipes->in_pipe == 1 && 
					 is_redirection(vars->prev_type))
			{
				// CRITICAL FIX: In pipeline, after redirection, next token should be CMD
				vars->curr_type = TYPE_CMD;
				determined_type = TYPE_CMD;
				fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Classified as CMD (pipeline + after redirection)\n");
			}
			else
			{
				vars->curr_type = TYPE_ARGS;
				determined_type = TYPE_ARGS;
				fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Classified as ARGS (default)\n");
			}
		}
	}
	else
	{
		if (!vars->head || vars->prev_type == TYPE_PIPE)
		{
			vars->curr_type = TYPE_CMD;
			determined_type = TYPE_CMD;
			fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Empty token classified as CMD\n");
		}
		else
		{
			vars->curr_type = TYPE_ARGS;
			determined_type = TYPE_ARGS;
			fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Empty token classified as ARGS\n");
		}
	}
	
	fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: Final classification: '%s' -> %s\n", 
			input ? input : "NULL", get_token_str(determined_type));
	fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: prev_type remains: %s (will be updated in maketoken)\n",
			get_token_str(vars->prev_type));
	fprintf(stderr, "DEBUG-SET-TOKEN-TYPE: === TOKEN CLASSIFICATION END ===\n\n");
	
	token_position++;
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
