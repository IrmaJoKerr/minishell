/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 11:09:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Joins a chunk to an existing string, freeing the original string.
- Takes base string and chunk to append.
- Creates new joined string and frees original.
Returns:
Newly allocated string with combined content.
Original input string if chunk is NULL.
New copy of chunk if base string is NULL.
Works with expand_one_line().

Example: str = "Hello", chunk = " World"
- Returns: "Hello World"
- Original "Hello" string is freed
*/
char	*merge_and_free(char *str, char *chunk)
{
	char	*new_str;

	if (!chunk)
		return (str);
	if (!str)
		return (ft_strdup(chunk));
	new_str = ft_strjoin(str, chunk);
	free(str);
	return (new_str);
}

/*
Extracts and expands a variable from heredoc line.
- Processes a variable starting with $ character.
- Falls back to returning "$" if expansion fails.
Returns:
Newly allocated string with expanded variable.
"$" if variable can't be expanded.
Works with expand_one_line().

Example: At position of "$HOME" in a line
- Returns: "/Users/username" (expanded value)
- Updates position to after variable name
*/
// char	*expand_heredoc_var(char *line, int *pos, t_vars *vars)
// {
// 	char	*expanded;
// 	int		old_pos;

// 	old_pos = *pos;
// 	expanded = handle_expansion(line, pos, vars);
// 	if (!expanded)
// 	{
// 		*pos = old_pos + 1;
// 		return (ft_strdup("$"));
// 	}
// 	return (expanded);
// }
char	*expand_heredoc_var(char *line, int *pos, t_vars *vars)
{
    char	*expanded;
    int		old_pos;

    // Remember original position for error recovery
    old_pos = *pos;
    
    // Replace handle_expansion() with direct call to expand_variable()
    expanded = expand_variable(line, pos, NULL, vars);
    
    // Handle expansion failures - return "$" and move position
    if (!expanded)
    {
        *pos = old_pos + 1;  // Skip just the $ character
        expanded = ft_strdup("$");
        
        // Register allocation for debug tracking
        #ifdef DEBUG_MEMORY
        register_allocation(expanded, "HEREDOC_FAILBACK", __func__);
        #endif
        
        fprintf(stderr, "DEBUG: expand_heredoc_var: Expansion failed, returning '$'\n");
        return (expanded);
    }
    
    fprintf(stderr, "DEBUG: expand_heredoc_var: Expanded to '%s'\n", expanded);
    return (expanded);
}

/*
Extracts a regular text string up to $ character.
- Processes text segment from current position to next $.
- Creates substring of this text segment.
Returns:
Newly allocated substring from start to next variable.
NULL if no characters to extract.
Works with expand_one_line().

Example: At start of "Hello $USER"
- Returns: "Hello "
- Updates position to the $ character
*/
char	*read_heredoc_str(char *line, int *pos)
{
	int	start;

	start = *pos;
	while (line[*pos] && line[*pos] != '$')
		(*pos)++;
	if (*pos > start)
		return (ft_substr(line, start, *pos - start));
	return (NULL);
}

/*
Processes one segment of a heredoc line.
- Handles either variable expansion or regular text.
- Joins processed segment to result string.
- Updates position for next segment processing.
Returns:
Updated result string with new segment added.
Works with expand_heredoc_line().

Example: For input "Hello $HOME"
- First call processes "Hello " (regular text)
- Second call processes "$HOME" (variable)
- Returns combined result after each call
*/
char	*expand_one_line(char *line, int *pos, t_vars *vars, char *result)
{
	char	*segment;

	if (line[*pos] == '$')
	{
		segment = expand_heredoc_var(line, pos, vars);
		result = merge_and_free(result, segment);
		free(segment);
	}
	else
	{
		segment = read_heredoc_str(line, pos);
		result = merge_and_free(result, segment);
		free(segment);
	}
	return (result);
}

/*
Expands all variables in a heredoc line.
- Processes entire line character by character.
- Builds result by combining expanded segments.
- Handles all variable expansions in the line.
Returns:
New string with all variables expanded.
Empty string on NULL input or if no expansions.
Works with write_to_heredoc().

Example: Input "Hello $USER world"
- Expands to "Hello username world"
- All $VAR references replaced with their values
*/
char	*expand_heredoc_line(char *line, t_vars *vars)
{
	int		pos;
	char	*result;

	if (!line || !vars)
		return (ft_strdup(""));
	result = NULL;
	pos = 0;
	while (line[pos])
		result = expand_one_line(line, &pos, vars, result);
	if (!result)
		return (ft_strdup(""));
	return (result);
}

// /*
// Checks if a heredoc delimiter contains quotes.
// - Examines delimiter character by character.
// - Determines if variables should be expanded.
// Returns:
// 1 if variables should be expanded (no quotes).
// 0 if variables shouldn't be expanded (has quotes).
// Works with handle_heredoc().

// Example: "EOF" -> 1 (expand variables)
// 		 "'EOF'" -> 0 (don't expand variables)
// */
// int	chk_expand_heredoc(char *delimiter)
// {
// 	int	i;

// 	if (!delimiter)
// 		return (0);
// 	i = 0;
// 	while (delimiter[i])
// 	{
// 		if (ft_isquote(delimiter[i]))
// 			return (0);
// 		i++;
// 	}
// 	return (1);
// }

void setup_heredoc_pipe(t_vars *vars)
{
    int fd[2];
    
    // Clean up previous heredoc pipe if it exists
    if (vars->pipes->heredoc_fd > 0)
    {
        close(vars->pipes->heredoc_fd);
        vars->pipes->heredoc_fd = -1;
    }
    
    if (vars->pipes->hd_fd_write > 0)  // Also close write end if open
    {
        close(vars->pipes->hd_fd_write);
        vars->pipes->hd_fd_write = -1;
    }
    
    // Create a new pipe
    if (pipe(fd) == -1)
    {
        DBG_PRINTF(1, "Failed to create heredoc pipe: %s\n", strerror(errno));
        return;
    }
    
    // Store the ends in separate variables
    vars->pipes->heredoc_fd = fd[0];  // Read end
    vars->pipes->hd_fd_write = fd[1];    // Write end
    
    DBG_PRINTF(1, "Heredoc pipe created: read_fd=%d, write_fd=%d\n", 
              fd[0], fd[1]);
}

// /*
// Processes quotes in heredoc delimiter
// - Removes the outside quotes but preserves content
// - Returns 1 if quotes were removed, 0 otherwise
// */
// int strip_outer_quotes(char **delimiter)
// {
//     char	*str;
//     size_t	len;
//     char	*new_str;
//     int		result;
    
//     if (!delimiter || !*delimiter)
// 	{
//         return (0);
// 	}
// 	result = 0;
//     str = *delimiter;
//     len = ft_strlen(str);
//     fprintf(stderr, "[DBG_HEREDOC] strip_outer_quotes: Processing '%s'\n", str);
//     // Need at least 2 chars for quotes
//     if (len < 2)
//         return (0);
//     // Check for matching quotes at start and end
//     if ((str[0] == '"' && str[len - 1] == '"')
// 		|| (str[0] == '\'' && str[len - 1] == '\''))
//     {
//         fprintf(stderr, "[DBG_HEREDOC] Removing quotes from '%s'\n", str);
//         new_str = ft_substr(str, 1, len - 2);
//         if (new_str)
//         {
//             free(*delimiter);  // Free the string content
//             *delimiter = new_str;
//             result = 1;
//             fprintf(stderr, "[DBG_HEREDOC] After removing quotes: '%s'\n", *delimiter);
//         }
//     }
//     return (result);
// }

// /**
//  * Processes quotes in heredoc delimiter
//  * - Removes the outside quotes but preserves content
//  * - Returns 1 if quotes were removed, 0 otherwise
//  */
// int strip_outer_quotes(char **delimiter)
// {
//     char	*str;
//     size_t	len;
//     char	*new_str;
//     int		result;
    
//     if (!delimiter || !*delimiter)
//         return (0);
        
//     result = 0;
//     str = *delimiter;
//     len = ft_strlen(str);
//     fprintf(stderr, "[DBG_HEREDOC] strip_outer_quotes: Processing '%s'\n", str);
    
//     // Need at least 2 chars for quotes
//     if (len < 2)
//         return (0);
        
//     // Check for matching quotes at start and end
//     if ((str[0] == '"' && str[len - 1] == '"') || 
//         (str[0] == '\'' && str[len - 1] == '\''))
//     {
//         fprintf(stderr, "[DBG_HEREDOC] Removing quotes from '%s'\n", str);
//         new_str = ft_substr(str, 1, len - 2);
//         if (new_str)
//         {
//             free(*delimiter);  // Free the string content
//             *delimiter = new_str;
//             result = 1;
//             fprintf(stderr, "[DBG_HEREDOC] After removing quotes: '%s'\n", *delimiter);
//         }
//     }
//     return (result);
// }
/** //POTENTIAL TO DEPRECATE
 * Processes a heredoc delimiter by removing quotes if present.
 * This is for comparison purposes with input lines.
 * 
 * @param raw_delimiter The delimiter with potential quotes
 * @return A newly allocated string with quotes removed, or NULL on error
 */
int strip_outer_quotes(char **delimiter)
{
    char *str;
    size_t len;
    char *new_str;
    int result;
    
    if (!delimiter || !*delimiter)
        return (0);
        
    result = 0;
    str = *delimiter;
    len = ft_strlen(str);
    fprintf(stderr, "[DBG_HEREDOC] strip_outer_quotes: Processing '%s'\n", str);
    
    // Need at least 2 chars for quotes
    if (len < 2)
        return (0);
        
    // Check for matching quotes at start and end
    if ((str[0] == '"' && str[len - 1] == '"') || 
        (str[0] == '\'' && str[len - 1] == '\''))
    {
        fprintf(stderr, "[DBG_HEREDOC] Removing quotes from '%s'\n", str);
        new_str = ft_substr(str, 1, len - 2);
        if (new_str)
        {
            free(*delimiter);  // Free the string content
            *delimiter = new_str;
            result = 1;
            fprintf(stderr, "[DBG_HEREDOC] After removing quotes: '%s'\n", *delimiter);
        }
    }
    return (result);
}

// int store_heredoc_content(char *input, int start_pos, char *delimiter, t_vars *vars)
// {
//     int i = start_pos;
//     int line_start;
//     int line_end;
//     // char *line;
//     int found_delimiter = 0;
//     int fd;
    
//     // Open temporary file for writing
//     fprintf(stderr, "[DBG_HEREDOC] Opening temporary file for heredoc content\n");
//     fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
//     if (fd == -1)
//     {
//         fprintf(stderr, "[DBG_HEREDOC] Failed to open temp file: %s\n", strerror(errno));
//         return (0);
//     }
    
//     // Process lines and write to temp file
//     while (input[i])
//     {
//         line_start = i;
//         // Find end of this line
//         while (input[i] && input[i] != '\n')
//             i++;
//         line_end = i;
        
//         // Check if this line is the delimiter
//         if (ft_strlen(delimiter) == (size_t)(line_end - line_start) &&
//             !ft_strncmp(&input[line_start], delimiter, line_end - line_start))
//         {
//             fprintf(stderr, "[DBG_HEREDOC] Found delimiter '%s' at position %d\n", 
//                   delimiter, line_start);
//             found_delimiter = 1;
//             break;
//         }
        
//         // Extract this line
//         // line = ft_substr(input, line_start, line_end - line_start);
//         // if (line)
// 		// Extract this line
// 		char *line = ft_substr(input, line_start, line_end - line_start);
// 		if (line)
// 		{
//     		// Write to file with optional variable expansion
//     		fprintf(stderr, "[DBG_HEREDOC] Writing line to temp file: '%s'\n", line);
//     		write_to_heredoc(fd, line, vars, chk_expand_heredoc(delimiter));
//     		free(line); // Free the temporary line
// 		}
//         if (input[i] == '\n')
//             i++;
//     }
//     // Close the file when done writing
//     fprintf(stderr, "[DBG_HEREDOC] Closing temp file\n");
//     close(fd);
//     if (found_delimiter)
//     {
//         fprintf(stderr, "[DBG_HEREDOC] Content stored in temp file, setting heredoc_mode=1\n");
//         vars->heredoc_mode = 1; // Mark as multiline mode
//         return (1);
//     }
//     else
//     {
//         fprintf(stderr, "[DBG_HEREDOC] No delimiter found\n");
//         // File left in place but we'll overwrite it next time
//         return (0);
//     }
// }

/*
MODIFIED: Writes line to heredoc fd (TMP_BUF) with variable expansion based on vars->pipes->hd_expand.
Returns 1 on success, 0 on failure.
*/
int write_to_heredoc(int fd, char *line, t_vars *vars)
{
    char	*expanded_line;
    int		write_result;

    if (!line || fd < 0 || !vars || !vars->pipes)
        return (0);
    // Use the stored expansion flag
    if (vars->pipes->hd_expand == 1)
    {
        expanded_line = expand_heredoc_line(line, vars);
        if (!expanded_line)
            return (0); // Expansion failed (malloc?)
    }
    else
    {
        expanded_line = ft_strdup(line);
        if (!expanded_line)
            return (0); // Malloc failed
    }
    // Write the line to the fd with newline
    write_result = write(fd, expanded_line, ft_strlen(expanded_line));
    if (write_result != -1) // Only add newline if first write succeeded
        write(fd, "\n", 1);
    free(expanded_line);
    if (write_result == -1)
    {
        perror("bleshell: write error in heredoc");
        vars->error_code = ERR_DEFAULT;
        return (0); // Write failed
    }
    else
        return (1); // Success
}

// /*
// NEW: Stores content from multiline input buffer into TMP_BUF.
// Assumes the first line (with command and delimiter) is already processed.
// Uses the delimiter and expansion flag stored in vars.
// Returns 1 on success, 0 on failure.
// */
// int store_multiline_heredoc_content(char *input_after_first_line, t_vars *vars)
// {
//     int		fd;
//     char	*current_line;
//     char	*next_line_start;
//     int		line_len;
//     int		found_delimiter = 0;

//     if (!input_after_first_line || !vars || !vars->pipes || !vars->pipes->heredoc_delim)
//         return (0);
//     fprintf(stderr, "[DBG_HEREDOC] Storing multiline content, Delim='%s', Expand=%d\n",
//             vars->pipes->heredoc_delim, vars->pipes->hd_expand);
//     fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
//     if (fd == -1)
//     {
//         perror("bleshell: failed to open temp heredoc file for multiline storage");
//         vars->error_code = ERR_DEFAULT;
//         return (0);
//     }
//     next_line_start = input_after_first_line;
//     while (*next_line_start)
//     {
//         current_line = next_line_start;
//         // Find the end of the current line
//         while (*next_line_start && *next_line_start != '\n')
//             next_line_start++;
//         line_len = next_line_start - current_line;
//         // Check if this line is exactly the delimiter
//         if (line_len == (int)ft_strlen(vars->pipes->heredoc_delim) &&
//             ft_strncmp(current_line, vars->pipes->heredoc_delim, line_len) == 0)
//         {
//             fprintf(stderr, "[DBG_HEREDOC] Found delimiter in multiline input.\n");
//             found_delimiter = 1;
//             break; // Stop processing, don't write delimiter
//         }
//         // Write the line (allocate temporarily to pass to write_to_heredoc)
//         char *temp_line = ft_substr(current_line, 0, line_len);
//         if (!temp_line) { /* Malloc error */ close(fd); return (0); }
//         if (!write_to_heredoc(fd, temp_line, vars))
//         {
//             free(temp_line);
//             close(fd);
//             unlink(TMP_BUF); // Attempt cleanup
//             return (0); // Write failed
//         }
//         free(temp_line);
//         // Move to the start of the next line (skip '\n')
//         if (*next_line_start == '\n')
//             next_line_start++;
//     }
//     close(fd);
//     if (!found_delimiter)
//     {
//         fprintf(stderr, "bleshell: warning: here-document delimited by end-of-file (wanted `%s')\n", vars->pipes->heredoc_delim);
//         // Continue anyway, content up to EOF is stored.
//     }
//     fprintf(stderr, "[DBG_HEREDOC] Multiline content stored in %s\n", TMP_BUF);
//     return (1); // Success (even if delimiter wasn't found, EOF acts as delimiter)
// }
/*
MODIFIED: Stores the content provided in the 'content' buffer into the heredoc
temporary file (TMP_BUF). Stops if the delimiter is found within the buffer.
Performs expansion based on vars->pipes->hd_expand.

Returns:
 0: Delimiter found within the 'content' buffer.
 1: Delimiter NOT found within the 'content' buffer (reached end of buffer).
-1: Error (e.g., file open/write failed).
*/
int store_multiline_heredoc_content(char *content, t_vars *vars)
{
    int     fd;
    char    *line;
    char    *expanded_line;
    size_t  delim_len;
    int     delimiter_found = 0;
    char    *current_pos = content;
    char    *next_newline;

    if (!vars->pipes->heredoc_delim)
        return (-1); // Should not happen if called correctly

    DBG_PRINTF(DEBUG_HEREDOC, "Storing multiline content, Delim='%s', Expand=%d\n",
               vars->pipes->heredoc_delim, vars->pipes->hd_expand);

    fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) {
        perror("bleshell: store_multiline_heredoc_content: open");
        vars->error_code = ERR_DEFAULT;
        return (-1);
    }

    delim_len = ft_strlen(vars->pipes->heredoc_delim);

    while (current_pos && *current_pos)
    {
        next_newline = ft_strchr(current_pos, '\n');
        if (next_newline) {
            // Extract line without newline
            line = ft_substr(current_pos, 0, next_newline - current_pos);
            current_pos = next_newline + 1; // Move past newline for next iteration
        } else {
            // Last part of the buffer without a trailing newline
            line = ft_strdup(current_pos);
            current_pos = NULL; // Signal end of buffer
        }

        if (!line) { close(fd); vars->error_code = ERR_DEFAULT; return (-1); } // Malloc error

        // Check for delimiter
        if (ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0 &&
            line[delim_len] == '\0')
        {
            DBG_PRINTF(DEBUG_HEREDOC, "Found delimiter in multiline buffer.\n");
            delimiter_found = 1;
            free(line);
            break; // Delimiter found, stop processing buffer
        }

        // Expand if necessary
        if (vars->pipes->hd_expand) {
            // Replace perform_expansion with expand_heredoc_line
            expanded_line = expand_heredoc_line(line, vars);
            free(line); // Free the original line
            if (!expanded_line) { // Check if expansion failed
                 close(fd);
                 vars->error_code = ERR_DEFAULT; // Assuming expansion failure sets error
                 return (-1);
            }
            line = expanded_line; // Use the expanded line
        }

        // Write line + newline to temp file
        if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1) {
            perror("bleshell: store_multiline_heredoc_content: write");
            free(line); // Free line (original or expanded) before returning
            close(fd);
            vars->error_code = ERR_DEFAULT;
            return (-1);
        }
        free(line); // Free line (original or expanded) after successful write

        if (!next_newline) break; // Reached end if no newline was found
    }

    close(fd);

    if (!delimiter_found) {
        // Print warning only if we reached the end of the *initial* buffer without finding delim
         fprintf(stderr, "bleshell: warning: here-document delimited by end-of-file (wanted `%s')\n", vars->pipes->heredoc_delim);
         DBG_PRINTF(DEBUG_HEREDOC, "Delimiter NOT found in initial buffer.\n");
         return (1); // Signal that interactive reading is needed
    }

    DBG_PRINTF(DEBUG_HEREDOC, "Multiline content stored in %s (from buffer)\n", TMP_BUF);
    return (0); // Delimiter was found in the buffer
}

/*
Reads heredoc input interactively from stdin until the delimiter is found.
Appends the input (with expansion if needed) to the heredoc temporary file.
Assumes TMP_BUF might already contain content from store_multiline_heredoc_content.

Returns:
 0: Success (delimiter found or EOF reached after warning).
-1: Error (e.g., file open/write failed, malloc failed).
*/
int read_heredoc_interactive(t_vars *vars)
{
    int     fd;
    char    *line = NULL;
    char    *expanded_line;
    size_t  delim_len;
    int     eof_reached = 0;

    if (!vars->pipes->heredoc_delim)
        return (-1);

    DBG_PRINTF(DEBUG_HEREDOC, "Starting interactive heredoc reading, Delim='%s', Expand=%d\n",
               vars->pipes->heredoc_delim, vars->pipes->hd_expand);

    // Open in APPEND mode
    fd = open(TMP_BUF, O_WRONLY | O_APPEND, 0600);
    if (fd == -1) {
        // If the file doesn't exist (e.g., store_multiline failed silently or wasn't called), try creating it.
        fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (fd == -1) {
             perror("bleshell: read_heredoc_interactive: open");
             vars->error_code = ERR_DEFAULT;
             return (-1);
        }
    }

    delim_len = ft_strlen(vars->pipes->heredoc_delim);

    while (!eof_reached)
    {
        // Use readline for interactive prompt
        line = readline("> ");

        // Handle EOF (Ctrl+D)
        if (!line) {
            fprintf(stderr, "bleshell: warning: here-document delimited by end-of-file (wanted `%s')\n", vars->pipes->heredoc_delim);
            eof_reached = 1; // Mark EOF reached
            break; // Exit loop on EOF
        }

        // Check for delimiter
        if (ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0 &&
            line[delim_len] == '\0')
        {
            DBG_PRINTF(DEBUG_HEREDOC, "Found delimiter interactively.\n");
            free(line);
            break; // Delimiter found
        }

        // Expand if necessary
        if (vars->pipes->hd_expand) {
            // Use expand_heredoc_line (assuming it exists and handles full line expansion)
            expanded_line = expand_heredoc_line(line, vars);
            free(line); // Free the original line from readline
            if (!expanded_line) { // Check if expansion failed
                 close(fd);
                 vars->error_code = ERR_DEFAULT; // Assuming expansion failure sets error
                 return (-1);
            }
            line = expanded_line; // Use the expanded line for writing
        }

        // Write line + newline to temp file
        if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1) {
            perror("bleshell: read_heredoc_interactive: write");
            free(line); // Free line (original or expanded) before returning
            close(fd);
            vars->error_code = ERR_DEFAULT;
            return (-1);
        }
        free(line); // Free line (original or expanded) after successful write
        line = NULL; // Reset for next readline call
    }

    close(fd);
    DBG_PRINTF(DEBUG_HEREDOC, "Interactive heredoc reading finished.\n");
    return (0); // Success (even if EOF was reached after warning)
}

// /*
// Reads input for heredoc until delimiter is encountered.
// - Prompts user for input lines with "> ".
// - Compares each line against delimiter.
// - Writes valid lines to the pipe.
// Returns:
// 1 when completed successfully.
// 0 on any error.
// Works with handle_heredoc().

// Example: With delimiter "EOF"
// - Reads lines like "Hello", "$USER", "EOF"
// - Writes "Hello" and expanded "$USER" to pipe
// - Stops at "EOF" line, returning 1
// */
// int read_heredoc(int *fd, char *delimiter, t_vars *vars, int expand_vars)
// {
//     DBG_PRINTF(1, "Reading heredoc with delimiter: '%s'\n", delimiter);
    
//     // First check if we already have a heredoc pipe set up
//     if (vars->pipes->heredoc_fd > 0)
//     {
//         DBG_PRINTF(1, "Using existing heredoc pipe (fd=%d)\n", vars->pipes->heredoc_fd);
//         // Just pass along the existing file descriptor
//         fd[0] = vars->pipes->heredoc_fd;
//         vars->pipes->heredoc_fd = -1; // Transfer ownership
//         return (1);
//     }
    
//     // Otherwise, create a new pipe and collect input interactively
//     if (pipe(fd) == -1)
//         return (handle_heredoc_err(vars));
        
//     DBG_PRINTF(1, "Entering interactive heredoc mode\n");
    
//     // Your existing interactive heredoc code...
//     while (1)
//     {
//         char *line = readline(PROMPT);
//         if (!line)
//         {
//             DBG_PRINTF(1, "EOF in heredoc\n");
//             close(fd[1]);
//             return 0;
//         }
        
//         if (ft_strcmp(line, delimiter) == 0)
//         {
//             free(line);
//             close(fd[1]);
//             return 1;
//         }
        
//         write_to_heredoc(fd[1], line, vars, expand_vars);
//         free(line);
//     }
// }

/*
Creates pipe to handle heredoc redirection error cases.
- Validates node has required arguments.
- Sets error code appropriately.
Returns:
-1 to indicate error condition.
Works with handle_heredoc().
*/
int	handle_heredoc_err(t_vars *vars)
{
	vars->error_code = 1;
	return (0);
}

/*
MODIFIED: Sets up redirection using the heredoc fd stored in vars->pipes->heredoc_fd.
Calls process_heredoc to ensure content is ready (either reads interactively or opens pre-stored file).
*/
int handle_heredoc(t_node *node, t_vars *vars)
{
    fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Setting up heredoc for command\n");
    // Ensure heredoc content is ready in TMP_BUF and fd is set
    if (!process_heredoc(node, vars))
    {
        fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() process_heredoc failed.\n");
        // process_heredoc should set error_code
        return (0); // Failure
    }
    // Check if process_heredoc successfully set the fd
    if (vars->pipes->heredoc_fd < 0)
    {
        fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Invalid heredoc_fd after process_heredoc.\n");
        return (0); // Should not happen if process_heredoc succeeded
    }
    // Connect the pipe/file to stdin
    if (dup2(vars->pipes->heredoc_fd, STDIN_FILENO) == -1)
    {
        perror("bleshell: failed to redirect stdin for heredoc");
        close(vars->pipes->heredoc_fd);
        vars->pipes->heredoc_fd = -1;
        vars->error_code = ERR_DEFAULT;
        return (0);
    }
    // Close the original fd as it's now duplicated to stdin
    close(vars->pipes->heredoc_fd);
    vars->pipes->heredoc_fd = -1; // Mark as used/closed
    fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Heredoc connected to stdin\n");
    return (1); // Success
}

/*
Resets the heredoc pipe infrastructure.
Closes existing pipe if any and creates a fresh one.
Returns the write end fd or -1 on error.
*/
int	reset_heredoc_pipe(t_vars *vars)
{
    int	fd[2];
    
    fprintf(stderr, "[DBG_HEREDOC] Resetting heredoc pipe\n");
    
    // Close existing pipe if any
    if (vars->pipes->heredoc_fd > 0) {
        fprintf(stderr, "[DBG_HEREDOC] Closing existing heredoc fd=%d\n", 
                vars->pipes->heredoc_fd);
        close(vars->pipes->heredoc_fd);
    }
    
    // Create a fresh pipe
    if (pipe(fd) == -1) {
        fprintf(stderr, "[DBG_HEREDOC] Pipe creation failed: %s\n", strerror(errno));
        vars->pipes->heredoc_fd = -1;
        return (-1);
    }
    
    fprintf(stderr, "[DBG_HEREDOC] Created new heredoc pipe: read_fd=%d, write_fd=%d\n", 
            fd[0], fd[1]);
    
    // Store read end for later use
    vars->pipes->heredoc_fd = fd[0];
    
    // Return write end for immediate use
    return (fd[1]);
}

// /**
//  * Processes a heredoc delimiter: removes quotes if present and sets expansion flag.
//  * This is the central function for all delimiter processing.
//  * 
//  * @param raw_delimiter The raw delimiter string extracted from input
//  * @param vars Program state variables to update
//  * @return 1 if successfully processed, 0 on error
//  */
// int process_heredoc_info(char *raw_delimiter, t_vars *vars)
// {
//     char *delimiter_copy;
//     int was_quoted;
    
//     if (!raw_delimiter || !vars || !vars->pipes)
//         return 0;
        
//     // Make a copy we can safely modify
//     delimiter_copy = ft_strdup(raw_delimiter);
//     if (!delimiter_copy)
//         return 0;
        
//     // Clean up any existing delimiter
//     if (vars->pipes->heredoc_delim)
//     {
//         free(vars->pipes->heredoc_delim);
//         vars->pipes->heredoc_delim = NULL;
//     }
    
//     // Process quotes and determine expansion mode
//     was_quoted = strip_outer_quotes(&delimiter_copy);
    
//     // Store the processed delimiter and expansion flag
//     vars->pipes->heredoc_delim = delimiter_copy;
//     vars->pipes->hd_expand = !was_quoted; // Expand if NOT quoted
    
//     fprintf(stderr, "[DBG_HEREDOC] Delimiter processed: '%s', expand_vars=%d\n", 
//             vars->pipes->heredoc_delim, vars->pipes->hd_expand);
            
//     return 1;
// }

// /**
//  * Extracts and processes a heredoc delimiter from input string.
//  * Combines extraction and processing in one function.
//  * 
//  * @param input The input string containing a heredoc operator
//  * @param vars Program state variables to update
//  * @return The extracted and processed delimiter, or NULL if error
//  */
// char *get_heredoc_delimiter(char *input, t_vars *vars)
// {
//     char *raw_delimiter;
    
//     raw_delimiter = extract_heredoc_delimiter(input, vars);
//     if (!raw_delimiter)
//         return NULL;
    
//     // Process the delimiter and update vars
//     if (!process_heredoc_info(raw_delimiter, vars))
//     {
//         free(raw_delimiter);
//         return NULL;
//     }
    
//     // Return a copy of the processed delimiter
//     return ft_strdup(vars->pipes->heredoc_delim);
// }
// char *get_heredoc_delimiter(char *input, t_vars *vars)
// {
//     char *raw_delimiter;
//     char *result;
    
//     raw_delimiter = extract_heredoc_delimiter(input, vars);
//     if (!raw_delimiter)
//         return NULL;
    
//     // Process the delimiter and update vars
//     if (!process_heredoc_info(raw_delimiter, vars))
//     {
//         free(raw_delimiter);
//         return NULL;
//     }
    
//     // Free the raw delimiter after processing
//     free(raw_delimiter);
    
//     // Return a copy of the processed delimiter
//     result = ft_strdup(vars->pipes->heredoc_delim);
//     return result;
// }
/*
Extracts and processes a heredoc delimiter from input string.
Handles all aspects of delimiter extraction and processing.
Returns:
- The extracted and processed delimiter.
- NULL if error.
*/
// char *get_heredoc_delimiter(char *input, t_vars *vars)
// {
//     char *delimiter;
//     char *result;
//     int was_quoted;
    
//     // Extract raw delimiter from input
//     delimiter = extract_heredoc_delimiter(input, vars);
//     if (!delimiter || !vars || !vars->pipes)
//         return NULL;
//     // Clean up any existing delimiter
//     if (vars->pipes->heredoc_delim)
//     {
//         free(vars->pipes->heredoc_delim);
//         vars->pipes->heredoc_delim = NULL;
//     }
//     // Process quotes and determine expansion mode
//     was_quoted = strip_outer_quotes(&delimiter);
//     // Store the processed delimiter and expansion flag
//     vars->pipes->heredoc_delim = delimiter;
//     vars->pipes->hd_expand = !was_quoted; // Expand if NOT quoted
//     fprintf(stderr, "[DBG_HEREDOC] Delimiter processed: '%s', expand_vars=%d\n", 
//             vars->pipes->heredoc_delim, vars->pipes->hd_expand);
//     // Return a copy of the processed delimiter
//     result = ft_strdup(vars->pipes->heredoc_delim);
//     return result;
// }
// void get_heredoc_delimiter(char *input, t_vars *vars)
// {
//     char *delimiter;
//     int was_quoted;
//     size_t len;
    
//     // Extract raw delimiter from input
//     delimiter = extract_heredoc_delimiter(input, vars);
//     if (!delimiter || !vars || !vars->pipes)
//         return;
    
//     // Clean up any existing delimiter
//     if (vars->pipes->heredoc_delim)
//     {
//         free(vars->pipes->heredoc_delim);
//         vars->pipes->heredoc_delim = NULL;
//     }
    
//     // Check if the delimiter is quoted without modifying it first
//     len = ft_strlen(delimiter);
//     was_quoted = 0;
    
//     if (len >= 2 && ft_isquote(delimiter[0]) && delimiter[0] == delimiter[len-1])
//     {
//         was_quoted = 1;
//         // Now strip the quotes for storage
//         char *unquoted = ft_substr(delimiter, 1, len-2);
//         if (unquoted)
//         {
//             free(delimiter);
//             delimiter = unquoted;
//         }
//     }
    
//     // Store the processed delimiter and expansion flag
//     vars->pipes->heredoc_delim = delimiter;
//     vars->pipes->hd_expand = !was_quoted; // Expand if NOT quoted
    
//     fprintf(stderr, "[DBG_HEREDOC] Delimiter processed: '%s', expand_vars=%d\n", 
//             vars->pipes->heredoc_delim, vars->pipes->hd_expand);
// }

/*
Processes a heredoc from stored lines or interactive input.
Returns 1 on success, 0 on failure.
Updated process_heredoc with fixed variable declarations and logic
*/
// int process_heredoc(t_node *node, t_vars *vars)
// {
// char *delimiter;

// fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Processing heredoc for node: %p, mode=%d\n", 
// 		(void *)node, vars->heredoc_mode);

// if (!node || !node->right || !node->right->args || !node->right->args[0])
// {
// 	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Invalid node structure for heredoc\n");
// 	return 0;
// }

// // Get delimiter from right node
// delimiter = node->right->args[0];

// // Set expansion flag based on node or fallback to string check
// if (vars->pipes->last_heredoc == node)
// {
// 	// Keep existing flag value if this node was already processed
// 	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Using existing expansion flag: %d\n", 
// 			vars->pipes->hd_expand);
// }
// else
// {
// 	// Otherwise, determine based on delimiter content
// 	vars->pipes->hd_expand = chk_expand_heredoc(delimiter);
// 	// Store the node for future reference
// 	vars->pipes->last_heredoc = node;
	
// 	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Setting expansion flag to: %d based on delimiter\n", 
// 			vars->pipes->hd_expand);
// }

// fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Delimiter: '%s', expand_vars: %d\n", 
// 		delimiter, vars->pipes->hd_expand);

// // Check heredoc_mode early to decide processing approach
// if (vars->heredoc_mode == 1)
// {
// 	// Content already in temp file, just open it for reading
// 	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Using stored heredoc content from temp file\n");
// 	int fd = open(TMP_BUF, O_RDONLY);
// 	if (fd == -1) {
// 		fprintf(stderr, "[DBG_HEREDOC] process_heredoc().WARNING: Failed to open temp file: %s\n", 
// 				strerror(errno));
// 		return (0);
// 	}
// 	// Store the file descriptor for redirection
// 	vars->pipes->heredoc_fd = fd;
// }
// else
// {
// 	// Interactive mode - read from terminal
// 	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Reading heredoc input from terminal\n");
	
// 	// Open temp file for writing
// 	int fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
// 	if (fd == -1)
// 	{
// 		fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Failed to create temp file: %s\n", 
// 				strerror(errno));
// 		return (0);
// 	}
	
// 	// Read lines until delimiter is found
// 	char *line;
// 	while (1)
// 	{
// 		line = readline("heredoc> ");
// 		if (!line || ft_strcmp(line, delimiter) == 0)
// 		{
// 			free(line);
// 			break;
// 		}
// 		write_to_heredoc(fd, line, vars, vars->pipes->hd_expand);
// 		free(line);
// 	}
	
// 	// Close write end
// 	close(fd);
	
// 	// Open the file for reading
// 	fd = open(TMP_BUF, O_RDONLY);
// 	if (fd == -1)
// 	{
// 		fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Failed to open temp file for reading: %s\n", 
// 				strerror(errno));
// 		return (0);
// 	}
// 	vars->pipes->heredoc_fd = fd;
// }

// fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Heredoc content ready on fd=%d\n", 
// 		vars->pipes->heredoc_fd);
// return (1);
// }
/*
MODIFIED: Processes heredoc based on mode.
Mode 0 (Interactive): Reads input via readline, compares with stored delimiter, writes to TMP_BUF using stored expansion flag.
Mode 1 (Multiline): Opens the pre-filled TMP_BUF for reading.
Sets vars->pipes->heredoc_fd to the READ fd of TMP_BUF on success.
*/
int process_heredoc(t_node *node, t_vars *vars)
{
    int		fd = -1;
    char	*line = NULL;

    // Basic validation
    if (!node || !vars || !vars->pipes || !vars->pipes->heredoc_delim)
    {
        fprintf(stderr, "[DBG_HEREDOC] process_heredoc() Invalid state (node or delimiter missing).\n");
        vars->error_code = ERR_DEFAULT; // Internal error
        return (0);
    }
    fprintf(stderr, "[DBG_HEREDOC] process_heredoc(). Mode=%d, Delim='%s', Expand=%d\n",
            vars->heredoc_mode, vars->pipes->heredoc_delim, vars->pipes->hd_expand);
    // Ensure previous fd is closed if reusing
    if (vars->pipes->heredoc_fd >= 0) {
        close(vars->pipes->heredoc_fd);
        vars->pipes->heredoc_fd = -1;
    }

    if (vars->heredoc_mode == 1)
    { // Content already stored in TMP_BUF by store_multiline_heredoc_content
        fprintf(stderr, "[DBG_HEREDOC] Using stored heredoc content from temp file.\n");
        fd = open(TMP_BUF, O_RDONLY);
        if (fd == -1)
        {
            perror("bleshell: failed to open temp heredoc file for reading");
            vars->error_code = ERR_DEFAULT;
            unlink(TMP_BUF); // Attempt cleanup
            return (0); // Failure
        }
        vars->pipes->heredoc_fd = fd; // Store the READ fd
    }
    else
    { // Interactive mode (heredoc_mode == 0)
        fprintf(stderr, "[DBG_HEREDOC] Reading heredoc input interactively.\n");
        fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (fd == -1)
        {
            perror("bleshell: failed to create temp heredoc file for writing");
            vars->error_code = ERR_DEFAULT;
            return (0); // Failure
        }
        while (1)
        {
            line = readline("heredoc> ");
            if (!line)
            { // EOF (Ctrl+D)
                fprintf(stderr, "\nbleshell: warning: here-document delimited by end-of-file (wanted `%s')\n", vars->pipes->heredoc_delim);
                break;
            }
            // Exact match comparison using the stored delimiter
            if (ft_strcmp(line, vars->pipes->heredoc_delim) == 0)
            {
                free(line);
                break; // Delimiter found
            }
            // Write line using the stored expansion flag
            if (!write_to_heredoc(fd, line, vars))
            {
                // write_to_heredoc sets error_code and prints perror
                free(line);
                close(fd); // Close write fd on error
                unlink(TMP_BUF); // Attempt to remove partial file
                return (0); // Failure reported by write_to_heredoc
            }
            free(line);
            line = NULL;
        }
        close(fd); // Close write end
        // Re-open TMP_BUF for reading
        fd = open(TMP_BUF, O_RDONLY);
        if (fd == -1)
        {
            perror("bleshell: failed to open temp heredoc file for reading after interactive input");
            vars->error_code = ERR_DEFAULT;
            unlink(TMP_BUF); // Attempt to remove file
            return (0); // Failure
        }
        vars->pipes->heredoc_fd = fd; // Store the READ fd
    }
    fprintf(stderr, "[DBG_HEREDOC] Heredoc content ready on fd=%d\n", vars->pipes->heredoc_fd);
    return (1); // Success
}

/*
NEW: Validates the raw heredoc delimiter string according to strict rules.
Sets vars->pipes->heredoc_delim and vars->pipes->hd_expand on success.
Returns 1 on success, 0 on failure (syntax error).
*/
int validate_heredoc_delimiter(char *raw_delimiter, t_vars *vars)
{
    char	*final_delimiter = NULL;
    int		was_quoted = 0;
    size_t	len;

    if (!raw_delimiter)
        return (0); // Should not happen if called correctly
    len = ft_strlen(raw_delimiter);
    // --- Strict Quote Validation ---
    if (len == 2 && raw_delimiter[0] == '\"' && raw_delimiter[1] == '\"')
    { // Case: ""
        final_delimiter = ft_strdup(""); // Empty delimiter
        was_quoted = 1;
    }
    else if (len >= 2 && raw_delimiter[0] == '\"' && raw_delimiter[len - 1] == '\"')
    { // Case: "..."
        final_delimiter = ft_substr(raw_delimiter, 1, len - 2);
        was_quoted = 1;
    }
    else if (len >= 2 && raw_delimiter[0] == '\'' && raw_delimiter[len - 1] == '\'')
    { // Case: '...'
        final_delimiter = ft_substr(raw_delimiter, 1, len - 2);
        was_quoted = 1;
    }
    else
    { // Case: Unquoted or invalid
        for (size_t i = 0; i < len; ++i)
        {
            if (raw_delimiter[i] == '\'' || raw_delimiter[i] == '\"')
            {
                fprintf(stderr, "bleshell: syntax error near unexpected token `%.*s'\n", (int)len, raw_delimiter);
                vars->error_code = ERR_SYNTAX;
                return (0); // Invalid: quote inside unquoted delimiter
            }
        }
        // Treat as literal if no internal quotes found
        final_delimiter = ft_strdup(raw_delimiter);
        was_quoted = 0;
    }
    if (!final_delimiter)
    {
        perror("bleshell: malloc error during delimiter processing");
        vars->error_code = ERR_DEFAULT;
        return (0);
    }
    // --- Store results ---
    if (vars->pipes->heredoc_delim)
        free(vars->pipes->heredoc_delim); // Free previous delimiter if any
    vars->pipes->heredoc_delim = final_delimiter;
    vars->pipes->hd_expand = !was_quoted;
    fprintf(stderr, "[DBG_HEREDOC] Validated Delimiter: '%s', expand_vars=%d\n",
            vars->pipes->heredoc_delim, vars->pipes->hd_expand);
    return (1); // Success
}
