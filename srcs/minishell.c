/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/05 18:35:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void print_tokens(t_node *head)
{
    t_node *curr = head;
    int i;
    
    fprintf(stderr, "\n--- TOKEN LIST ---\n");
    while (curr)
    {
        fprintf(stderr, "Token: type=%d (%s), content='%s'\n", 
               curr->type, get_token_str(curr->type), 
               curr->args ? curr->args[0] : "NULL");
        
        if (curr->args && curr->args[1])
        {
            fprintf(stderr, "  Arguments:");
            i = 1;
            while (curr->args[i])
            {
                fprintf(stderr, " '%s'", curr->args[i]);
                i++;
            }
            fprintf(stderr, "\n");
        }
        curr = curr->next;
    }
    fprintf(stderr, "----------------\n\n");
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
Sets up environment variables for the shell.
- Duplicates environment array from envp.
- Handles memory allocation errors.
- Initializes environment-dependent shell variables.
Works with init_shell().

Example: When shell starts
- Copies environment variables from system
- Sets up SHLVL variable for shell nesting level
- Reports errors if environment setup fails
*/
void	setup_env(t_vars *vars, char **envp)
{
	vars->env = dup_env(envp);
	if (!vars->env)
	{
		ft_putstr_fd("bleshell: error: Failed to duplicate environment\n", 2);
		exit(1);
	}
	vars->shell_level = get_shell_level(vars);
	if (vars->shell_level == -1)
	{
		ft_putstr_fd("bleshell: error: Failed to get shell level\n", 2);
		exit(1);
	}
	vars->shell_level = incr_shell_level(vars);
	if (!vars->shell_level)
		ft_putstr_fd("bleshell: warning: Failed to increment SHLVL\n", 2);
}

/*
Initializes the shell environment and variables.
- Sets up signal handlers.
- Initializes environment variables.
- Sets up shell history.
- Prepares the command prompt.
Works with main() as program entry point.
*/
/*
Initializes the shell environment and variables.
- Sets up signal handlers.
- Initializes environment variables.
- Sets up shell history.
- Prepares the command prompt.
Works with main() as program entry point.
*/
void	init_shell(t_vars *vars, char **envp)
{
	vars->env = dup_env(envp);
	if (!vars->env)
		exit(1);
	get_shell_level(vars);
	incr_shell_level(vars);
	load_signals();
	load_history();
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
	// For handle_quote_completion()
	DBG_PRINTF(DEBUG_TOKENIZE, "handle_quote_completion: about to tokenize '%s'\n", cmd);
    improved_tokenize(cmd, vars);
    
    return (cmd);
}

/*
Processes input with pipes that need continuation.
- Gets additional input for pipes at end of line.
- Re-tokenizes the completed command.
- Ensures proper error handling.
Returns:
Newly allocated complete command string.
NULL on memory allocation failure.
Works with process_command().

Example: When user types "ls |"
- Prompts for continuation after the pipe
- User types "grep foo" and Enter
- Returns combined string: "ls | grep foo"
*/
char	*handle_pipe_valid(char *cmd, t_vars *vars, int syntax_chk)
{
	char	*new_cmd;
	char	*pipe_cmd;

	if (syntax_chk != 2)
		return (cmd);
	pipe_cmd = ft_strdup(cmd);
	if (handle_unfinished_pipes(&pipe_cmd, vars, NULL) >= 0)
		new_cmd = pipe_cmd;
	else
		new_cmd = NULL;
	if (!new_cmd)
		return (NULL);
	if (new_cmd != cmd)
	{
    	free(cmd);  // Remove the conditional check for error_msg
    	cmd = new_cmd;
	}
	return (cmd);
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
// void	build_and_execute(t_vars *vars)
// {
// 	vars->astroot = build_ast(vars);
// 	if (vars->astroot)
// 		execute_cmd(vars->astroot, vars->env, vars);
// }
void	build_and_execute(t_vars *vars)
{
    t_node	*root;
    
	root = NULL;
    if (!vars || !vars->head)
        return ;

    DBG_PRINTF(DEBUG_EXEC, "Building AST from token list\n");
    find_cmd(NULL, NULL, FIND_ALL, vars);
	// Fix: Use vars->cmd_nodes[0] instead of undefined 'current'
	if (vars->cmd_count > 0 && vars->cmd_nodes[0] && vars->cmd_nodes[0]->args) {
        DBG_PRINTF(DEBUG_ARGS, "build_and_execute: Found command node: content='%s'\n", 
            vars->cmd_nodes[0]->args[0]);
    }
    process_token_list(vars);
    
    // Get the root node
    root = vars->astroot;
    if (!root && vars->cmd_count > 0)
    {
        root = vars->cmd_nodes[0];
        vars->astroot = root;
    }
    // Execute the command if AST built successfully
    if (vars->astroot)
    {
        DBG_PRINTF(DEBUG_EXEC, "Executing command tree\n");
        execute_cmd(vars->astroot, vars->env, vars);
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
int process_input_tokens(char *command, t_vars *vars)
{
    /* Skip processing if command is NULL or empty */
    if (!command || !*command)
        return (0);
    
    /* Initialize the token list */
    cleanup_token_list(vars);
    
    /* Tokenize the input with improved quote handling */
	// For process_input_tokens()
	DBG_PRINTF(DEBUG_TOKENIZE, "process_input_tokens: about to tokenize '%s'\n", command);
    if (!improved_tokenize(command, vars))
	{
        return (0);
	}
	process_quotes_and_expansions(vars);
    /* Process variable expansions in tokens */
    process_expansions(vars);
    
    return (1);
}

/*
Handles pipe syntax validation and completion.
- Checks for pipe syntax errors.
- Handles unfinished pipes by prompting for continuation.
Returns:
- Updated command string after pipe processing.
- NULL on memory allocation failure or other error.
- Command unchanged if no pipe issues detected.
Works with process_command() as second processing stage.

Example: For input "ls |"
- Detects unfinished pipe
- Prompts for continuation
- Returns completed command with pipe and continuation
*/
char	*process_pipe_syntax(char *command, t_vars *vars)
{
	int		syntax_chk;
	char	*processed_cmd;

	processed_cmd = command;
	syntax_chk = chk_pipe_syntax_err(vars);
	if (syntax_chk == 1)
	{
		if (processed_cmd != vars->partial_input && processed_cmd != command)
			free(processed_cmd);
		if (vars->partial_input && vars->partial_input != command)
			free(vars->partial_input);
		return (NULL);
	}
	processed_cmd = handle_pipe_completion(processed_cmd, vars, syntax_chk);
	return (processed_cmd);
}

/*
Process the user command through lexing and execution.
- Handles input tokenization, syntax checking, and execution.
- Breaks processing into smaller logical stages.
- Manages memory throughout command processing lifecycle.
Returns:
- 1 to continue shell loop.
- 0 to exit.
Works with main() in command processing loop.

Example: When user types a complex command
- Processes tokens and handles unclosed quotes
- Validates and completes pipe syntax if needed
- Builds and executes command if valid
- Frees all temporary resources
*/
// int process_command(char *command, t_vars *vars)
// {
//     char *processed_cmd;
    
//     // Make a copy to work with
//     processed_cmd = ft_strdup(command);
//     if (!processed_cmd)
//         return (1);
    
//     // Handle unclosed quotes first
//     processed_cmd = handle_quote_completion(processed_cmd, vars);
//     if (!processed_cmd)
//         return (1);
    
//     /* Tokenize and process input */
//     if (!process_input_tokens(processed_cmd, vars))
//     {
//         if (processed_cmd != command)
//             ft_safefree((void **)&processed_cmd);
//         return (1);
//     }
    
//     /* Check pipe syntax and handle completion */
//     if (!process_pipe_syntax(processed_cmd, vars))
//     {
//         if (processed_cmd != command)
//             ft_safefree((void **)&processed_cmd);
//         return (1);
//     }
    
//     /* Build and execute AST */
//     build_and_execute(vars);
    
//     // Free the processed command if it's different from the original
//     if (processed_cmd != command)
//         ft_safefree((void **)&processed_cmd);
        
//     return (1);
// }
int process_command(char *command, t_vars *vars)
{
    // Store original command in vars->partial_input
    vars->partial_input = ft_strdup(command);
    if (!vars->partial_input)
        return (1);
    vars->partial_input = handle_quote_completion(vars->partial_input, vars);
    if (!vars->partial_input)
        return (1);
    if (!process_input_tokens(vars->partial_input, vars))
    {
        free(vars->partial_input);
        vars->partial_input = NULL;
        return (1);
    }
    if (!process_pipe_syntax(vars->partial_input, vars))
    {
        free(vars->partial_input);
        vars->partial_input = NULL;
        return (1);
    }
    build_and_execute(vars);
    free(vars->partial_input);
    vars->partial_input = NULL;
    return (1);
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
            builtin_exit(exit_args, &vars);
        if (input[0] == '\0')
        {
            free(input);
            continue ;
        }
        process_command(input, &vars);
        free(input);
    }
    return (0);
}
