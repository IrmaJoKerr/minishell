/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/20 18:29:58 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void print_tokens(t_node *head)
{
	t_node *curr = head;
	int i;
	
	//fprintf(stderr, "\n--- TOKEN LIST ---\n");
	while (curr)
	{
		//fprintf(stderr, "Token: type=%d (%s), content='%s'\n", 
		//       curr->type, get_token_str(curr->type), 
		//       curr->args ? curr->args[0] : "NULL");
		
		if (curr->args && curr->args[1])
		{
			//fprintf(stderr, "  Arguments:");
			i = 1;
			while (curr->args[i])
			{
				//fprintf(stderr, " '%s'", curr->args[i]);
				i++;
			}
			//fprintf(stderr, "\n");
		}
		curr = curr->next;
	}
	//fprintf(stderr, "----------------\n\n");
}

/*
Reads input line from the user with prompt display.
- Displays the shell prompt and awaits user input.
- Handles Ctrl+D (EOF) by calling builtin_exit.
- Adds non-empty lines to command history.
Returns:
- User input as an allocated string.
- Never returns on EOF (exits program).
Works with main() in command loop.

Example: When user types "ls -la"
- Displays prompt "bleshell$> "
- Returns "ls -la" as an allocated string
- Adds command to history for up-arrow recall
*/
char	*reader(void)
{
	char	*line;

	line = readline(PROMPT);
	if (!line)
		return (NULL);
	if (*line)
		add_history(line);
	return (line);
}

/*
Processes input with quotes that need completion.
- Gets additional input for unclosed quotes.
- Re-tokenizes the completed command.
- Updates token list with completed input.
Returns:
Newly allocated complete command string.
NULL on memory allocation failure.
Works with process_command().

Example: When user types "echo "hello
- Prompts for completion of the double quote
- User types "world" and Enter
- Returns combined string: echo "hello world"
*/
char *handle_quote_completion(char *cmd, t_vars *vars)
{
	char *new_cmd;
	
	// Initialize quote context
	vars->quote_depth = 0;
	
	// Check if we have unclosed quotes
	if (validate_quotes(cmd, vars))
		return cmd;  // No unclosed quotes
	
	// Need to get more input for unclosed quotes
	new_cmd = fix_open_quotes(cmd, vars);
	if (!new_cmd)
		return (NULL);
		
	// If we got a new command string, free the old one
	if (new_cmd != cmd)
	{
		free(cmd);  // Remove the conditional check for error_msg
		cmd = new_cmd;
	}
	// Re-tokenize with the completed command
	cleanup_token_list(vars);
	improved_tokenize(cmd, vars);
	
	return (cmd);
}

/*
Finds the end of a command sequence in the token list
Returns:
 - The last node in the list if not empty
 - NULL if start_node is NULL
*/
t_node *find_command_end(t_node *start_node)
{
	t_node *current;
	
	if (!start_node)
		return (NULL);
	
	current = start_node;
	
	// Traverse to the end of the list
	while (current && current->next)
		current = current->next;
	
	// Return the last node in the list
	return (current);
}

/*
Builds and executes the command's abstract syntax tree.
- Creates AST from tokenized input.
- Executes the command if AST built successfully.
- Provides debug information about the process.
Returns:
Nothing (void function).
Works with process_command().

Example: For "echo hello | grep h"
- Builds AST with pipe node at root
- Echo command on left branch, grep on right
- Executes the pipeline with proper redirection
*/
void build_and_execute(t_vars *vars)
{
	
	fprintf(stderr, "[build_and_execute:%d] Building AST from token list\n", __LINE__);
    
	
    // Add validation here - check each redirection node for a target
    // t_node *current = vars->head;
    // while (current)
	// {
        // if (is_redirection(current->type) && !current->right)
		// {
        //     // Error: Redirection has no target
        //     fprintf(stderr, "DEBUG: Redirection without target detected at %p, type=%d\n", 
        //             (void*)current, current->type);
        //     shell_error("syntax error near unexpected token `newline'", ERR_SYNTAX, vars);
        //     vars->error_code = 2;
        //     return vars->error_code;
        // }
		// Validate token list before building AST
		if (!validate_redirection_targets(vars))
		{
			fprintf(stderr, "[build_and_execute:%d] Redirection validation failed, skipping execution\n", __LINE__);
			return ;
		}
        // current = current->next;
    // }
	
	// Use proc_token_list instead of process_token_list
	vars->astroot = proc_token_list(vars);
	if (vars->astroot)
	{
		DBG_PRINTF(DEBUG_EXEC, "AST built successfully, root type=%d\n", vars->astroot->type);
		// Add in build_and_execute before execution
		fprintf(stderr, "[DBG_HEREDOC] heredoc_mode before execution: %d\n", vars->heredoc_mode);
		execute_cmd(vars->astroot, vars->env, vars);
	}
	else
	{
		DBG_PRINTF(DEBUG_EXEC, "Failed to build AST, no valid root node\n");
	}
}

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
	if (!improved_tokenize(command, vars))
		return (0);
	return (1);
}

/*
Process the user command through lexing and execution.
- Handles input tokenization, syntax checking, and execution.
- Breaks processing into smaller logical stages.
- Manages memory throughout command processing lifecycle.
Works with handle_input() in command processing loop.

Example: When user types a complex command
- Processes tokens and handles unclosed quotes
- Validates and completes pipe syntax if needed
- Builds and executes command if valid
- Frees all temporary resources
*/
void process_command(char *command, t_vars *vars)
{
    fprintf(stderr, "[DEBUG] process_command() called\n");
    
    int pipe_result;
    
    // Reset error state at the beginning of each command
    vars->error_code = 0;
    
    // Store original command in vars->partial_input
    vars->partial_input = ft_strdup(command);
    if (!vars->partial_input)
        return;
        
    // Handle quote completion first
    vars->partial_input = handle_quote_completion(vars->partial_input, vars);
    if (!vars->partial_input)
        return;
        
    // Tokenize the input
    if (!process_input_tokens(vars->partial_input, vars))
    {
        free(vars->partial_input);
        vars->partial_input = NULL;
        return;
    }
    
    // Use pipe analysis system 
    pipe_result = analyze_pipe_syntax(vars);
    
    // Key part: handle pipe completion if needed
    if (pipe_result == 2) // Needs pipe completion
    {
        char *completed_cmd = complete_pipe_command(vars->partial_input, vars);
        if (!completed_cmd)
        {
            free(vars->partial_input);
            vars->partial_input = NULL;
            return;
        }
        free(vars->partial_input);
        vars->partial_input = completed_cmd;
        
        // Re-tokenize with completed command
        cleanup_token_list(vars);
        if (!process_input_tokens(vars->partial_input, vars))
        {
            free(vars->partial_input);
            vars->partial_input = NULL;
            return;
        }
    }
    else if (pipe_result == 1) // Syntax error
    {
        // Error handling code
        cleanup_token_list(vars);
        vars->error_code = 2;
        free(vars->partial_input);
        vars->partial_input = NULL;
        return;
    }
    
    // Build and execute the command
    build_and_execute(vars);
    // Add at the end of process_command()
	if (vars->partial_input)
	{
    	fprintf(stderr, "[MEM_DEBUG] Freeing partial_input=%p\n", 
            (void*)vars->partial_input);
    	free(vars->partial_input);
    	vars->partial_input = NULL;
	}
    // // Clean up
    // free(vars->partial_input);
    // vars->partial_input = NULL;
    fprintf(stderr, "[DEBUG] Reached the end of process_command()\n");
}

void reset_terminal_after_heredoc(void)
{
    struct termios term;
    char *tty_path;
    int fd;
    
    fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: Starting terminal reset\n");
    fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: STDIN_FILENO=%d isatty=%d\n", 
            STDIN_FILENO, isatty(STDIN_FILENO));
    fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: STDOUT_FILENO=%d isatty=%d\n", 
            STDOUT_FILENO, isatty(STDOUT_FILENO));
    
    if (!isatty(STDIN_FILENO)) {
        tty_path = ttyname(STDOUT_FILENO);
        fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: STDIN not a tty, using STDOUT tty_path=%s\n", 
                tty_path ? tty_path : "NULL");
        if (tty_path) {
            fd = open(tty_path, O_RDONLY);
            fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: Opened tty fd=%d\n", fd);
            if (fd >= 0) {
                dup2(fd, STDIN_FILENO);
                close(fd);
                fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: STDIN restored to tty\n");
            }
        }
    }
    
    // Add similar check for STDOUT
    if (!isatty(STDOUT_FILENO)) {
        tty_path = ttyname(STDERR_FILENO); // Use STDERR to get tty path
        fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: STDOUT not a tty, using STDERR tty_path=%s\n", 
                tty_path ? tty_path : "NULL");
        if (tty_path) {
            fd = open(tty_path, O_WRONLY);
            fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: Opened tty for STDOUT fd=%d\n", fd);
            if (fd >= 0) {
                dup2(fd, STDOUT_FILENO);
                close(fd);
                fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: STDOUT restored to tty\n");
            }
        }
    }
    
    if (isatty(STDIN_FILENO)) {
        int result = tcgetattr(STDIN_FILENO, &term);
        fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: tcgetattr result=%d\n", result);
        term.c_lflag |= (ICANON | ECHO);
        result = tcsetattr(STDIN_FILENO, TCSANOW, &term);
        fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: tcsetattr result=%d\n", result);
        rl_on_new_line();
        fprintf(stderr, "[DEBUG] reset_terminal_after_heredoc: Terminal reset complete\n");
    }
}

/*
Main shell loop that processes user commands and manages execution flow.
- Reads input through reader() function.
- Handles Ctrl+D and empty input cases.
- Processes commands through tokenizing and execution.
- Manages exit status tracking through pipeline.
Works as the start point of the shell.
*/
int	main(int argc, char **argv, char **envp)
{
	t_vars	vars;
	char	*input;
	char	*exit_args[2] = {NULL, NULL};

	(void)argc;
	(void)argv;
	ft_memset(&vars, 0, sizeof(t_vars));
	init_shell(&vars, envp);
	while (1)
	{
		input = reader();
		if (input == NULL)
		{
			fprintf(stderr, "[DEBUG] NULL input from reader(): '%s'\n", input);
			builtin_exit(exit_args, &vars);
		}
		if (input[0] == '\0')
		{
			free(input);
			continue ;
		}
		handle_input(input, &vars);
		free(input);
		reset_shell(&vars);
	}
	return (0);
}
