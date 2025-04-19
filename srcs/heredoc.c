/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/19 22:22:02 by bleow            ###   ########.fr       */
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
- Uses handle_expansion() to perform the expansion.
- Falls back to returning "$" if expansion fails.
Returns:
Newly allocated string with expanded variable.
"$" if variable can't be expanded.
Works with expand_one_line().

Example: At position of "$HOME" in a line
- Returns: "/Users/username" (expanded value)
- Updates position to after variable name
*/
char	*expand_heredoc_var(char *line, int *pos, t_vars *vars)
{
	char	*expanded;
	int		old_pos;

	old_pos = *pos;
	expanded = handle_expansion(line, pos, vars);
	if (!expanded)
	{
		*pos = old_pos + 1;
		return (ft_strdup("$"));
	}
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

/*
Checks if a heredoc delimiter contains quotes.
- Examines delimiter character by character.
- Determines if variables should be expanded.
Returns:
1 if variables should be expanded (no quotes).
0 if variables shouldn't be expanded (has quotes).
Works with handle_heredoc().

Example: "EOF" -> 1 (expand variables)
		 "'EOF'" -> 0 (don't expand variables)
*/
int	chk_expand_heredoc(char *delimiter)
{
	int	i;

	if (!delimiter)
		return (0);
	i = 0;
	while (delimiter[i])
	{
		if (ft_isquote(delimiter[i]))
			return (0);
		i++;
	}
	return (1);
}

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
/**
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

int store_heredoc_content(char *input, int start_pos, char *delimiter, t_vars *vars)
{
    int i = start_pos;
    int line_start;
    int line_end;
    // char *line;
    int found_delimiter = 0;
    int fd;
    
    // Open temporary file for writing
    fprintf(stderr, "[DBG_HEREDOC] Opening temporary file for heredoc content\n");
    fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1)
    {
        fprintf(stderr, "[DBG_HEREDOC] Failed to open temp file: %s\n", strerror(errno));
        return (0);
    }
    
    // Process lines and write to temp file
    while (input[i])
    {
        line_start = i;
        // Find end of this line
        while (input[i] && input[i] != '\n')
            i++;
        line_end = i;
        
        // Check if this line is the delimiter
        if (ft_strlen(delimiter) == (size_t)(line_end - line_start) &&
            !ft_strncmp(&input[line_start], delimiter, line_end - line_start))
        {
            fprintf(stderr, "[DBG_HEREDOC] Found delimiter '%s' at position %d\n", 
                  delimiter, line_start);
            found_delimiter = 1;
            break;
        }
        
        // Extract this line
        // line = ft_substr(input, line_start, line_end - line_start);
        // if (line)
		// Extract this line
		char *line = ft_substr(input, line_start, line_end - line_start);
		if (line)
		{
    		// Write to file with optional variable expansion
    		fprintf(stderr, "[DBG_HEREDOC] Writing line to temp file: '%s'\n", line);
    		write_to_heredoc(fd, line, vars, chk_expand_heredoc(delimiter));
    		free(line); // Free the temporary line
		}
        if (input[i] == '\n')
            i++;
    }
    // Close the file when done writing
    fprintf(stderr, "[DBG_HEREDOC] Closing temp file\n");
    close(fd);
    if (found_delimiter)
    {
        fprintf(stderr, "[DBG_HEREDOC] Content stored in temp file, setting heredoc_mode=1\n");
        vars->heredoc_mode = 1; // Mark as multiline mode
        return (1);
    }
    else
    {
        fprintf(stderr, "[DBG_HEREDOC] No delimiter found\n");
        // File left in place but we'll overwrite it next time
        return (0);
    }
}

/*
Writes line to heredoc pipe with variable expansion.
- Handles newline addition to each input line.
- Optionally expands variables based on delimiter quotes.
- Manages all write operations and error handling.
Returns:
1 on successful write.
0 on any failure.
Works with read_heredoc().

Example: Line "echo $HOME" with expand_vars=true
- Expands to "echo /Users/username"
- Writes expanded content plus newline to fd
*/
int write_to_heredoc(int fd, char *line, t_vars *vars, int expand_vars)
{
	char	*expanded_line;
	int		write_result;
	
	if (!line)
		return (0);
	if (expand_vars == 1)
	{
		expanded_line = expand_heredoc_line(line, vars);
		if (!expanded_line)
			return (0);
	}
	else
	{
		expanded_line = ft_strdup(line);
		if (!expanded_line)
			return (0);
	}
	// Write the line to the pipe with newline
	write_result = write(fd, expanded_line, ft_strlen(expanded_line));
	write(fd, "\n", 1);
	free(expanded_line);
	if (write_result == -1)
		return (0);
	else
		return (1);
}

/*
Reads input for heredoc until delimiter is encountered.
- Prompts user for input lines with "> ".
- Compares each line against delimiter.
- Writes valid lines to the pipe.
Returns:
1 when completed successfully.
0 on any error.
Works with handle_heredoc().

Example: With delimiter "EOF"
- Reads lines like "Hello", "$USER", "EOF"
- Writes "Hello" and expanded "$USER" to pipe
- Stops at "EOF" line, returning 1
*/
int read_heredoc(int *fd, char *delimiter, t_vars *vars, int expand_vars)
{
    DBG_PRINTF(1, "Reading heredoc with delimiter: '%s'\n", delimiter);
    
    // First check if we already have a heredoc pipe set up
    if (vars->pipes->heredoc_fd > 0)
    {
        DBG_PRINTF(1, "Using existing heredoc pipe (fd=%d)\n", vars->pipes->heredoc_fd);
        // Just pass along the existing file descriptor
        fd[0] = vars->pipes->heredoc_fd;
        vars->pipes->heredoc_fd = -1; // Transfer ownership
        return (1);
    }
    
    // Otherwise, create a new pipe and collect input interactively
    if (pipe(fd) == -1)
        return (handle_heredoc_err(vars));
        
    DBG_PRINTF(1, "Entering interactive heredoc mode\n");
    
    // Your existing interactive heredoc code...
    while (1)
    {
        char *line = readline(PROMPT);
        if (!line)
        {
            DBG_PRINTF(1, "EOF in heredoc\n");
            close(fd[1]);
            return 0;
        }
        
        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            close(fd[1]);
            return 1;
        }
        
        write_to_heredoc(fd[1], line, vars, expand_vars);
        free(line);
    }
}

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

int handle_heredoc(t_node *node, t_vars *vars)
{
    fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Setting up heredoc for command\n");
    
    // Process heredoc content if needed
    if (!process_heredoc(node, vars))
        return (0);
    
    // Connect the pipe to stdin
    if (dup2(vars->pipes->heredoc_fd, STDIN_FILENO) == -1) {
        fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Failed to redirect stdin: %s\n", strerror(errno));
        close(vars->pipes->heredoc_fd);
        return (0);
    }
    
    // Close the read end as it's now duplicated to stdin
    close(vars->pipes->heredoc_fd);
    vars->pipes->heredoc_fd = -1;  // Mark as used
    
    fprintf(stderr, "[DBG_HEREDOC] handle_heredoc() Heredoc connected to stdin\n");
    return (1);
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

/**
 * Processes a heredoc delimiter: removes quotes if present and sets expansion flag.
 * This is the central function for all delimiter processing.
 * 
 * @param raw_delimiter The raw delimiter string extracted from input
 * @param vars Program state variables to update
 * @return 1 if successfully processed, 0 on error
 */
int process_heredoc_info(char *raw_delimiter, t_vars *vars)
{
    char *delimiter_copy;
    int was_quoted;
    
    if (!raw_delimiter || !vars || !vars->pipes)
        return 0;
        
    // Make a copy we can safely modify
    delimiter_copy = ft_strdup(raw_delimiter);
    if (!delimiter_copy)
        return 0;
        
    // Clean up any existing delimiter
    if (vars->pipes->heredoc_delim)
    {
        free(vars->pipes->heredoc_delim);
        vars->pipes->heredoc_delim = NULL;
    }
    
    // Process quotes and determine expansion mode
    was_quoted = strip_outer_quotes(&delimiter_copy);
    
    // Store the processed delimiter and expansion flag
    vars->pipes->heredoc_delim = delimiter_copy;
    vars->pipes->hd_expand = !was_quoted; // Expand if NOT quoted
    
    fprintf(stderr, "[DBG_HEREDOC] Delimiter processed: '%s', expand_vars=%d\n", 
            vars->pipes->heredoc_delim, vars->pipes->hd_expand);
            
    return 1;
}

/**
 * Extracts and processes a heredoc delimiter from input string.
 * Combines extraction and processing in one function.
 * 
 * @param input The input string containing a heredoc operator
 * @param vars Program state variables to update
 * @return The extracted and processed delimiter, or NULL if error
 */
char *get_heredoc_delimiter(char *input, t_vars *vars)
{
    char *raw_delimiter;
    
    raw_delimiter = extract_heredoc_delimiter(input, vars);
    if (!raw_delimiter)
        return NULL;
    
    // Process the delimiter and update vars
    if (!process_heredoc_info(raw_delimiter, vars))
    {
        free(raw_delimiter);
        return NULL;
    }
    
    // Return a copy of the processed delimiter
    return ft_strdup(vars->pipes->heredoc_delim);
}

/*
Processes a heredoc from stored lines or interactive input.
Returns 1 on success, 0 on failure.
*/
// int process_heredoc(t_node *node, t_vars *vars)
// {
//     char *delimiter;
//     int expand_vars;
    
//     fprintf(stderr, "[DBG_HEREDOC] heredoc_mode at start of process_heredoc: %d\n", vars->heredoc_mode);
//     fprintf(stderr, "[DBG_HEREDOC] Processing heredoc for node: %p, mode=%d\n", 
//             (void*)node, vars->heredoc_mode);
    
//     // Validate node structure
//     if (!node || !node->right || !node->right->args || !node->right->args[0])
//     {
//         fprintf(stderr, "[DBG_HEREDOC] Invalid node structure\n");
//         return (0);
//     }
    
//     // Get delimiter from right node
//     delimiter = node->right->args[0];
    
//     // Determine if variables should be expanded
//     expand_vars = !is_quoted_delimiter(delimiter);
//     fprintf(stderr, "[DBG_HEREDOC] Delimiter: '%s', expand_vars: %d\n", 
//             delimiter, expand_vars);
    
//     // Check heredoc_mode early to decide processing approach
//     if (vars->heredoc_mode == 1)
//     {
//         // Content already in temp file, just open it for reading
//         fprintf(stderr, "[DBG_HEREDOC] Using stored heredoc content from temp file\n");
//         int fd = open(TMP_BUF, O_RDONLY);
//         if (fd == -1) {
//             fprintf(stderr, "[DBG_HEREDOC] WARNING: Failed to open temp file: %s\n", 
//                     strerror(errno));
//             return (0);
//         }
//         // Store the file descriptor for redirection
//         vars->pipes->heredoc_fd = fd;
//     }
//     else
//     {
//         // Interactive mode - read from terminal
//         fprintf(stderr, "[DBG_HEREDOC] Reading heredoc input from terminal\n");
        
//         // Open temp file for writing
//         int fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
//         if (fd == -1)
//         {
//             fprintf(stderr, "[DBG_HEREDOC] Failed to create temp file: %s\n", 
//                     strerror(errno));
//             return (0);
//         }
        
//         // Read lines until delimiter is found
//         char *line;
//         while (1)
//         {
//             line = readline("heredoc> ");
//             if (!line || ft_strcmp(line, delimiter) == 0)
//             {
//                 free(line);
//                 break;
//             }
//             write_to_heredoc(fd, line, vars, expand_vars);
//             free(line);
//         }
        
//         // Close write end
//         close(fd);
        
//         // Open the file for reading
//         fd = open(TMP_BUF, O_RDONLY);
//         if (fd == -1)
//         {
//             fprintf(stderr, "[DBG_HEREDOC] Failed to open temp file for reading: %s\n", 
//                     strerror(errno));
//             return (0);
//         }
//         vars->pipes->heredoc_fd = fd;
//     }
    
//     fprintf(stderr, "[DBG_HEREDOC] Heredoc content ready on fd=%d\n", 
//             vars->pipes->heredoc_fd);
//     return (1);
// }

// int process_heredoc(t_node *node, t_vars *vars)
// {
//     fprintf(stderr, "[DBG_HEREDOC] Processing heredoc for node: %p, mode=%d\n", 
//             (void *)node, vars->heredoc_mode);
    
//     if (!node || !node->right || !node->right->args || !node->right->args[0])
//     {
//         fprintf(stderr, "[DBG_HEREDOC] Invalid node structure for heredoc\n");
//         return 0;
//     }
    
//     // Get delimiter from right node
//     delimiter = node->right->args[0];
    
//     // Determine if variables should be expanded
//     expand_vars = !is_quoted_delimiter(delimiter);
//     fprintf(stderr, "[DBG_HEREDOC] Delimiter: '%s', expand_vars: %d\n", 
//             delimiter, expand_vars);
    
//     // Check heredoc_mode early to decide processing approach
//     if (vars->heredoc_mode == 1)
//     {
//         // Content already in temp file, just open it for reading
//         fprintf(stderr, "[DBG_HEREDOC] Using stored heredoc content from temp file\n");
//         int fd = open(TMP_BUF, O_RDONLY);
//         if (fd == -1) {
//             fprintf(stderr, "[DBG_HEREDOC] WARNING: Failed to open temp file: %s\n", 
//                     strerror(errno));
//             return (0);
//         }
//         // Store the file descriptor for redirection
//         vars->pipes->heredoc_fd = fd;
//     }
//     else
//     {
//         // Interactive mode - read from terminal
//         fprintf(stderr, "[DBG_HEREDOC] Reading heredoc input from terminal\n");
        
//         // Open temp file for writing
//         int fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
//         if (fd == -1)
//         {
//             fprintf(stderr, "[DBG_HEREDOC] Failed to create temp file: %s\n", 
//                     strerror(errno));
//             return (0);
//         }
        
//         // Read lines until delimiter is found
//         char *line;
//         while (1)
//         {
//             line = readline("heredoc> ");
//             if (!line || ft_strcmp(line, delimiter) == 0)
//             {
//                 free(line);
//                 break;
//             }
//             write_to_heredoc(fd, line, vars, expand_vars);
//             free(line);
//         }
        
//         // Close write end
//         close(fd);
        
//         // Open the file for reading
//         fd = open(TMP_BUF, O_RDONLY);
//         if (fd == -1)
//         {
//             fprintf(stderr, "[DBG_HEREDOC] Failed to open temp file for reading: %s\n", 
//                     strerror(errno));
//             return (0);
//         }
//         vars->pipes->heredoc_fd = fd;
//     }
    
//     fprintf(stderr, "[DBG_HEREDOC] Heredoc content ready on fd=%d\n", 
//             vars->pipes->heredoc_fd);
//     return (1);
// }
// return is_quoted_delimiter_str(node->right->args[0]);
    
// return 0;
// }

// Updated process_heredoc with fixed variable declarations and logic
int process_heredoc(t_node *node, t_vars *vars)
{
char *delimiter;

fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Processing heredoc for node: %p, mode=%d\n", 
		(void *)node, vars->heredoc_mode);

if (!node || !node->right || !node->right->args || !node->right->args[0])
{
	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Invalid node structure for heredoc\n");
	return 0;
}

// Get delimiter from right node
delimiter = node->right->args[0];

// Set expansion flag based on node or fallback to string check
if (vars->pipes->last_heredoc == node)
{
	// Keep existing flag value if this node was already processed
	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Using existing expansion flag: %d\n", 
			vars->pipes->hd_expand);
}
else
{
	// Otherwise, determine based on delimiter content
	vars->pipes->hd_expand = chk_expand_heredoc(delimiter);
	// Store the node for future reference
	vars->pipes->last_heredoc = node;
	
	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Setting expansion flag to: %d based on delimiter\n", 
			vars->pipes->hd_expand);
}

fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Delimiter: '%s', expand_vars: %d\n", 
		delimiter, vars->pipes->hd_expand);

// Check heredoc_mode early to decide processing approach
if (vars->heredoc_mode == 1)
{
	// Content already in temp file, just open it for reading
	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Using stored heredoc content from temp file\n");
	int fd = open(TMP_BUF, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "[DBG_HEREDOC] process_heredoc().WARNING: Failed to open temp file: %s\n", 
				strerror(errno));
		return (0);
	}
	// Store the file descriptor for redirection
	vars->pipes->heredoc_fd = fd;
}
else
{
	// Interactive mode - read from terminal
	fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Reading heredoc input from terminal\n");
	
	// Open temp file for writing
	int fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
	{
		fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Failed to create temp file: %s\n", 
				strerror(errno));
		return (0);
	}
	
	// Read lines until delimiter is found
	char *line;
	while (1)
	{
		line = readline("heredoc> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break;
		}
		write_to_heredoc(fd, line, vars, vars->pipes->hd_expand);
		free(line);
	}
	
	// Close write end
	close(fd);
	
	// Open the file for reading
	fd = open(TMP_BUF, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Failed to open temp file for reading: %s\n", 
				strerror(errno));
		return (0);
	}
	vars->pipes->heredoc_fd = fd;
}

fprintf(stderr, "[DBG_HEREDOC] process_heredoc().Heredoc content ready on fd=%d\n", 
		vars->pipes->heredoc_fd);
return (1);
}
