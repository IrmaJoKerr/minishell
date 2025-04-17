/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 02:41:39 by bleow             #+#    #+#             */
/*   Updated: 2025/04/17 21:50:24 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// /*
// Processes heredoc input from command array
// Stores heredoc lines in vars->heredoc_lines
// Executes the heredoc command
// */
// void	proc_heredoc_input(char **cmdarr, int line_count, t_vars *vars)
// {
// 	int	i;

// 	// Free existing heredoc lines if any
// 	if (vars->heredoc_lines)
// 		ft_free_2d(vars->heredoc_lines, vars->heredoc_count);
// 	// Set up new heredoc storage
// 	vars->heredoc_count = line_count - 1;
// 	vars->heredoc_lines = malloc(sizeof(char*) * vars->heredoc_count);
// 	vars->heredoc_index = 0;
// 	// Store heredoc lines
// 	if (vars->heredoc_lines)
// 	{
// 		DBG_PRINTF(1, "Storing %d lines for heredoc\n", vars->heredoc_count);
// 		i = 0;
// 		while (i < vars->heredoc_count)
// 		{
// 			vars->heredoc_lines[i] = ft_strdup(cmdarr[i+1]);
// 			DBG_PRINTF(1, "  Stored: '%s'\n", vars->heredoc_lines[i]);
// 			i++;
// 		}
// 	}
// 	// Process the heredoc command
// 	DBG_PRINTF(1, "Processing heredoc command: '%s'\n", cmdarr[0]);
// 	add_history(cmdarr[0]);
// 	process_command(cmdarr[0], vars);
// }

/* FLAG FOR REUSE. CAN BE USED FOR PASTED INPUT
Processes multiple command lines from command array
Adds commands to history and processes them
*/
void	proc_multiline_input(char **cmdarr, int line_count, t_vars *vars)
{
	int	i;
	
	i = 0;
	while (i < line_count)
	{
		if (*cmdarr[i])
		{
			DBG_PRINTF(1, "Processing command: '%s'\n", cmdarr[i]);
			add_history(cmdarr[i]);
			process_command(cmdarr[i], vars);
		}
		i++;
	}
}

/*
Handles user input and processes commands
Manages both single-line and multi-line inputs
Detects and processes heredoc inputs
*/
// void	handle_input(char *input, t_vars *vars)
// {
// 	char	**cmdarr;
// 	int		count;
	
// 	if (!input || !*input)
// 		return;
// 	if (ft_strchr(input, '\n'))
// 	{
// 		DBG_PRINTF(1, "Multi-line input detected\n");
// 		cmdarr = ft_split(input, '\n');
// 		if (!cmdarr)
// 			return ;
// 		count = ft_arrlen(cmdarr);
// 		if (count > 1 && ft_strnstr(cmdarr[0], "<<" , ft_strlen(cmdarr[0])))
// 			proc_heredoc_input(cmdarr, count, vars);
// 		else
// 		{
// 			proc_multiline_input(cmdarr, count, vars);
// 		}
// 		ft_free_2d(cmdarr, count);
// 	}
// 	else
// 	{
// 		DBG_PRINTF(1, "Single-line input: '%s'\n", input);
// 		process_command(input, vars);
// 	}
// }

/*
Completes input based on its determined mode.
Handles quote completion, pipe completion, etc.
*/
char	*complete_input(char *input, t_inmode mode, t_vars *vars)
{
	char	*result;
	
    if (mode == INPUT_QUOTE_COMPLETION)
		result = fix_open_quotes(input, vars);
    else if (mode == INPUT_PIPE_COMPLETION)      
        result = complete_pipe_command(input, vars);
    else if (mode == INPUT_HEREDOC_MODE)
	{
        setup_heredoc_mode(input, vars);
        result = ft_strdup(input);
	}
    else if (mode == INPUT_NORMAL)
		result = ft_strdup(input);
	return (result);
}

void	clear_partial_input(t_vars *vars)
{
	if (vars->partial_input)
	{
		free(vars->partial_input);
		vars->partial_input = NULL;
	}
}

/*
Checks if an input string contains a heredoc operator '<<'
outside of quotes.

Parameters:
- input: The string to check

Returns:
- 1 if a heredoc operator is found, 0 otherwise
*/
int has_heredoc_operator(char *input)
{
    int i;
    int in_quotes;
    char quote_char;
    
    if (!input)
        return 0;
    
    i = 0;
    in_quotes = 0;
    quote_char = 0;
    
    while (input[i])
    {
        // Handle quote tracking
        if ((input[i] == '\'' || input[i] == '\"') && 
            (!in_quotes || quote_char == input[i]))
        {
            if (in_quotes && quote_char == input[i])
                in_quotes = 0;
            else if (!in_quotes)
            {
                in_quotes = 1;
                quote_char = input[i];
            }
        }
        // Look for << outside of quotes
        else if (!in_quotes && input[i] == '<' && input[i + 1] == '<')
        {
            return 1; // Found heredoc operator
        }
        i++;
    }
    return 0;
}

/*
Processes a line of heredoc content, handling variable expansion
if needed.

Parameters:
- line: The line of text to process
- vars: Program state variables

Returns:
- Processed line with variables expanded (newly allocated)
- Original line duplicated if no expansion needed
*/
char *process_heredoc_line(char *line, t_vars *vars)
{
    // If we don't have a line, return NULL
    if (!line)
        return NULL;
        
    // Handle variable expansion for heredoc content
    if (vars->pipes->heredoc_delim
		&& !is_quoted_delimiter(vars->pipes->heredoc_delim))
	{
        return expand_heredoc_line(line, vars);
	}
    // No expansion needed, just duplicate the line
    return ft_strdup(line);
}

/*
Helper to process a command that has a heredoc.
Sets up heredoc mode and processes the command.

Parameters:
- cmd_line: The command line that contains a heredoc operator
- vars: Program state variables

Returns: void
*/
void process_command_with_heredoc(char *cmd_line, t_vars *vars)
{
    fprintf(stderr, "[DEBUG] process_cmd_heredoc: Processing command with heredoc\n");
    
    // Set up for heredoc usage
    vars->heredoc_mode = 1;
    
    // Process the command line normally
    process_single_command(cmd_line, vars);
    
    // Reset heredoc state after command completes
    vars->heredoc_mode = 0;
    vars->heredoc_active = 0;
    if (vars->pipes->heredoc_delim)
    {
        free(vars->pipes->heredoc_delim);
        vars->pipes->heredoc_delim = NULL;
    }
    
    fprintf(stderr, "[DEBUG] process_cmd_heredoc: Command processed, heredoc state reset\n");
}

/*
Non-recursive helper that processes a single command line.
Contains the core of handle_input without the multiline detection.

Parameters:
- input: Single line of input to process
- vars: Program state variables

Returns: void
*/
void process_single_command(char *input, t_vars *vars)
{
    char *completed_input;
    t_inmode mode;
    
    fprintf(stderr, "[DEBUG] process_single_command: Processing '%s'\n", input);
    
    if (!input || !*input)
        return;
    
    // Duplicate input to partial_input
    vars->partial_input = ft_strdup(input);
    if (!vars->partial_input)
        return;
    
    // Check if we're in heredoc continuation
    if (vars->heredoc_active)
    {
        fprintf(stderr, "[DEBUG] process_single_command: Continuing heredoc input\n");
        process_heredoc_continuation(vars->partial_input, vars);
        clear_partial_input(vars);
        return;
    }
    
    // Check input state and complete as needed
    mode = check_input_state(vars->partial_input, vars);
    fprintf(stderr, "[DEBUG] process_single_command: Input mode: %d\n", mode);
    
    completed_input = complete_input(vars->partial_input, mode, vars);
    if (!completed_input)
    {
        clear_partial_input(vars);
        return;
    }
    
    if (completed_input != vars->partial_input)
    {
        free(vars->partial_input);
        vars->partial_input = completed_input;
    }
    
    // Process tokens and execute
    if (!process_input_tokens(vars->partial_input, vars))
    {
        clear_partial_input(vars);
        return;
    }
    
    build_and_execute(vars);
    clear_partial_input(vars);
}

/*
Helper function to read an entire file into memory.
Returns a newly allocated string containing the file contents,
or NULL if an error occurred.
*/
char *read_entire_file(const char *filename)
{
    int fd;
    char *content = NULL;
    struct stat file_stats;
    ssize_t bytes_read;
    
    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "[ERROR] read_entire_file: Failed to open file %s: %s\n", 
                filename, strerror(errno));
        return NULL;
    }
    
    // Get file size
    if (fstat(fd, &file_stats) == -1)
    {
        fprintf(stderr, "[ERROR] read_entire_file: Failed to stat file %s: %s\n", 
                filename, strerror(errno));
        close(fd);
        return NULL;
    }
    
    // Allocate memory for file content plus null terminator
    content = malloc(file_stats.st_size + 1);
    if (!content)
    {
        close(fd);
        return NULL;
    }
    
    // Read entire file
    bytes_read = read(fd, content, file_stats.st_size);
    if (bytes_read == -1)
    {
        fprintf(stderr, "[ERROR] read_entire_file: Failed to read file %s: %s\n", 
                filename, strerror(errno));
        free(content);
        close(fd);
        return NULL;
    }
    
    // Null terminate the content
    content[bytes_read] = '\0';
    close(fd);
    
    return content;
}

/*
New helper function to read from temp file and process each line.
Uses the main command processing logic without recursion.

Parameters:
- vars: Program state variables

Returns: void
*/
void read_and_process_from_tmp_buf(t_vars *vars)
{
    // Read the entire file content
    char *file_content = read_entire_file(TMP_BUF);
    if (!file_content)
        return;
    
    // Process the content character by character to find quote boundaries
    char *buffer = malloc(strlen(file_content) + 1);
    if (!buffer) {
        free(file_content);
        return;
    }
    
    int bufPos = 0;
    int in_quotes = 0;
    char quote_type = 0;
    int i = 0;
    
    while (file_content[i]) {
        // Track quote state
        if ((file_content[i] == '"' || file_content[i] == '\'') && 
            (!in_quotes || quote_type == file_content[i])) {
            if (in_quotes && quote_type == file_content[i]) {
                in_quotes = 0;
                quote_type = 0;
            } else if (!in_quotes) {
                in_quotes = 1;
                quote_type = file_content[i];
            }
        }
        
        // If we hit a newline
        if (file_content[i] == '\n') {
            if (in_quotes) {
                // Inside quotes - replace newline with space
                buffer[bufPos] = ' ';
                bufPos++;
            } else {
                // End of command - process and reset
                buffer[bufPos] = '\0';
                if (bufPos > 0) {
                    fprintf(stderr, "[DEBUG] read_and_process: Processing command: '%s'\n", buffer);
                    add_history(buffer);
                    process_single_command(buffer, vars);
                }
                bufPos = 0;
            }
        } else {
            // Normal character
            buffer[bufPos] = file_content[i];
            bufPos++;
        }
        i++;
    }
    
    // Process any remaining content
    if (bufPos > 0) {
        buffer[bufPos] = '\0';
        fprintf(stderr, "[DEBUG] read_and_process: Processing final command: '%s'\n", buffer);
        add_history(buffer);
        process_single_command(buffer, vars);
    }
    
    free(buffer);
    free(file_content);
}

/*
Processes multiline input with a unified approach for both
regular commands and heredocs.

Parameters:
- input: Multiline input string
- vars: Program state variables

Returns:
- 1 on success, 0 on failure
*/
int process_multiline_input(char *input, t_vars *vars)
{
    char **lines;
    int i = 0;
    int heredoc_found = 0;
    char *heredoc_delimiter = NULL;
    int fd;
    
    fprintf(stderr, "[DEBUG] process_multiline_input: Processing multiline input\n");
    
    // Split the input into lines
    lines = ft_split(input, '\n');
    if (!lines)
    {
        fprintf(stderr, "[ERROR] process_multiline_input: Failed to split input\n");
        return 0;
    }
    
    // Check first line for heredoc operator
    if (has_heredoc_operator(lines[0]))
    {
        fprintf(stderr, "[DEBUG] process_multiline_input: First line contains heredoc operator\n");
        
        // Extract the delimiter
        heredoc_delimiter = extract_heredoc_delimiter(lines[0], vars);
        if (heredoc_delimiter)
        {
            fprintf(stderr, "[DEBUG] process_multiline_input: Found heredoc delimiter: '%s'\n", 
                    heredoc_delimiter);
            heredoc_found = 1;
            
            // Store the delimiter for future reference
            if (vars->pipes->heredoc_delim)
                free(vars->pipes->heredoc_delim);
            vars->pipes->heredoc_delim = ft_strdup(heredoc_delimiter);
        }
    }
    
    // Open temp file for all multiline content
    fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1)
    {
        fprintf(stderr, "[ERROR] process_multiline_input: Failed to open temp file: %s\n", 
                strerror(errno));
        if (heredoc_delimiter)
            free(heredoc_delimiter);
        ft_free_2d(lines, ft_arrlen(lines));
        return 0;
    }
    
    // Write all lines to the temp file
    fprintf(stderr, "[DEBUG] process_multiline_input: Writing %zu lines to temp file\n", 
        ft_arrlen(lines));
    i = 0;
    int delimiter_found = 0;
    
    while (lines[i])
    {
        // For heredoc mode, check if this line is the delimiter
        if (heredoc_found && i > 0 && ft_strcmp(lines[i], heredoc_delimiter) == 0)
        {
            fprintf(stderr, "[DEBUG] process_multiline_input: Found delimiter line '%s'\n", 
                    lines[i]);
            delimiter_found = 1;
            // Don't write the delimiter line to the file
            i++;
            continue;
        }
        
        // For heredoc content, process variables if needed
        if (heredoc_found && i > 0 && !delimiter_found && 
            !is_quoted_delimiter(heredoc_delimiter))
        {
            // Process and write heredoc content with variable expansion
            char *processed_line = process_heredoc_line(lines[i], vars);
            if (processed_line)
            {
                fprintf(stderr, "[DEBUG] process_multiline_input: Writing processed line: '%s'\n", 
                        processed_line);
                write(fd, processed_line, ft_strlen(processed_line));
                write(fd, "\n", 1);
                free(processed_line);
            }
        }
        else
        {
            // Write regular line without processing
            fprintf(stderr, "[DEBUG] process_multiline_input: Writing line %d: '%s'\n", 
                    i, lines[i]);
            write(fd, lines[i], ft_strlen(lines[i]));
            write(fd, "\n", 1);
        }
        i++;
    }
    
    // Close the file
    close(fd);
    
    // Set appropriate flags based on what we found
    if (heredoc_found)
    {
        fprintf(stderr, "[DEBUG] process_multiline_input: Setting heredoc_mode = 1\n");
        vars->heredoc_mode = 1;  // Indicate we're using stored content
        
        if (delimiter_found)
        {
            fprintf(stderr, "[DEBUG] process_multiline_input: Delimiter was found in input\n");
            // Process the command with heredoc input ready
            process_command_with_heredoc(lines[0], vars);
        }
        else
        {
            fprintf(stderr, "[DEBUG] process_multiline_input: Delimiter not found, "
                    "will read interactively\n");
            // We need to continue reading heredoc input interactively
            vars->heredoc_active = 1;
            // Store first line for later execution
            if (vars->partial_input)
                free(vars->partial_input);
            vars->partial_input = ft_strdup(lines[0]);
        }
    }
    else
    {
        // Process regular multiline commands
        fprintf(stderr, "[DEBUG] process_multiline_input: Processing regular multiline commands\n");
        read_and_process_from_tmp_buf(vars);
    }
    
    // Clean up
    if (heredoc_delimiter)
        free(heredoc_delimiter);
    ft_free_2d(lines, ft_arrlen(lines));
    return 1;
}


// /*
// Main entry point for input processing with a unified approach to
// handle all input types consistently.
// */
// void handle_input(char *input, t_vars *vars)
// {
//     char *completed_input;
//     t_inmode mode;
    
//     if (!input || !*input)
//         return;
    
//     // Check if this is multiline input
//     if (ft_strchr(input, '\n'))
//     {
//         fprintf(stderr, "[DEBUG] Detected multiline input\n");
        
//         // Process the multiline input line by line
//         if (!process_multiline_input(input, vars))
//             return;
        
//         // Exit early as multiline input has been fully processed
//         return;
//     }
// 	// Continue with normal single-line processing
//     vars->partial_input = ft_strdup(input);
//     if (!vars->partial_input)
//         return ;
// 	// Reset heredoc mode at the start of new input
//     // if (!vars->heredoc_active)
//     //     vars->heredoc_mode = 0;
//     if (vars->heredoc_active)
// 	{
// 		fprintf(stderr, "[DBG_HEREDOC] Continuing heredoc input\n");
//         process_heredoc_continuation(vars->partial_input, vars);
//         clear_partial_input(vars);
//         return ;
//     }
//     mode = check_input_state(vars->partial_input, vars);
// 	fprintf(stderr, "[DBG_HEREDOC] Input mode: %d\n", mode);
//     completed_input = complete_input(vars->partial_input, mode, vars);
//     if (!completed_input)
// 	{
//         clear_partial_input(vars);
//         return ;
//     }
//     if (completed_input != vars->partial_input)
// 	{
//         free(vars->partial_input);
//         vars->partial_input = completed_input;
//     }
//     if (!process_input_tokens(vars->partial_input, vars))
// 	{
//         clear_partial_input(vars);
//         return ;
//     }
//     build_and_execute(vars);
//     clear_partial_input(vars);
// }
/*
Main input handler - serves as controller for all input processing.
Handles both single-line and multiline input.

Parameters:
- input: The input string to process
- vars: Program state variables

Returns: void
*/
void handle_input(char *input, t_vars *vars)
{
    char *completed_input;
    t_inmode mode;
    
    if (!input || !*input)
        return;
    
    fprintf(stderr, "[DEBUG] handle_input: Processing input (len=%zu)\n", ft_strlen(input));
    
    // Check if this is multiline input (contains newlines)
    if (ft_strchr(input, '\n'))
    {
        fprintf(stderr, "[DEBUG] handle_input: Detected multiline input\n");
        
        // Process the multiline input with unified approach
        if (!process_multiline_input(input, vars))
        {
            fprintf(stderr, "[DEBUG] handle_input: Failed to process multiline input\n");
            return;
        }
        
        // Exit early as multiline input has been fully processed
        return;
    }
    
    fprintf(stderr, "[DEBUG] handle_input: Processing single-line input\n");
    
    // Continue with normal single-line processing
    vars->partial_input = ft_strdup(input);
    if (!vars->partial_input)
        return;
    
    // Check if we're in heredoc continuation mode
    if (vars->heredoc_active)
    {
        fprintf(stderr, "[DEBUG] handle_input: Continuing heredoc input\n");
        process_heredoc_continuation(vars->partial_input, vars);
        clear_partial_input(vars);
        return;
    }
    
    // Check input state (quote completion, pipe completion, heredoc, normal)
    mode = check_input_state(vars->partial_input, vars);
    fprintf(stderr, "[DEBUG] handle_input: Input mode: %d\n", mode);
    
    // Complete input based on detected mode
    completed_input = complete_input(vars->partial_input, mode, vars);
    if (!completed_input)
    {
        clear_partial_input(vars);
        return;
    }
    
    if (completed_input != vars->partial_input)
    {
        free(vars->partial_input);
        vars->partial_input = completed_input;
    }
    
    // Process tokens and execute
    if (!process_input_tokens(vars->partial_input, vars))
    {
        clear_partial_input(vars);
        return;
    }
    
    build_and_execute(vars);
    clear_partial_input(vars);
}

/*
Determines the state of input and what processing is needed.
Returns the appropriate input mode for further processing.
*/
t_inmode	check_input_state(char *input, t_vars *vars)
{
    // Check for incomplete quotes first
    if (!validate_quotes(input, vars))
        return (INPUT_QUOTE_COMPLETION);
    // Check for unfinished pipes
    if (analyze_pipe_syntax(vars) == 2)  // 2 indicates pipe at end
        return (INPUT_PIPE_COMPLETION);
    // Check for heredoc operation (has << without content)
    if (has_unprocessed_heredoc(input))
        return (INPUT_HEREDOC_MODE);
    // Normal complete input
    return (INPUT_NORMAL);
}

void	term_heredoc(t_vars *vars)
{
	struct termios	heredoc_term;
	
	heredoc_term = vars->ori_term_settings;
    // Set raw mode for heredoc to capture Ctrl+D properly
    heredoc_term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &heredoc_term);
}

// /*
// Processes input as continuation data for an active heredoc.
// Takes the current input line and processes it as heredoc content.
// If the line matches the delimiter, completes the heredoc processing.

// Parameters:
// - input: The current input line to process
// - vars: Program state with heredoc information
// */
// void	process_heredoc_continuation(char *input, t_vars *vars)
// {
//     int fd[2];
//     int is_delimiter;
//     int expand_vars;
    
// 	fprintf(stderr, "[DBG_HEREDOC] Processing continuation: '%s'\n", input);
//     // Check if input matches the heredoc delimiter
//     is_delimiter = (ft_strcmp(input, vars->pipes->heredoc_delim) == 0);
// 	fprintf(stderr, "[DBG_HEREDOC] Is delimiter match: %s\n", 
// 		is_delimiter ? "yes" : "no");
//     // If this is the delimiter, complete heredoc
//     if (is_delimiter)
//     {
//         // Mark heredoc as no longer active
//         vars->heredoc_active = 0;
//         // Clean up the delimiter
//         free(vars->pipes->heredoc_delim);
//         vars->pipes->heredoc_delim = NULL;
//         // Inform user that heredoc is complete
//         fprintf(stderr, "heredoc> completed\n");
//         // Reset terminal state
//         manage_terminal_state(vars, TERM_RESTORE);
//         return ;
//     }
// 	fprintf(stderr, "[DBG_HEREDOC] Creating pipe for heredoc content\n");
//     // Not the delimiter, so add this line to the heredoc
//     if (pipe(fd) == -1)
// 	{
// 		fprintf(stderr, "[DBG_HEREDOC] Pipe creation failed: %s\n", strerror(errno));
//         return ;
// 	}
// 	// Determine if we should expand variables based on delimiter quoting
//     expand_vars = !is_quoted_delimiter(vars->pipes->heredoc_delim);
//     // Write the line to the heredoc pipe
//     write_to_heredoc(fd[1], input, vars, expand_vars);
// 	fprintf(stderr, "[DBG_HEREDOC] Line written to fd=%d\n", fd[1]);
//     // Update file descriptors (append mode)
//     close(fd[1]);
// 	fprintf(stderr, "[DBG_HEREDOC] Updated pipe fd=%d\n", vars->pipes->heredoc_fd);
//     // If we already have a heredoc_fd, close it
//     if (vars->pipes->heredoc_fd > 0)
//         close(vars->pipes->heredoc_fd);
//     // Update to the new fd
//     vars->pipes->heredoc_fd = fd[0];
//     // Prompt for more heredoc input
//     fprintf(stderr, "heredoc> ");
// }
/*
Handles a line of heredoc input in interactive mode.
Checks for delimiter and writes content to TMP_BUF.

Parameters:
- input: Line of input in heredoc mode
- vars: Program state variables

Returns: void
*/
void process_heredoc_continuation(char *input, t_vars *vars)
{
    fprintf(stderr, "[DEBUG] process_heredoc_cont: Processing line in heredoc mode\n");
    
    // Check if this line is the delimiter
    if (ft_strcmp(input, vars->pipes->heredoc_delim) == 0)
    {
        fprintf(stderr, "[DEBUG] process_heredoc_cont: Found delimiter '%s', ending heredoc\n", 
                vars->pipes->heredoc_delim);
        
        // Close the temp file
        if (vars->pipes->hd_fd_write != -1)
        {
            close(vars->pipes->hd_fd_write);
            vars->pipes->hd_fd_write = -1;
        }
        
        // Set mode and reset active flag
        vars->heredoc_mode = 1;  // Now using stored content
        vars->heredoc_active = 0;  // No longer collecting interactively
        
        // Process the original command
        if (vars->partial_input)
        {
            process_single_command(vars->partial_input, vars);
        }
        
        // Reset heredoc state
        vars->heredoc_mode = 0;
        if (vars->pipes->heredoc_delim)
        {
            free(vars->pipes->heredoc_delim);
            vars->pipes->heredoc_delim = NULL;
        }
    }
    else
    {
        // Write this line to temp file
        if (vars->pipes->hd_fd_write != -1)
        {
            // Process line based on expansion setting
            char *processed_line = NULL;
            if (vars->pipes->hd_expand)
                processed_line = process_heredoc_line(input, vars);
            else
                processed_line = ft_strdup(input);
            
            if (processed_line)
            {
                fprintf(stderr, "[DEBUG] process_heredoc_cont: Writing line: '%s'\n", 
                        processed_line);
                write(vars->pipes->hd_fd_write, processed_line, ft_strlen(processed_line));
                write(vars->pipes->hd_fd_write, "\n", 1);
                free(processed_line);
            }
        }
    }
}

/*
Manages terminal states throughout different operations
*/
void	manage_terminal_state(t_vars *vars, int action)
{
    if (action == TERM_SAVE)
	{
        // Save initial terminal settings
        if (!vars->ori_term_saved)
		{
            tcgetattr(STDIN_FILENO, &vars->ori_term_settings);
            vars->ori_term_saved = 1;
        }
	}  
	else if (action == TERM_HEREDOC)
	{
        // Configure terminal for heredoc input
		term_heredoc(vars);
	}
    else if (action == TERM_RESTORE)
	{
        // Restore original terminal settings
        if (vars->ori_term_saved)
		{
            tcsetattr(STDIN_FILENO, TCSANOW, &vars->ori_term_settings);
            // Make readline aware of the restored state
            rl_on_new_line();
        }
    }
	return ;
}

// /*
// Sets up heredoc mode and stores necessary data
// */
// void	setup_heredoc_mode(char *input, t_vars *vars)
// {
// 	int expand_vars;
	
// 	fprintf(stderr, "[DBG_HEREDOC] Setting up heredoc mode\n");
//     // Parse the heredoc delimiter from the input
//     vars->pipes->heredoc_delim = extract_heredoc_delimiter(input, vars);
//     if (!vars->pipes->heredoc_delim)
// 	{
//         return ;
// 	}
// 	fprintf(stderr, "[DBG_HEREDOC] Current heredoc_mode=%d\n", vars->heredoc_mode);
    
// 	// Only print debug info, no need to reset the mode if it's already 0
// 	fprintf(stderr, "[DBG_HEREDOC] Current heredoc_mode=%d (%s)\n", 
// 		vars->heredoc_mode, vars->heredoc_mode == 1 ? "multiline" : "interactive");
    
// 	fprintf(stderr, "[DBG_HEREDOC] Got delimiter: %s\n", 
//         vars->pipes->heredoc_delim ? vars->pipes->heredoc_delim : "NULL");
//     // Mark heredoc as active
//     vars->heredoc_active = 1;
//     // Determine if we should expand variables
//     expand_vars = !is_quoted_delimiter(vars->pipes->heredoc_delim);
// 	fprintf(stderr, "[DBG_HEREDOC] Variable expansion: %s\n", 
// 		expand_vars ? "enabled" : "disabled");
//     // Prepare for heredoc data collection
//     manage_terminal_state(vars, TERM_HEREDOC);
//     // If we have stored heredoc lines, use those
//     // if (vars->heredoc_lines)
// 	// {
//     //     process_stored_heredoc_lines(vars);
//     // }
// 	// else
// 	// {
//         // Otherwise set up for interactive heredoc input
//         setup_interactive_heredoc(vars, expand_vars);
//     // }
// 	fprintf(stderr, "[DBG_HEREDOC] Heredoc setup complete, active=%d\n", 
// 		vars->heredoc_active);
// }
/*
Modified to focus only on interactive heredoc setup.
Uses TMP_BUF for consistent storage.

Parameters:
- input: Command line containing a heredoc operator
- vars: Program state variables

Returns: void
*/
void setup_heredoc_mode(char *input, t_vars *vars)
{
    int expand_vars;
    
    fprintf(stderr, "[DEBUG] setup_heredoc_mode: Setting up heredoc\n");
    
    // Parse the heredoc delimiter from the input
    vars->pipes->heredoc_delim = extract_heredoc_delimiter(input, vars);
    if (!vars->pipes->heredoc_delim)
    {
        fprintf(stderr, "[ERROR] setup_heredoc_mode: Failed to extract delimiter\n");
        return;
    }
    
    fprintf(stderr, "[DEBUG] setup_heredoc_mode: Got delimiter: '%s', heredoc_mode=%d\n", 
            vars->pipes->heredoc_delim, vars->heredoc_mode);
    
    // Mark heredoc as active for interactive input
    vars->heredoc_active = 1;
    
    // Determine if we should expand variables based on delimiter quoting
    expand_vars = !is_quoted_delimiter(vars->pipes->heredoc_delim);
    fprintf(stderr, "[DEBUG] setup_heredoc_mode: Variable expansion: %s\n", 
            expand_vars ? "enabled" : "disabled");
    
    // Prepare for interactive heredoc input
    manage_terminal_state(vars, TERM_HEREDOC);
    
    // Set up for interactive collection if we're not already using stored content
    if (!vars->heredoc_mode)
    {
        setup_interactive_heredoc(vars, expand_vars);
    }
    
    fprintf(stderr, "[DEBUG] setup_heredoc_mode: Setup complete, active=%d, mode=%d\n", 
            vars->heredoc_active, vars->heredoc_mode);
}

/*
Writes a line to a heredoc pipe with optional variable expansion.
Adds a newline character after the content.

Parameters:
- fd: File descriptor to write to
- line: The line content to write
- vars: Program state for variable expansion
*/
// void	write_heredoc_line(int fd, char *line, t_vars *vars)
// {
//     char	*expanded_line;
    
//     // Expand variables if needed (based on delimiter quoting)
//     if (vars->pipes->heredoc_delim && !is_quoted_delimiter(vars->pipes->heredoc_delim))
//         expanded_line = expand_heredoc_line(line, vars);
//     else
//         expanded_line = ft_strdup(line);
        
//     if (!expanded_line)
//         return;
    
//     // Write the line with a newline
//     write(fd, expanded_line, ft_strlen(expanded_line));
//     write(fd, "\n", 1);
    
//     free(expanded_line);
// }

/*
Determines if a heredoc delimiter string appears to have been quoted.
Examines the delimiter for properly closed quote patterns.

Parameters:
- delimiter: The delimiter string to check

Returns:
- 1 (true) if the delimiter contains properly closed quotes
- 0 (false) otherwise
*/
int	is_quoted_delimiter(char *delimiter)
{
    int	i;
    int	in_single_quote;
    int	in_double_quote;
    
	fprintf(stderr, "[DBG_HEREDOC] Checking if delimiter '%s' is quoted\n", 
		delimiter ? delimiter : "NULL");
    if (!delimiter)
        return (0);
    i = 0;
    in_single_quote = 0;
    in_double_quote = 0;
    while (delimiter[i])
    {
        // Track single quotes
        if (delimiter[i] == '\'' && !in_double_quote)
            in_single_quote = !in_single_quote;
        // Track double quotes
        else if (delimiter[i] == '\"' && !in_single_quote)
            in_double_quote = !in_double_quote;
        i++;
    }
	
    // If we found quotes and all quotes are closed, consider it quoted
    // If quotes are unclosed, this is likely an error case - return 0
	int result = (((in_single_quote == 0) && (in_double_quote == 0)) && 
                 (ft_strchr(delimiter, '\'') || ft_strchr(delimiter, '\"')));
    fprintf(stderr, "[DBG_HEREDOC] Delimiter quoted status: %d\n", result);
    return (result);
    // if (((in_single_quote == 0) && (in_double_quote == 0)) && 
    //     (ft_strchr(delimiter, '\'') || ft_strchr(delimiter, '\"')))
    //     return (1);
    // return (0);
}

/*
Extracts the heredoc delimiter from a command string.
Looks for the first token after << operator that isn't a special shell token.

Parameters:
- input: The command string containing a heredoc operator

Returns:
- The extracted delimiter as a newly allocated string
- NULL if no valid delimiter is found
*/
char	*extract_heredoc_delimiter(char *input, t_vars *vars)
{
    int		i;
    int		start;
    int		in_quotes;
    char	quote_char;
    char	*result;
    
    if (!input)
	{
        return (NULL);
	}
	fprintf(stderr, "[DBG_HEREDOC] Extracting delimiter from input\n");
    i = 0;
    in_quotes = 0;
    quote_char = 0;
    // Find the << operator outside of quotes
    while (input[i])
    {
        // Handle quote tracking
        if ((input[i] == '\'' || input[i] == '\"') && 
            (!in_quotes || quote_char == input[i]))
        {
            if (in_quotes && quote_char == input[i])
                in_quotes = 0;
            else if (!in_quotes)
            {
                in_quotes = 1;
                quote_char = input[i];
            }
        }
        // Look for << outside of quotes
        else if (!in_quotes && input[i] == '<' && input[i + 1] == '<')
        {
			fprintf(stderr, "[DBG_HEREDOC] Found << operator at position %d\n", i-2);
            // Skip past << and any whitespace
            i += 2;
            while (input[i] && ft_isspace(input[i]))
                i++;
            // Check if we have a valid delimiter
            if (!input[i] || input[i] == '|' || input[i] == '<' || input[i] == '>')
                return (NULL);
            // Store the start of the delimiter
            start = i;
            // Find the end of the delimiter (until whitespace or operator)
            while (input[i] && !ft_isspace(input[i]) && 
                   input[i] != '|' && input[i] != '<' && input[i] != '>')
            {
                i++;
            }
            // Near the end of extract_heredoc_delimiter
			result = ft_substr(input, start, i - start);
			if (result)
			{
    			// Attempt to store heredoc content if this is a multiline input
    			int pos_after_delimiter = i;
    			while (input[pos_after_delimiter] && ft_isspace(input[pos_after_delimiter]))
				{
        			pos_after_delimiter++;
				}
				if (store_heredoc_content(input, pos_after_delimiter, result, vars))
				{
					fprintf(stderr, "[DBG_HEREDOC] Stored multiline content, setting heredoc_mode=1\n");
					vars->heredoc_mode = 1;
				}
				else
				{
					fprintf(stderr, "[DBG_HEREDOC] Failed to store multiline content\n");
				}
			}
			fprintf(stderr, "[DBG_HEREDOC] Extracted delimiter: '%s'\n", 
				result ? result : "NULL");
			// Add at the end of extract_heredoc_delimiter
			fprintf(stderr, "[DBG_HEREDOC] heredoc_mode after extraction: %d\n", vars->heredoc_mode);

			return (result);
        }
        i++;
    }
	// Add at the end of extract_heredoc_delimiter
	fprintf(stderr, "[DBG_HEREDOC] Extract heredoc delimiter at no heredoc operator found: %d\n", vars->heredoc_mode);

    // No heredoc operator found
    return (NULL);
}

/*
Checks if input contains an unprocessed heredoc operator (<<).
Returns 1 if an unprocessed heredoc is found, 0 otherwise.

Parameters:
- input: The command string to check
- vars: Program state (used for consistency with other checks)
*/
int	has_unprocessed_heredoc(char *input)
{
    int		i;
    int		in_quotes;
    char	quote_char;
    
	fprintf(stderr, "[DBG_HEREDOC] Checking for unprocessed heredoc\n");
    if (!input)
        return (0);
    i = 0;
    in_quotes = 0;
    quote_char = 0;
    while (input[i])
    {
        // Handle quote tracking - similar to validate_quotes()
        if ((input[i] == '\'' || input[i] == '\"') && 
            (!in_quotes || quote_char == input[i]))
        {
            if (in_quotes && quote_char == input[i])
                in_quotes = 0;
            else if (!in_quotes)
            {
                in_quotes = 1;
                quote_char = input[i];
            }
        }
        // Look for << outside of quotes
        else if (!in_quotes && input[i] == '<' && input[i + 1] == '<')
        {
            // Found a heredoc operator, now check for delimiter
            i += 2; // Skip past <<
            // Skip whitespace
            while (input[i] && ft_isspace(input[i]))
                i++;
            // If we're at the end of input or only have space/whitespace left
            if (!input[i] || input[i] == '|' || input[i] == '<' || input[i] == '>')
			{
				fprintf(stderr, "[DBG_HEREDOC] Unprocessed heredoc found, needs more input\n");
                return (1); // No delimiter found, heredoc needs processing
			}
            // We found what appears to be a delimiter, so heredoc is properly formed
            return (0);
        }
        i++;
    }
    // No unprocessed heredocs found
	fprintf(stderr, "[DBG_HEREDOC] No unprocessed heredoc\n");
    return (0);
}

// /*
// Process a heredoc from stored lines (multi-line input case)
// */
// void	process_stored_heredoc_lines(t_vars *vars)
// {
//     int fd[2];
//     char *line;
//     int i;
//     int expand_vars;
	
// 	fprintf(stderr, "[DBG_HEREDOC] Processing %d stored lines starting at index %d\n", 
// 		vars->heredoc_count, vars->heredoc_index);
//     if (pipe(fd) == -1)
// 	{
//         fprintf(stderr, "[DBG_HEREDOC] Pipe creation failed: %s\n", strerror(errno));
//         vars->heredoc_active = 0;
//     }

// 	// Determine if we should expand variables based on delimiter quoting
//     expand_vars = !is_quoted_delimiter(vars->pipes->heredoc_delim);
//     fprintf(stderr, "[DBG_HEREDOC] Variable expansion: %s\n", 
//         expand_vars ? "enabled" : "disabled");
//     // Process each stored line
//     i = vars->heredoc_index;
// 	while (i < vars->heredoc_count)
//     {
//         line = vars->heredoc_lines[i];
//         fprintf(stderr, "[DBG_HEREDOC] Checking line %d: '%s'\n", i, line);
        
//         // Check for delimiter
//         if (ft_strcmp(line, vars->pipes->heredoc_delim) == 0)
//         {
//             fprintf(stderr, "[DBG_HEREDOC] Found delimiter, stopping at index %d\n", i);
//             vars->heredoc_index = i + 1;
//             break;
//         }
        
//         // Write the line to the pipe
//         write_to_heredoc(fd[1], line, vars, expand_vars);
//         i++;
//     }
//     close(fd[1]); // Close write end
// 	fprintf(stderr, "[DBG_HEREDOC] Heredoc pipe fd=%d ready\n", 
// 		vars->pipes->heredoc_fd);
//     vars->pipes->heredoc_fd = fd[0]; // Store read end
//     // Reset terminal state
//     manage_terminal_state(vars, TERM_RESTORE);
// }

// /*
// Setup for interactive heredoc input
// */
// void	setup_interactive_heredoc(t_vars *vars, int expand_vars)
// {
//     int	fd[2];
// 	fprintf(stderr, "[DBG_HEREDOC] Setting up interactive heredoc mode, expand_vars=%d\n", 
// 		expand_vars);
//     if (pipe(fd) == -1)
// 	{
// 		fprintf(stderr, "[DBG_HEREDOC] Pipe creation failed: %s\n", strerror(errno));
//         vars->heredoc_active = 0;
//         return ;
//     }
//     // Read lines interactively until delimiter is reached
//     read_heredoc(fd, vars->pipes->heredoc_delim, vars, expand_vars);
// 	fprintf(stderr, "[DBG_HEREDOC] Interactive heredoc complete\n");
//     close(fd[1]); // Close write end
//     vars->pipes->heredoc_fd = fd[0]; // Store read end
//     // Reset terminal state
//     manage_terminal_state(vars, TERM_RESTORE);
// 	fprintf(stderr, "[DBG_HEREDOC] Heredoc pipe fd=%d ready\n", 
// 		vars->pipes->heredoc_fd);
// }
/*
Modified to use TMP_BUF consistently for heredoc content.
Sets up file for interactive heredoc input collection.

Parameters:
- vars: Program state variables
- expand_vars: Whether to expand variables in the heredoc content

Returns: void
*/
void setup_interactive_heredoc(t_vars *vars, int expand_vars)
{
    int fd;
    
    fprintf(stderr, "[DEBUG] setup_interactive_heredoc: Setting up interactive heredoc\n");
    
    // Create/truncate the temp file
    fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1)
    {
        fprintf(stderr, "[ERROR] setup_interactive_heredoc: Failed to open temp file: %s\n", 
                strerror(errno));
        return;
    }
    
    // Store file descriptor for writing
    vars->pipes->hd_fd_write = fd;
    
    // Store expansion flag for later use
    vars->pipes->hd_expand = expand_vars;
    
    fprintf(stderr, "[DEBUG] setup_interactive_heredoc: Temp file prepared, fd=%d, expand=%d\n", 
            fd, expand_vars);
}
