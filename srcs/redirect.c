/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:51:05 by bleow             #+#    #+#             */
/*   Updated: 2025/05/25 17:36:58 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Determines if token is a redirection operator.
- Checks if token type matches any redirection types.
- Includes all input and output redirection variants.
Returns:
- 1 if token is a redirection.
- 0 if not.
Works with process_redirections() and other redirection handlers.

Example: When processing token list
- Returns 1 for tokens of type <, >, >>, or <<
- Returns 0 for command, pipe, or other token types
*/
int	is_redirection(t_tokentype type)
{
	if (type == TYPE_HEREDOC || type == TYPE_IN_REDIRECT
		|| type == TYPE_OUT_REDIRECT || type == TYPE_APPEND_REDIRECT)
		return (1);
	else
		return (0);
}

/*
Restores a standard file descriptor (like stdin/stdout) from a saved fd.
- Checks if the saved_fd is valid (> 2).
- Duplicates saved_fd onto target_fd (e.g., STDIN_FILENO).
- Closes the saved_fd.
- Logs debug/error messages.
- Resets the saved_fd value to -1 via the pointer.
Works with reset_redirect_fds() to restore original file descriptors.
*/
void	restore_fd(int *saved_fd_ptr, int target_fd)
{
	int	result;

	if (*saved_fd_ptr > 2)
	{
		result = dup2(*saved_fd_ptr, target_fd);
		if (result == -1)
		{
			perror("dup2");
		}
		close(*saved_fd_ptr);
		*saved_fd_ptr = -1;
	}
}

/*
Resets specific redirection tracking variables within the t_pipe structure.
Works with reset_redirect_fds() to clean up redirection state after
command execution or FD restoration.
*/
void	reset_pipe_redir_state(t_pipe *pipes)
{
	if (!pipes)
		return ;
	pipes->out_mode = OUT_MODE_NONE;
	pipes->current_redirect = NULL;
	pipes->last_in_redir = NULL;
	pipes->last_out_redir = NULL;
	pipes->cmd_redir = NULL;
}

/*
Resets saved standard file descriptors and redirection state.
- Restores original stdin and stdout if they were changed.
- Closes any open heredoc or general redirection file descriptors.
- Resets internal redirection tracking variables.
Works with execute_cmd() to clean up after command execution.
*/
void	reset_redirect_fds(t_vars *vars)
{
	if (!vars || !vars->pipes)
		return ;
	restore_fd(&vars->pipes->saved_stdin, STDIN_FILENO);
	restore_fd(&vars->pipes->saved_stdout, STDOUT_FILENO);
	if (vars->pipes->heredoc_fd >= 0)
	{
		close(vars->pipes->heredoc_fd);
		vars->pipes->heredoc_fd = -1;
	}
	if (vars->pipes->redirection_fd > 2)
	{
		close(vars->pipes->redirection_fd);
		vars->pipes->redirection_fd = -1;
	}
	reset_pipe_redir_state(vars->pipes);
}

/*
Finds the next redirection node associated with a specific command.
- Iterates through the token list starting from the node after 'current'.
- Checks if a node is a redirection type using is_redirection().
- Checks if the redirection targets the specified 'cmd' node using
  get_redir_target().
Returns:
- Pointer to the next t_node that is a redirection for 'cmd'.
- NULL if no further redirections for 'cmd' are found after 'current'.
Works with proce_redir_chain() redirection processing logic to handle
multiple redirections applied to the same command.
(e.g., cmd < in1 > out1 < in2).
*/
t_node	*get_next_redir(t_node *current, t_node *cmd)
{
	t_node	*next;

	next = current->next;
	while (next)
	{
		if (is_redirection(next->type)
			&& get_redir_target(next, NULL) == cmd)
		{
			return (next);
		}
		next = next->next;
	}
	return (NULL);
}

/*
Handles creation of a redirection token and its filename.
Creates the redirection node and extracts the filename.
*/
// int handle_redirection_token(char *input, int *i, t_vars *vars, t_tokentype type)
// {
//     char *redir_str;
//     t_node *redir_node;
//     int moves;
    
//     // Create the redirection token
//     moves = (type == TYPE_HEREDOC || type == TYPE_APPEND_REDIRECT) ? 2 : 1;
//     redir_str = ft_substr(input, *i, moves);
//     if (!redir_str)
//         return (0);
        
//     // Create redirection node without a filename yet
//     redir_node = initnode(type, redir_str);
//     free(redir_str);
    
//     if (!redir_node)
//         return (0);
    
//     // Move past the redirection operator
//     *i += moves;
    
//     // Process the filename
//     if (!process_redir_filename(input, i, redir_node))
//     {
//         free_token_node(redir_node);
//         return (0);
//     }
    
//     // Add the node to the token list
//     build_token_linklist(vars, redir_node);
    
//     // Update vars for next token
//     vars->start = *i;
    
//     return (1);
// }
// int handle_redirection_token(char *input, int *i, t_vars *vars, t_tokentype type)
// {
//     char *redir_str;
//     t_node *redir_node;
//     int moves;
//     // int original_pos = *i;
    
//     fprintf(stderr, "DEBUG-REDIR-TOKEN: Handling redirection token at pos %d\n", *i);
    
//     // Create the redirection token
//     moves = (type == TYPE_HEREDOC || type == TYPE_APPEND_REDIRECT) ? 2 : 1;
//     redir_str = ft_substr(input, *i, moves);
//     if (!redir_str)
//     {
//         fprintf(stderr, "DEBUG-REDIR-TOKEN: Failed to create redirection string\n");
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-TOKEN: Created redirection string: '%s'\n", redir_str);
    
//     // Create redirection node without a filename yet
//     redir_node = initnode(type, redir_str);
//     free(redir_str);
    
//     if (!redir_node)
//     {
//         fprintf(stderr, "DEBUG-REDIR-TOKEN: Failed to initialize redirection node\n");
//         return (0);
//     }
    
//     // Move past the redirection operator
//     *i += moves;
    
//     fprintf(stderr, "DEBUG-REDIR-TOKEN: Looking for filename at position %d\n", *i);
    
//     // Process the filename
//     if (!process_redir_filename(input, i, redir_node))
//     {
//         fprintf(stderr, "DEBUG-REDIR-TOKEN: Failed to process redirection filename\n");
//         free_token_node(redir_node);
//         return (0);
//     }
    
//     // Add the node to the token list
//     build_token_linklist(vars, redir_node);
    
//     // Update vars for next token
//     vars->start = *i;
    
//     fprintf(stderr, "DEBUG-REDIR-TOKEN: Redirection token handled successfully\n");
//     fprintf(stderr, "DEBUG-REDIR-TOKEN: Redirection operator='%s', filename='%s'\n", 
//             get_token_str(type), redir_node->args ? redir_node->args[0] : "NULL");
    
//     return (1);
// }
int handle_redirection_token(char *input, int *i, t_vars *vars, t_tokentype type)
{
    char *redir_str;
    t_node *redir_node;
    int moves;
    
    fprintf(stderr, "DEBUG-REDIR-TOKEN: Handling redirection token at pos %d\n", *i);
    
    // Create the redirection token
    moves = (type == TYPE_HEREDOC || type == TYPE_APPEND_REDIRECT) ? 2 : 1;
    redir_str = ft_substr(input, *i, moves);
    if (!redir_str)
    {
        fprintf(stderr, "DEBUG-REDIR-TOKEN: Failed to create redirection string\n");
        return (0);
    }
    
    fprintf(stderr, "DEBUG-REDIR-TOKEN: Created redirection string: '%s'\n", redir_str);
    
    // Create redirection node without a filename yet
    redir_node = initnode(type, redir_str);
    free(redir_str);
    
    if (!redir_node)
    {
        fprintf(stderr, "DEBUG-REDIR-TOKEN: Failed to initialize redirection node\n");
        return (0);
    }
    
    // Move past the redirection operator
    *i += moves;
    
    fprintf(stderr, "DEBUG-REDIR-TOKEN: Looking for filename at position %d\n", *i);
    
    // Process the filename
    if (!process_redir_filename(input, i, redir_node))
    {
        fprintf(stderr, "DEBUG-REDIR-TOKEN: Failed to process redirection filename\n");
        free_token_node(redir_node);
        return (0);
    }
    
    // Add the node to the token list
    build_token_linklist(vars, redir_node);
    
    // Update vars for next token
    vars->start = *i;
    
    fprintf(stderr, "DEBUG-REDIR-TOKEN: Redirection token handled successfully\n");
    fprintf(stderr, "DEBUG-REDIR-TOKEN: Redirection operator='%s', filename='%s'\n", 
            get_token_str(type), redir_node->args ? redir_node->args[0] : "NULL");
    
    return (1);
}

/*
Processes and extracts the filename for a redirection token.
Skips whitespace and extracts the filename.
*/
// int process_redir_filename(char *input, int *i, t_node *redir_node)
// {
//     int start;
//     char *filename;
    
//     // Skip whitespace
//     while (input[*i] && ft_isspace(input[*i]))
//         (*i)++;
    
//     // Check if we have a filename
//     if (!input[*i])
//         return (1); // This will be caught later as a syntax error
    
//     // Find the end of the filename
//     start = *i;
//     while (input[*i] && !ft_isspace(input[*i]) && 
//            !is_operator_token(get_token_at(input, *i, &(int){0})))
//         (*i)++;
    
//     // Extract the filename
//     filename = ft_substr(input, start, *i - start);
//     if (!filename)
//         return (0);
    
//     // Store filename directly in args[0]
//     free(redir_node->args[0]); // Free the original token string
//     redir_node->args[0] = filename;
    
//     return (1);
// }
// int process_redir_filename(char *input, int *i, t_node *redir_node)
// {
//     int start;
//     char *filename;
    
//     fprintf(stderr, "DEBUG-REDIR-FILENAME: Processing filename at pos %d\n", *i);
    
//     // Skip whitespace
//     while (input[*i] && ft_isspace(input[*i]))
//     {
//         fprintf(stderr, "DEBUG-REDIR-FILENAME: Skipping whitespace '%c' at pos %d\n", 
//                 input[*i], *i);
//         (*i)++;
//     }
    
//     // Check if we have a filename
//     if (!input[*i])
//     {
//         fprintf(stderr, "DEBUG-REDIR-FILENAME: End of input reached, no filename found\n");
//         return (1); // This will be caught later as a syntax error
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-FILENAME: Found potential filename starting at pos %d: '%c'\n", 
//             *i, input[*i]);
    
//     // Find the end of the filename
//     start = *i;
//     while (input[*i] && !ft_isspace(input[*i]) && 
//            !is_operator_token(get_token_at(input, *i, &(int){0})))
//     {
//         fprintf(stderr, "DEBUG-REDIR-FILENAME: Including char at pos %d: '%c'\n", 
//                 *i, input[*i]);
//         (*i)++;
//     }
    
//     // Extract the filename
//     filename = ft_substr(input, start, *i - start);
//     if (!filename)
//     {
//         fprintf(stderr, "DEBUG-REDIR-FILENAME: Failed to extract filename\n");
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-FILENAME: Extracted filename: '%s'\n", filename);
    
//     // Store filename directly in args[0] - THIS IS THE KEY CHANGE
//     free(redir_node->args[0]); // Free the original redirection operator string
//     redir_node->args[0] = filename;
    
//     fprintf(stderr, "DEBUG-REDIR-FILENAME: Set redirection node args[0] to filename: '%s'\n", 
//             redir_node->args[0]);
    
//     return (1);
// }
int process_redir_filename(char *input, int *i, t_node *redir_node)
{
    int start;
    char *filename;
    
    fprintf(stderr, "DEBUG-REDIR-FILENAME: Processing filename at pos %d\n", *i);
    
    // Skip whitespace
    while (input[*i] && ft_isspace(input[*i]))
    {
        fprintf(stderr, "DEBUG-REDIR-FILENAME: Skipping whitespace '%c' at pos %d\n", 
                input[*i], *i);
        (*i)++;
    }
    
    // Check if we have a filename
    if (!input[*i])
    {
        fprintf(stderr, "DEBUG-REDIR-FILENAME: End of input reached, no filename found\n");
        return (1); // This will be caught later as a syntax error
    }
    
    fprintf(stderr, "DEBUG-REDIR-FILENAME: Found potential filename starting at pos %d: '%c'\n", 
            *i, input[*i]);
    
    // Find the end of the filename
    start = *i;
    while (input[*i] && !ft_isspace(input[*i]) && 
           !is_operator_token(get_token_at(input, *i, &(int){0})))
    {
        fprintf(stderr, "DEBUG-REDIR-FILENAME: Including char at pos %d: '%c'\n", 
                *i, input[*i]);
        (*i)++;
    }
    
    // Extract the filename
    filename = ft_substr(input, start, *i - start);
    if (!filename)
    {
        fprintf(stderr, "DEBUG-REDIR-FILENAME: Failed to extract filename\n");
        return (0);
    }
    
    fprintf(stderr, "DEBUG-REDIR-FILENAME: Extracted filename: '%s'\n", filename);
    
    // Store filename directly in args[0]
    free(redir_node->args[0]); // Free the original token string
    redir_node->args[0] = filename;
    
    fprintf(stderr, "DEBUG-REDIR-FILENAME: Set node args[0] to filename: '%s'\n", 
            redir_node->args[0]);
    
    return (1);
}
