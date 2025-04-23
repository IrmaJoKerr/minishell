/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 02:41:39 by bleow             #+#    #+#             */
/*   Updated: 2025/04/23 17:01:34 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// /*
// Completes input based on its determined mode.
// Handles quote completion, pipe completion, etc.
// */
// char	*complete_input(char *input, t_inmode mode, t_vars *vars)
// {
// 	char	*result;
	
//     if (mode == INPUT_QUOTE_COMPLETION)
// 		result = fix_open_quotes(input, vars);
//     else if (mode == INPUT_PIPE_COMPLETION)      
//         result = complete_pipe_cmd(input, vars);
//     // else if (mode == INPUT_HEREDOC_MODE)
// 	// {
//     //     setup_heredoc_mode(input, vars);
//     //     result = ft_strdup(input);
// 	// }
//     else if (mode == INPUT_NORMAL)
// 		result = ft_strdup(input);
// 	return (result);
// }

// void	clear_partial_input(t_vars *vars)
// {
// 	if (vars->partial_input)
// 	{
// 		free(vars->partial_input);
// 		vars->partial_input = NULL;
// 	}
// }

// /*
// Checks if an input string contains a heredoc operator '<<'
// outside of quotes.

// Parameters:
// - input: The string to check

// Returns:
// - 1 if a heredoc operator is found, 0 otherwise
// */
// int has_heredoc_operator(char *input)
// {
//     int i;
//     int in_quotes;
//     char quote_char;
    
//     if (!input)
//         return 0;
    
//     i = 0;
//     in_quotes = 0;
//     quote_char = 0;
    
//     while (input[i])
//     {
//         // Handle quote tracking
//         if ((input[i] == '\'' || input[i] == '\"') && 
//             (!in_quotes || quote_char == input[i]))
//         {
//             if (in_quotes && quote_char == input[i])
//                 in_quotes = 0;
//             else if (!in_quotes)
//             {
//                 in_quotes = 1;
//                 quote_char = input[i];
//             }
//         }
//         // Look for << outside of quotes
//         else if (!in_quotes && input[i] == '<' && input[i + 1] == '<')
//         {
//             return 1; // Found heredoc operator
//         }
//         i++;
//     }
//     return 0;
// }

// /*
// Processes a line of heredoc content, handling variable expansion
// if needed.

// Parameters:
// - line: The line of text to process
// - vars: Program state variables

// Returns:
// - Processed line with variables expanded (newly allocated)
// - Original line duplicated if no expansion needed
// Updated process_heredoc_line to use hd_expand directly
// */
// char *process_heredoc_line(char *line, t_vars *vars)
// {
//     // If we don't have a line, return NULL
//     if (!line)
//         return NULL;
        
//     // Handle variable expansion based on the flag
//     if (vars->pipes->hd_expand)
//     {
//         fprintf(stderr, "[DEBUG] process_heredoc_line: Expanding variables in: '%s'\n", line);
//         return hd_expander(line, vars);
//     }
    
//     // No expansion needed
//     fprintf(stderr, "[DEBUG] process_heredoc_line: No expansion for: '%s'\n", line);
//     return ft_strdup(line);
// }

// /*
// Helper to process a command that has a heredoc.
// Sets up heredoc mode and processes the command.

// Parameters:
// - cmd_line: The command line that contains a heredoc operator
// - vars: Program state variables

// Returns: void
// */
// void process_command_with_heredoc(char *cmd_line, t_vars *vars)
// {
//     fprintf(stderr, "[DEBUG] process_cmd_heredoc: Processing command with heredoc\n");
    
//     // Set up for heredoc usage
//     vars->heredoc_mode = 1;
    
//     // Process the command line normally
//     // process_single_command(cmd_line, vars);
// 	process_command(cmd_line, vars);
    
//     // Reset heredoc state after command completes
//     vars->heredoc_mode = 0;
//     vars->heredoc_active = 0;
//     if (vars->pipes->heredoc_delim)
//     {
//         free(vars->pipes->heredoc_delim);
//         vars->pipes->heredoc_delim = NULL;
//     }
    
//     fprintf(stderr, "[DEBUG] process_cmd_heredoc: Command processed, heredoc state reset\n");
// }

// /*
// Helper function to read an entire file into memory.
// Returns a newly allocated string containing the file contents,
// or NULL if an error occurred.
// */
// char *read_entire_file(const char *filename)
// {
//     int fd;
//     char *content = NULL;
//     struct stat file_stats;
//     ssize_t bytes_read;
    
//     fd = open(filename, O_RDONLY);
//     if (fd == -1)
//     {
//         fprintf(stderr, "[ERROR] read_entire_file: Failed to open file %s: %s\n", 
//                 filename, strerror(errno));
//         return NULL;
//     }
    
//     // Get file size
//     if (fstat(fd, &file_stats) == -1)
//     {
//         fprintf(stderr, "[ERROR] read_entire_file: Failed to stat file %s: %s\n", 
//                 filename, strerror(errno));
//         close(fd);
//         return NULL;
//     }
    
//     // Allocate memory for file content plus null terminator
//     content = malloc(file_stats.st_size + 1);
//     if (!content)
//     {
//         close(fd);
//         return NULL;
//     }
    
//     // Read entire file
//     bytes_read = read(fd, content, file_stats.st_size);
//     if (bytes_read == -1)
//     {
//         fprintf(stderr, "[ERROR] read_entire_file: Failed to read file %s: %s\n", 
//                 filename, strerror(errno));
//         free(content);
//         close(fd);
//         return NULL;
//     }
    
//     // Null terminate the content
//     content[bytes_read] = '\0';
//     close(fd);
    
//     return content;
// }
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

    fprintf(stderr, "[ML_DEBUG] read_entire_file: Opening %s\n", filename); // DEBUG
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
    fprintf(stderr, "[ML_DEBUG] read_entire_file: File size is %ld bytes\n", file_stats.st_size); // DEBUG

    // Allocate memory for file content plus null terminator
    content = malloc(file_stats.st_size + 1);
    if (!content)
    {
        fprintf(stderr, "[ERROR] read_entire_file: Malloc failed for %ld bytes\n", file_stats.st_size + 1); // DEBUG
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
     fprintf(stderr, "[ML_DEBUG] read_entire_file: Read %zd bytes\n", bytes_read); // DEBUG

    // Null terminate the content
    content[bytes_read] = '\0';
    close(fd);
    fprintf(stderr, "[ML_DEBUG] read_entire_file: Closed fd, returning content.\n"); // DEBUG

    return content;
}

// /*
// New helper function to read from temp file and process each line.
// Uses the main command processing logic without recursion.

// Parameters:
// - vars: Program state variables

// Returns: void
// */
// void read_and_process_from_tmp_buf(t_vars *vars)
// {
//     // Read the entire file content
//     char *file_content = read_entire_file(TMP_BUF);
//     if (!file_content)
//         return;
    
//     // Process the content character by character to find quote boundaries
//     char *buffer = malloc(strlen(file_content) + 1);
//     if (!buffer) {
//         free(file_content);
//         return;
//     }
    
//     int bufPos = 0;
//     int in_quotes = 0;
//     char quote_type = 0;
//     int i = 0;
    
//     while (file_content[i]) {
//         // Track quote state
//         if ((file_content[i] == '"' || file_content[i] == '\'') && 
//             (!in_quotes || quote_type == file_content[i])) {
//             if (in_quotes && quote_type == file_content[i]) {
//                 in_quotes = 0;
//                 quote_type = 0;
//             } else if (!in_quotes) {
//                 in_quotes = 1;
//                 quote_type = file_content[i];
//             }
//         }
        
//         // If we hit a newline
//         if (file_content[i] == '\n') {
//             if (in_quotes) {
//                 // Inside quotes - replace newline with space
//                 buffer[bufPos] = ' ';
//                 bufPos++;
//             } else {
//                 // End of command - process and reset
//                 buffer[bufPos] = '\0';
//                 if (bufPos > 0) {
//                     fprintf(stderr, "[DEBUG] read_and_process: Processing command: '%s'\n", buffer);
//                     add_history(buffer);
//                     // process_single_command(buffer, vars);
// 					process_command(buffer, vars);
//                 }
//                 bufPos = 0;
//             }
//         } else {
//             // Normal character
//             buffer[bufPos] = file_content[i];
//             bufPos++;
//         }
//         i++;
//     }
    
//     // Process any remaining content
//     if (bufPos > 0) {
//         buffer[bufPos] = '\0';
//         fprintf(stderr, "[DEBUG] read_and_process: Processing final command: '%s'\n", buffer);
//         add_history(buffer);
//         // process_single_command(buffer, vars);
// 		process_command(buffer, vars);
//     }
    
//     free(buffer);
//     free(file_content);
// }
/*
New helper function to read from temp file and process each line.
Uses the main command processing logic without recursion.
Parameters:
- vars: Program state variables
*/
void	read_and_process_from_tmp_buf(t_vars *vars)
{
    char *file_content;
    char *buffer;
    int bufPos = 0;
    int in_quotes = 0;
    char quote_type = 0;
    int i = 0;

    fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: START\n"); // DEBUG
    file_content = read_entire_file(TMP_BUF);
    if (!file_content)
	{
        fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Failed to read TMP_BUF\n"); // DEBUG
        return ;
    }
    fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Read %zu bytes from TMP_BUF\n", strlen(file_content)); // DEBUG

    buffer = malloc(strlen(file_content) + 1);
    if (!buffer) {
        free(file_content);
        fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Failed to malloc buffer\n"); // DEBUG
        return ;
    }

    while (file_content[i])
	{
        // Track quote state
        if ((file_content[i] == '"' || file_content[i] == '\'') &&
            (!in_quotes || quote_type == file_content[i])) {
            if (in_quotes && quote_type == file_content[i])
			{
                in_quotes = 0;
                quote_type = 0;
            }
			else if (!in_quotes)
			{
                in_quotes = 1;
                quote_type = file_content[i];
            }
        }

        // If we hit a newline
        if (file_content[i] == '\n')
		{
            if (in_quotes)
			{
                fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Newline inside quotes at pos %d, replacing with space.\n", i); // DEBUG
                buffer[bufPos] = ' ';
                bufPos++;
            }
			else
			{
                // End of command - process and reset
                buffer[bufPos] = '\0';
                if (bufPos > 0)
				{
                    fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Found command end. Processing: '%s'\n", buffer); // DEBUG
                    // add_history(buffer); // History added in handle_input for the whole block
                    process_command(buffer, vars);
                    fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Finished processing command.\n"); // DEBUG
                }
				else
				{
                    fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Found empty line, skipping.\n"); // DEBUG
                }
                bufPos = 0; // Reset buffer position
            }
        }
		else
		{
            // Normal character
            buffer[bufPos] = file_content[i];
            bufPos++;
        }
        i++;
    }

    // Process any remaining content (if file doesn't end with newline)
    if (bufPos > 0)
	{
        buffer[bufPos] = '\0';
        fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Processing final command: '%s'\n", buffer); // DEBUG
        // add_history(buffer); // History added in handle_input for the whole block
        process_command(buffer, vars);
        fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: Finished processing final command.\n"); // DEBUG
    }
    free(buffer);
    free(file_content);
    // Optionally remove the temp file after processing
    unlink(TMP_BUF);
    fprintf(stderr, "[ML_DEBUG] read_and_process_from_tmp_buf: END\n"); // DEBUG
}

// /*
// MODIFIED: Processes multiline input. If the first line contains a valid heredoc,
// it stores the content in TMP_BUF and sets mode=1. Otherwise, processes as regular commands.
// */
// int process_multiline_input(char *input, t_vars *vars)
// {
//     char	*first_line_end;
//     char	*content_start;
//     int		first_line_len;
//     int		tokenize_success;

//     fprintf(stderr, "[DEBUG] process_multiline_input: Processing multiline input\n");
//     // Find the end of the first line
//     first_line_end = ft_strchr(input, '\n');
//     if (!first_line_end) {
//         // Should not happen if called for multiline input, but handle defensively
//         fprintf(stderr, "[ERROR] process_multiline_input: No newline found in multiline input?\n");
//         process_command(input, vars); // Treat as single line
//         return (0); // Indicate failure or that it was handled as single line
//     }
//     first_line_len = first_line_end - input;
//     content_start = first_line_end + 1; // Start of the rest of the content

//     // Temporarily null-terminate the first line for tokenization
//     *first_line_end = '\0';
//     fprintf(stderr, "[DEBUG] process_multiline_input: Tokenizing first line: '%.*s'\n", first_line_len, input);

//     // Tokenize the first line - this will call is_valid_delim if << is present
//     reset_shell(vars); // Reset token list etc. before tokenizing
//     tokenize_success = improved_tokenize(input, vars);

//     // Restore the newline character
//     *first_line_end = '\n';

//     if (!tokenize_success || vars->error_code == ERR_SYNTAX) {
//         fprintf(stderr, "[DEBUG] process_multiline_input: Tokenization failed or syntax error on first line.\n");
//         // Error message should have been printed by tokenizer/validator
//         return (0); // Indicate failure
//     }

//     // Check if tokenization resulted in a valid heredoc delimiter being stored
//     if (vars->pipes->heredoc_delim != NULL)
//     {
//         fprintf(stderr, "[DEBUG] process_multiline_input: Heredoc delimiter '%s' validated.\n", vars->pipes->heredoc_delim);

//         // *** Perform Trailing Character Check ***
//         // Find the delimiter token in the list (it should be the right child of <<)
//         t_node *heredoc_node = vars->head; // Start searching from head
//         t_node *delimiter_node = NULL;
//         while(heredoc_node) {
//             if (heredoc_node->type == TYPE_HEREDOC) {
//                 delimiter_node = heredoc_node->right; // Assuming AST structure links it
//                 break;
//             }
//             heredoc_node = heredoc_node->next;
//         }

//         if (!delimiter_node || !delimiter_node->args || !delimiter_node->args[0]) {
//             fprintf(stderr, "[ERROR] process_multiline_input: Could not find delimiter token node after validation.\n");
//             return (0); // Internal error
//         }

//         // Calculate position after the raw delimiter token on the first line
//         // This requires token end position info, which might not be readily available.
//         // Alternative: Search for the raw delimiter string after <<.
//         char *raw_delim_in_line = strstr(input, delimiter_node->args[0]); // Find raw delim occurrence
//         if (!raw_delim_in_line) { /* Error */ return 0; }
//         int pos_after_raw_delim = (raw_delim_in_line - input) + strlen(delimiter_node->args[0]);


//         if (!check_trailing_chars(input, pos_after_raw_delim)) {
//             vars->error_code = ERR_SYNTAX;
//             // Delimiter might be stored, need cleanup? is_valid_delim handles freeing old one.
//             return (0); // Syntax error
//         }

//         // Trailing chars OK, proceed to store content
//         fprintf(stderr, "[DEBUG] process_multiline_input: Trailing chars OK. Storing content.\n");
//         if (!store_multiline_heredoc_content(content_start, vars)) {
//             fprintf(stderr, "[DEBUG] process_multiline_input: Failed to store heredoc content.\n");
//             // store_multiline_heredoc_content should set error code
//             return (0); // Failure during storage
//         }

//         // Set mode and process the first line command
//         vars->heredoc_mode = 1; // Indicate content is pre-stored
//         fprintf(stderr, "[DEBUG] process_multiline_input: Executing command from first line with stored heredoc.\n");
//         // Restore null-termination for process_command
//         *first_line_end = '\0';
//         process_command(input, vars); // Execute the command part
//         *first_line_end = '\n'; // Restore again (though likely not needed after this)
//         vars->heredoc_mode = 0; // Reset mode after execution
//     }
//     else
//     {
//         // No valid heredoc found on the first line, treat as regular multiline command
//         fprintf(stderr, "[DEBUG] process_multiline_input: No heredoc found. Processing as regular commands.\n");
//         // Need a way to process the whole input buffer line by line or command by command
//         // read_and_process_from_tmp_buf might be adapted, or process commands sequentially.
//         // For simplicity, let's just process the first line for now.
//         *first_line_end = '\0';
//         process_command(input, vars);
//         *first_line_end = '\n';
//         // TODO: Implement robust handling for multiple commands in multiline input without heredoc.
//     }

//     return (1); // Indicate success
// }
/*
MODIFIED: Processes multiline input. If the first line contains a valid heredoc,
it stores the content in TMP_BUF and sets mode=1. Otherwise, writes the full input
to TMP_BUF and processes commands from there.
*/
int process_multiline_input(char *input, t_vars *vars)
{
    char	*first_line_end;
    char	*content_start;
    int		first_line_len;
    int		tokenize_success;
    int		write_fd;
    int		delimiter_found_in_buffer;
    char	*current_pos;
    char	*next_newline;
    char	*line;
    size_t	delim_len;

    DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: START\n");
    first_line_end = ft_strchr(input, '\n');
    if (!first_line_end) {
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: No newline found (shouldn't happen here)\n");
        process_command(input, vars); // Fallback? Should not occur.
        return (0);
    }
    first_line_len = first_line_end - input;
    content_start = first_line_end + 1;

    *first_line_end = '\0'; // Temporarily split first line
    DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: Tokenizing first line: '%.*s'\n", first_line_len, input);

    reset_shell(vars); // Reset state before tokenizing
    tokenize_success = improved_tokenize(input, vars);

    *first_line_end = '\n'; // Restore newline

    if (!tokenize_success || vars->error_code == ERR_SYNTAX) {
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: Tokenization failed or syntax error on first line.\n");
        return (0);
    }

    if (vars->pipes->heredoc_delim != NULL)
    {
        // --- HEREDOC PATH ---
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: HEREDOC path entered. Delim='%s'\n", vars->pipes->heredoc_delim);

        // *** Perform Trailing Character Check (Keep your existing logic) ***
        t_node *heredoc_node = vars->head;
        t_node *delimiter_node = NULL;
        while(heredoc_node) {
            if (heredoc_node->type == TYPE_HEREDOC) {
                delimiter_node = heredoc_node->next;
                while (delimiter_node && delimiter_node->type != TYPE_ARGS && delimiter_node->next) { // Skip non-arg tokens like whitespace if any
                    delimiter_node = delimiter_node->next;
                }
                if (delimiter_node && delimiter_node->type == TYPE_ARGS) break;
                else delimiter_node = NULL;
            }
            heredoc_node = heredoc_node->next;
        }
        if (!delimiter_node || !delimiter_node->args || !delimiter_node->args[0]) {
            fprintf(stderr, "[ERROR] process_multiline_input: Could not find delimiter token node after validation.\n");
            return (0);
        }
        // Find raw delimiter in first line to check trailing chars
        char *heredoc_op_ptr = ft_strnstr(input, "<<", first_line_len);
        char *raw_delim_in_line = NULL;
        if (heredoc_op_ptr) {
            int search_start_offset = (heredoc_op_ptr - input) + 2;
            int remaining_len = first_line_len - search_start_offset;
            if (remaining_len > 0) {
                raw_delim_in_line = ft_strnstr(input + search_start_offset, delimiter_node->args[0], remaining_len);
            }
        }
        if (!raw_delim_in_line) {
            *first_line_end = '\0';
            fprintf(stderr, "[ERROR] process_multiline_input: Could not find raw delimiter '%s' in first line '%s'.\n", delimiter_node->args[0], input);
            *first_line_end = '\n';
            return 0;
        }
        int pos_after_raw_delim = (raw_delim_in_line - input) + ft_strlen(delimiter_node->args[0]);
        *first_line_end = '\0'; // Temporarily null-terminate for check_trailing_chars
        if (!check_trailing_chars(input, pos_after_raw_delim)) {
            *first_line_end = '\n'; // Restore
            vars->error_code = ERR_SYNTAX; // check_trailing_chars should set this
            return (0);
        }
        *first_line_end = '\n'; // Restore
        // *** End Trailing Character Check ***

        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: Trailing chars OK. Processing content buffer.\n");

        // --- NEW LOGIC: Write buffer content and potentially read interactively ---
        write_fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (write_fd == -1) {
            perror("bleshell: process_multiline_input: open(TMP_BUF)");
            vars->error_code = ERR_DEFAULT;
            return (0);
        }

        delimiter_found_in_buffer = 0;
        delim_len = ft_strlen(vars->pipes->heredoc_delim);
        current_pos = content_start;

        // Process the rest of the input buffer line by line
        while (current_pos && *current_pos)
        {
            next_newline = ft_strchr(current_pos, '\n');
            if (next_newline) {
                line = ft_substr(current_pos, 0, next_newline - current_pos);
                current_pos = next_newline + 1;
            } else {
                line = ft_strdup(current_pos); // Last part of buffer
                current_pos = NULL;
            }
            if (!line) { close(write_fd); vars->error_code = ERR_DEFAULT; return (0); }

            // Check for delimiter
            if (ft_strncmp(line, vars->pipes->heredoc_delim, delim_len) == 0 && line[delim_len] == '\0')
            {
                DBG_PRINTF(DEBUG_HEREDOC, "Delimiter found in buffer.\n");
                delimiter_found_in_buffer = 1;
                free(line);
                break; // Stop processing buffer
            }

            // Write line (with potential expansion)
            if (!write_to_heredoc(write_fd, line, vars)) {
                free(line);
                close(write_fd);
                unlink(TMP_BUF);
                return (0); // write_to_heredoc sets error_code
            }
            free(line);
            if (!next_newline) break; // End of buffer
        }

        // If delimiter was NOT found in the buffer, read interactively
        if (!delimiter_found_in_buffer)
        {
            DBG_PRINTF(DEBUG_HEREDOC, "Delimiter not in buffer, calling get_interactive_hd().\n");
            if (get_interactive_hd(write_fd, vars) == -1) {
                close(write_fd);
                unlink(TMP_BUF);
                // get_interactive_hd -> write_to_heredoc sets error_code
                return (0);
            }
        }

        // Close the write fd, content is fully gathered
        close(write_fd);
        vars->hd_text_ready = 1; // Mark content as ready in TMP_BUF
        DBG_PRINTF(DEBUG_HEREDOC, "Heredoc content gathering complete. TMP_BUF ready.\n");

        // Execute the command from the first line
        DBG_PRINTF(DEBUG_HEREDOC, "Executing command from first line: '%.*s'\n", first_line_len, input);
        *first_line_end = '\0'; // Null-terminate first line again
        process_command(input, vars); // Execute the command part
        *first_line_end = '\n'; // Restore (for safety, though likely not needed)

        // hd_text_ready will be reset by reset_shell on the next input cycle
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: HEREDOC path finished.\n");
    }
    else
    {
        // --- NON-HEREDOC MULTILINE PATH ---
        DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: NON-HEREDOC path entered.\n");
        // Write the entire multiline input to TMP_BUF and process commands from there
        int fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (fd == -1) {
            perror("bleshell: process_multiline_input: open(TMP_BUF)");
            vars->error_code = ERR_DEFAULT;
            return (0);
        }
        if (write(fd, input, ft_strlen(input)) == -1) {
            perror("bleshell: process_multiline_input: write(TMP_BUF)");
            close(fd);
            unlink(TMP_BUF);
            vars->error_code = ERR_DEFAULT;
            return (0);
        }
        close(fd);
        DBG_PRINTF(DEBUG_HEREDOC, "Finished writing full input to TMP_BUF.\n");
        read_and_process_from_tmp_buf(vars); // This function now handles processing from TMP_BUF
    }
    DBG_PRINTF(DEBUG_HEREDOC, "process_multiline_input: END\n");
    return (1); // Indicate success
}

void handle_input(char *input, t_vars *vars)
{
    DBG_PRINTF(DEBUG_HEREDOC, "handle_input: START, input len=%zu\n", ft_strlen(input));
    if (ft_strchr(input, '\n'))
    {
        DBG_PRINTF(DEBUG_HEREDOC, "handle_input: Detected multiline input, calling process_multiline_input.\n");
        if (!process_multiline_input(input, vars))
        {
            DBG_PRINTF(DEBUG_HEREDOC, "handle_input: process_multiline_input FAILED.\n");
            // Error should be set within process_multiline_input or its callees
        } else {
            DBG_PRINTF(DEBUG_HEREDOC, "handle_input: process_multiline_input SUCCEEDED.\n");
        }
    }
    else
    {
        DBG_PRINTF(DEBUG_HEREDOC, "handle_input: Detected single-line input, calling process_command.\n");
        reset_shell(vars); // Reset state for single command
        process_command(input, vars);
        DBG_PRINTF(DEBUG_HEREDOC, "handle_input: Returned from process_command.\n");
    }
    if (vars->pipes && vars->pipes->heredoc_delim) {
        free(vars->pipes->heredoc_delim);
        vars->pipes->heredoc_delim = NULL;
    }
    reset_redirect_fds(vars);
    DBG_PRINTF(DEBUG_HEREDOC, "handle_input: END\n");
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
*/
// void handle_input(char *input, t_vars *vars)
// {
//     if (!input || !*input)
//         return;
//     fprintf(stderr, "[DEBUG] handle_input: Processing input (len=%zu)\n", ft_strlen(input));
//     // Add to history before processing
//     if (input && *input)
//         add_history(input);
//     // Reset shell state for new command processing
//     reset_shell(vars);
//     if (ft_strchr(input, '\n'))
//     {
//         fprintf(stderr, "[DEBUG] handle_input: Detected multiline input\n");
//         if (!process_multiline_input(input, vars))
//         {
//             fprintf(stderr, "[DEBUG] handle_input: Failed to process multiline input\n");
//             // Error code should be set by process_multiline_input
//         }
//     }
//     else
//     {
//         fprintf(stderr, "[DEBUG] handle_input: Processing single-line input\n");
//         process_command(input, vars);
//     }
//     // Cleanup after command execution (like freeing AST, resetting pipes) happens in process_command/build_and_execute
//     fprintf(stderr, "[DEBUG] handle_input: Finished processing input.\n");
// }
// void handle_input(char *input, t_vars *vars)
// {
//     if (!input || !*input)
//         return;
//     fprintf(stderr, "[ML_DEBUG] handle_input: START, input len=%zu\n", ft_strlen(input)); // DEBUG
//     // Add to history before processing
//     if (input && *input) {
//         fprintf(stderr, "[ML_DEBUG] handle_input: Adding to history: '%.30s...'\n", input); // DEBUG
//         add_history(input);
//     }
//     // Reset shell state for new command processing (token list, AST, etc.)
//     // Note: reset_shell might clear too much if called repeatedly by read_and_process_from_tmp_buf
//     // Consider moving reset_shell inside process_command or just before tokenization.
//     // reset_shell(vars); // Moved reset_shell inside process_multiline_input and process_command
//     if (ft_strchr(input, '\n'))
//     {
//         fprintf(stderr, "[ML_DEBUG] handle_input: Detected multiline input, calling process_multiline_input.\n"); // DEBUG
//         if (!process_multiline_input(input, vars))
//         {
//             fprintf(stderr, "[ML_DEBUG] handle_input: process_multiline_input FAILED.\n"); // DEBUG
//             // Error code should be set by process_multiline_input or its sub-functions
//         } else {
//             fprintf(stderr, "[ML_DEBUG] handle_input: process_multiline_input SUCCEEDED.\n"); // DEBUG
//         }
//     }
//     else
//     {
//         fprintf(stderr, "[ML_DEBUG] handle_input: Detected single-line input, calling process_command.\n"); // DEBUG
//         process_command(input, vars); // process_command should handle its own reset_shell
//         fprintf(stderr, "[ML_DEBUG] handle_input: Returned from process_command.\n"); // DEBUG
//     }
//     // Cleanup after command execution (like freeing AST, resetting pipes) happens in process_command/build_and_execute
//     fprintf(stderr, "[ML_DEBUG] handle_input: END\n"); // DEBUG
// }

// /*
// Determines the state of input and what processing is needed.
// Returns the appropriate input mode for further processing.
// */
// t_inmode	check_input_state(char *input, t_vars *vars)
// {
//     // Check for incomplete quotes first
//     if (!validate_quotes(input, vars))
//         return (INPUT_QUOTE_COMPLETION);
//     // Check for unfinished pipes
//     if (analyze_pipe_syntax(vars) == 2)  // 2 indicates pipe at end
//         return (INPUT_PIPE_COMPLETION);
//     // Check for heredoc operation (has << without content)
//     if (has_unprocessed_heredoc(input))
//         return (INPUT_HEREDOC_MODE);
//     // Normal complete input
//     return (INPUT_NORMAL);
// }

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
// /*
// MODIFIED: Handles a line entered interactively when heredoc is active.
// This function might become less relevant if process_heredoc handles all interactive input.
// If kept, it should write to the fd opened by setup_interactive_heredoc.
// */
// void process_heredoc_continuation(char *input, t_vars *vars)
// {
//     // This function assumes interactive mode was set up previously,
//     // and vars->pipes->hd_fd_write points to the open TMP_BUF.
//     // It also assumes vars->pipes->heredoc_delim is set.
//     if (!vars || !vars->pipes || !vars->pipes->heredoc_delim || vars->heredoc_mode != 0) {
//         fprintf(stderr, "[ERROR] process_heredoc_continuation: Invalid state.\n");
//         return ;
//     }
//     fprintf(stderr, "[DEBUG] process_heredoc_cont: Processing line '%s'\n", input);
//     // Check if this line is the delimiter
//     if (ft_strcmp(input, vars->pipes->heredoc_delim) == 0)
//     {
//         fprintf(stderr, "[DEBUG] process_heredoc_cont: Found delimiter '%s', ending heredoc.\n",
//                 vars->pipes->heredoc_delim);
//         // Close the temp file write descriptor
//         if (vars->pipes->hd_fd_write != -1)
//         {
//             close(vars->pipes->hd_fd_write);
//             vars->pipes->hd_fd_write = -1;
//         }
//         // Restore terminal
//         manage_terminal_state(vars, TERM_RESTORE);
//         // Set mode to indicate content is ready in TMP_BUF
//         vars->heredoc_mode = 1;
//         // Process the original command stored in partial_input
//         if (vars->partial_input)
//         {
//             fprintf(stderr, "[DEBUG] process_heredoc_cont: Executing original command: '%s'\n", vars->partial_input);
//             // We need to tokenize and execute the original command now
//             process_command(vars->partial_input, vars);
//             free(vars->partial_input);
//             vars->partial_input = NULL;
//         }
// 		else
// 		{
//             fprintf(stderr, "[ERROR] process_heredoc_cont: Original command missing (partial_input is NULL).\n");
//         }
//         // Reset heredoc state after execution
//         vars->heredoc_mode = 0;
//         if (vars->pipes->heredoc_delim)
// 		{
//             free(vars->pipes->heredoc_delim);
//             vars->pipes->heredoc_delim = NULL;
//         }
//         vars->pipes->hd_expand = 0; // Reset expansion flag

//     }
//     else
//     {
//         // Write this line to the temp file using the write fd
//         if (vars->pipes->hd_fd_write != -1)
//         {
//             if (!write_to_heredoc(vars->pipes->hd_fd_write, input, vars)) {
//                 // Error writing, cleanup needed
//                 close(vars->pipes->hd_fd_write);
//                 vars->pipes->hd_fd_write = -1;
//                 unlink(TMP_BUF);
//                 manage_terminal_state(vars, TERM_RESTORE);
//                 // Reset state?
//                 if (vars->partial_input) free(vars->partial_input);
//                 vars->partial_input = NULL;
//                 if (vars->pipes->heredoc_delim) free(vars->pipes->heredoc_delim);
//                 vars->pipes->heredoc_delim = NULL;
//             }
// 			else
// 			{
//                 // Continue prompting implicitly by returning to main loop
//             }
//         } else {
//             fprintf(stderr, "[ERROR] process_heredoc_cont: Invalid write fd.\n");
//             // Error state, need cleanup
//         }
//     }
// }

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
// /*
// Modified to focus only on interactive heredoc setup.
// Uses TMP_BUF for consistent storage.

// Parameters:
// - input: Command line containing a heredoc operator
// - vars: Program state variables

// Returns: void
// */
// void setup_heredoc_mode(char *input, t_vars *vars)
// {
    
    
//     fprintf(stderr, "[DEBUG] setup_heredoc_mode: Setting up heredoc\n");
    
//     // Extract and process the delimiter using our unified function
//     get_heredoc_delimiter(input, vars);
//     if (!vars->pipes->heredoc_delim)
// 	{
// 		fprintf(stderr, "[ERROR] setup_heredoc_mode: Failed to extract delimiter\n");
// 		return;
// 	}
//     {
//         fprintf(stderr, "[ERROR] setup_heredoc_mode: Failed to extract delimiter\n");
//         return;
//     }
    
//     fprintf(stderr, "[DEBUG] setup_heredoc_mode: Got delimiter: '%s', expand_vars=%d\n",
//             vars->pipes->heredoc_delim, vars->pipes->hd_expand);
    
//     // Mark heredoc as active for interactive input
//     vars->heredoc_active = 1;
    
//     // Prepare for interactive heredoc input
//     manage_terminal_state(vars, TERM_HEREDOC);
    
//     // Set up for interactive collection if we're not already using stored content
//     if (!vars->heredoc_mode)
//     {
//         setup_interactive_heredoc(vars, vars->pipes->hd_expand);
//     }
    
//     // // Free local copy since the data is now stored in vars->pipes->heredoc_delim
//     // free(delimiter);
    
//     fprintf(stderr, "[DEBUG] setup_heredoc_mode: Setup complete, active=%d, mode=%d\n", 
//             vars->heredoc_active, vars->heredoc_mode);
// }

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
//         expanded_line = hd_expander(line, vars);
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
// int	is_quoted_delimiter(char *delimiter)
// {
//     int	i;
//     int	in_single_quote;
//     int	in_double_quote;
    
// 	fprintf(stderr, "[DBG_HEREDOC] Checking if delimiter '%s' is quoted\n", 
// 		delimiter ? delimiter : "NULL");
//     if (!delimiter)
//         return (0);
//     i = 0;
//     in_single_quote = 0;
//     in_double_quote = 0;
//     while (delimiter[i])
//     {
//         // Track single quotes
//         if (delimiter[i] == '\'' && !in_double_quote)
//             in_single_quote = !in_single_quote;
//         // Track double quotes
//         else if (delimiter[i] == '\"' && !in_single_quote)
//             in_double_quote = !in_double_quote;
//         i++;
//     }
	
//     // If we found quotes and all quotes are closed, consider it quoted
//     // If quotes are unclosed, this is likely an error case - return 0
// 	int result = (((in_single_quote == 0) && (in_double_quote == 0)) && 
//                  (ft_strchr(delimiter, '\'') || ft_strchr(delimiter, '\"')));
//     fprintf(stderr, "[DBG_HEREDOC] Delimiter quoted status: %d\n", result);
//     return (result);
//     // if (((in_single_quote == 0) && (in_double_quote == 0)) && 
//     //     (ft_strchr(delimiter, '\'') || ft_strchr(delimiter, '\"')))
//     //     return (1);
//     // return (0);
// }
// New implementation for node-based checking
// int is_quoted_delimiter(t_node *node)
// {
//     if (!node) 
//         return 0;
        
//     // First check if delimiter was originally quoted
//     if (node->delim_quoted)
//     {
//         fprintf(stderr, "[DEBUG] is_quoted_delimiter: Node has delim_quoted flag set\n");
//         return 1;
//     }
        
//     // Fallback to checking for quotes in the delimiter string
//     if (node->right && node->right->args && node->right->args[0])
//     {
//         int result = chk_expand_heredoc(node->right->args[0]) == 0;
//         fprintf(stderr, "[DEBUG] is_quoted_delimiter: Checking delimiter '%s', quoted=%d\n", 
//                 node->right->args[0], result);
//         return result;
//     }
    
//     return 0;
// }
// /*
// Determines if heredoc content should have variables expanded based on delimiter quoting.
// If the delimiter was quoted (with '' or ""), variables should not be expanded.

// Parameters:
// - node: Node containing the heredoc redirection information
// - vars: Program state variables for accessing hd_expand flag

// Returns:
// - 1 if the delimiter indicates variables should not be expanded (quoted)
// - 0 if variables should be expanded
// */
// int is_quoted_delimiter(t_node *node, t_vars *vars)
// {
//     // If we have the node and it's the last heredoc
//     if (node && vars && vars->pipes && vars->pipes->last_heredoc == node)
//     {
//         // Use the hd_expand flag (inverse, since we're checking if quoted)
//         return !vars->pipes->hd_expand;
//     }
    
//     // Fallback to checking the delimiter string directly if needed
//     if (node && node->right && node->right->args && node->right->args[0])
//     {
//         char *delimiter_copy = ft_strdup(node->right->args[0]);
//         int was_quoted = strip_outer_quotes(&delimiter_copy);
//         free(delimiter_copy);
//         return was_quoted;
//     }
    
//     return 0;
// }

// /*
// SIMPLIFIED: Extracts the raw heredoc delimiter word following <<.
// Does NOT perform validation or storage.
// Returns: Newly allocated raw delimiter string, or NULL.
// */
// char	*extract_heredoc_delimiter(char *input)
// {
//     int		i;
//     int		start;
//     int		in_quotes;
//     char	quote_char;

//     if (!input)
//         return (NULL);
//     fprintf(stderr, "[DBG_HEREDOC] Extracting raw delimiter from input\n");
//     i = 0;
//     in_quotes = 0;
//     quote_char = 0;
//     while (input[i])
//     {
//         if ((input[i] == '\'' || input[i] == '\"') &&
//             (!in_quotes || quote_char == input[i]))
//         {
//             if (in_quotes && quote_char == input[i])
//                 in_quotes = 0;
//             else if (!in_quotes)
//             {
//                 in_quotes = 1;
//                 quote_char = input[i];
//             }
//         }
//         else if (!in_quotes && input[i] == '<' && input[i + 1] == '<')
//         {
//             fprintf(stderr, "[DBG_HEREDOC] Found << operator\n");
//             i += 2;
//             while (input[i] && ft_isspace(input[i]))
//                 i++;
//             if (!input[i] || input[i] == '|' || input[i] == '<' || input[i] == '>')
//                 return (NULL); // Invalid syntax after <<
//             start = i;
//             while (input[i] && !ft_isspace(input[i]) &&
//                    input[i] != '|' && input[i] != '<' && input[i] != '>')
//                 i++;
//             char *result = ft_substr(input, start, i - start);
//             fprintf(stderr, "[DBG_HEREDOC] Extracted raw delimiter: '%s'\n", result ? result : "NULL");
//             return (result); // Return the raw delimiter
//         }
//         i++;
//     }
//     fprintf(stderr, "[DBG_HEREDOC] No << operator found\n");
//     return (NULL); // No heredoc operator found
// }

/*
NEW Helper: Checks for non-whitespace characters after a certain position on a line.
Returns 1 if only whitespace/end-of-string found, 0 otherwise.
*/
int check_trailing_chars(const char *line, int start_pos)
{
    int i = start_pos;
    while (line[i] && ft_isspace(line[i])) {
        i++;
    }
    // If we found something that's not whitespace and not the end, it's an error
    if (line[i] != '\0')
	{
        return (0); // Invalid trailing characters
    }
    return (1); // Only whitespace or end of string found
}

// /*
// Checks if input contains an unprocessed heredoc operator (<<).
// Returns 1 if an unprocessed heredoc is found, 0 otherwise.

// Parameters:
// - input: The command string to check
// - vars: Program state (used for consistency with other checks)
// */
// int	has_unprocessed_heredoc(char *input)
// {
//     int		i;
//     int		in_quotes;
//     char	quote_char;
    
// 	fprintf(stderr, "[DBG_HEREDOC] Checking for unprocessed heredoc\n");
//     if (!input)
//         return (0);
//     i = 0;
//     in_quotes = 0;
//     quote_char = 0;
//     while (input[i])
//     {
//         // Handle quote tracking - similar to validate_quotes()
//         if ((input[i] == '\'' || input[i] == '\"') && 
//             (!in_quotes || quote_char == input[i]))
//         {
//             if (in_quotes && quote_char == input[i])
//                 in_quotes = 0;
//             else if (!in_quotes)
//             {
//                 in_quotes = 1;
//                 quote_char = input[i];
//             }
//         }
//         // Look for << outside of quotes
//         else if (!in_quotes && input[i] == '<' && input[i + 1] == '<')
//         {
//             // Found a heredoc operator, now check for delimiter
//             i += 2; // Skip past <<
//             // Skip whitespace
//             while (input[i] && ft_isspace(input[i]))
//                 i++;
//             // If we're at the end of input or only have space/whitespace left
//             if (!input[i] || input[i] == '|' || input[i] == '<' || input[i] == '>')
// 			{
// 				fprintf(stderr, "[DBG_HEREDOC] Unprocessed heredoc found, needs more input\n");
//                 return (1); // No delimiter found, heredoc needs processing
// 			}
//             // We found what appears to be a delimiter, so heredoc is properly formed
//             return (0);
//         }
//         i++;
//     }
//     // No unprocessed heredocs found
// 	fprintf(stderr, "[DBG_HEREDOC] No unprocessed heredoc\n");
//     return (0);
// }

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
// /*
// Modified to use TMP_BUF consistently for heredoc content.
// Sets up file for interactive heredoc input collection.
// */
// void setup_interactive_heredoc(t_vars *vars, int expand_vars)
// {
//     int fd;
    
//     fprintf(stderr, "[DEBUG] setup_interactive_heredoc: Setting up interactive heredoc\n");
//     // Create/truncate the temp file
//     fd = open(TMP_BUF, O_WRONLY | O_CREAT | O_TRUNC, 0600);
//     if (fd == -1)
//     {
//         fprintf(stderr, "[ERROR] setup_interactive_heredoc: Failed to open temp file: %s\n", 
//                 strerror(errno));
//         return ;
//     }
//     // Store file descriptor for writing
//     vars->pipes->hd_fd_write = fd;
//     // Store expansion flag for later use
//     vars->pipes->hd_expand = expand_vars;
//     fprintf(stderr, "[DEBUG] setup_interactive_heredoc: Temp file prepared, fd=%d, expand=%d\n", 
//             fd, expand_vars);
// }
