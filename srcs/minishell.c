/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 19:35:16 by bleow            ###   ########.fr       */
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

// /*
// Sets up environment variables for the shell.
// - Duplicates environment array from envp.
// - Handles memory allocation errors.
// - Initializes environment-dependent shell variables.
// Works with init_shell().

// Example: When shell starts
// - Copies environment variables from system
// - Sets up SHLVL variable for shell nesting level
// - Reports errors if environment setup fails
// */
// void	setup_env(t_vars *vars, char **envp)
// {
// 	vars->env = dup_env(envp);
// 	if (!vars->env)
// 	{
// 		ft_putstr_fd("bleshell: error: Failed to duplicate environment\n", 2);
// 		exit(1);
// 	}
// 	vars->shell_level = get_shell_level(vars);
// 	if (vars->shell_level == -1)
// 	{
// 		ft_putstr_fd("bleshell: error: Failed to get shell level\n", 2);
// 		exit(1);
// 	}
// 	vars->shell_level = incr_shell_level(vars);
// 	if (!vars->shell_level)
// 		ft_putstr_fd("bleshell: warning: Failed to increment SHLVL\n", 2);
// }



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
    t_node *root;
    
    if (!vars || !vars->head)
        return;

    //DBG_PRINTF(DEBUG_EXEC, "Building AST from token list\n");
    
    // Use proc_token_list instead of process_token_list
    root = proc_token_list(vars);
    
    // Only update astroot if we got a valid root node
    if (root)
	{
        vars->astroot = root;
        //DBG_PRINTF(DEBUG_EXEC, "AST built successfully, root type=%d\n", root->type);
        // Execute the command tree
        execute_cmd(vars->astroot, vars->env, vars);
    }
	else
	{
        //DBG_PRINTF(DEBUG_EXEC, "Failed to build AST, no valid root node\n");
    }
}

// void	build_and_execute(t_vars *vars)
// {
//     t_node *cmd_start;
//     t_node *cmd_end;
//     t_vars temp_vars;
    
// 	cmd_start = vars->head;
// 	cmd_end = NULL;
//     while (cmd_start)
// 	{
//         // Find end of current command (until we hit a semicolon or NULL)
//         cmd_end = find_command_end(cmd_start);
//         // Create a temporary vars with just this command's tokens
//         ft_memcpy(&temp_vars, vars, sizeof(t_vars));
//         temp_vars.head = cmd_start;
//         temp_vars.current = cmd_start;
//         // If there's a next command, temporarily break the list
//         if (cmd_end && cmd_end->next)
// 		{
//             t_node *next_cmd = cmd_end->next;
//             cmd_end->next = NULL;
            
//             // Build and execute current command
//             root = proc_token_list(&temp_vars);
//             if (root) {
//                 temp_vars.astroot = root;
//                 execute_cmd(temp_vars.astroot, vars->env, vars);
//             }
            
//             // Reconnect for next iteration
//             cmd_end->next = next_cmd;
//             cmd_start = next_cmd;
//         } 
// 		else 
// 		{
//             // Process final command
//             root = proc_token_list(&temp_vars);
//             if (root)
// 			{
//                 temp_vars.astroot = root;
//                 execute_cmd(temp_vars.astroot, vars->env, vars);
//             }
//             break;
//         }
//     }
// }

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
    if (!improved_tokenize(command, vars))
	{
        return (0);
	}
	// process_quotes_and_expansions(vars);
    /* Process variable expansions in tokens */
    process_expansions(vars);
    
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
// void	process_command(char *command, t_vars *vars)
// {
//     // Store original command in vars->partial_input
//     vars->partial_input = ft_strdup(command);
//     if (!vars->partial_input)
//         return ;
//     vars->partial_input = handle_quote_completion(vars->partial_input, vars);
//     if (!vars->partial_input)
//         return ;
//     if (!process_input_tokens(vars->partial_input, vars))
//     {
//         free(vars->partial_input);
//         vars->partial_input = NULL;
//         return ;
//     }
//     if (!process_pipe_syntax(vars->partial_input, vars))
//     {
//         free(vars->partial_input);
//         vars->partial_input = NULL;
//         return ;
//     }
//     build_and_execute(vars);
//     free(vars->partial_input);
//     vars->partial_input = NULL;
// }
void process_command(char *command, t_vars *vars)
{
    // Store original command in vars->partial_input
    vars->partial_input = ft_strdup(command);
    if (!vars->partial_input)
        return ;
    // Handle quote completion first
    vars->partial_input = handle_quote_completion(vars->partial_input, vars);
    if (!vars->partial_input)
        return ;
    // Tokenize the input
    if (!process_input_tokens(vars->partial_input, vars))
    {
        free(vars->partial_input);
        vars->partial_input = NULL;
        return ;
    }
    // Use new pipe analysis system 
    int pipe_result = analyze_pipe_syntax(vars);
    if (pipe_result == 1) // Syntax error
    {
        // Error already reported by analyze_pipe_syntax
        free(vars->partial_input);
        vars->partial_input = NULL;
        return;
    }
    else if (pipe_result == 2) // Needs pipe completion
    {
        char *completed_cmd = complete_pipe_command(vars->partial_input, vars);
        if (!completed_cmd)
        {
            free(vars->partial_input);
            vars->partial_input = NULL;
            return ;
        }
        
        free(vars->partial_input);
        vars->partial_input = completed_cmd;
        // Re-tokenize with completed command
        cleanup_token_list(vars);
        if (!process_input_tokens(vars->partial_input, vars))
        {
            free(vars->partial_input);
            vars->partial_input = NULL;
            return ;
        }
    }
    // Build and execute the command
    build_and_execute(vars);
    // Clean up
    free(vars->partial_input);
    vars->partial_input = NULL;
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
        handle_input(input, &vars);
        free(input);
		reset_shell(&vars);
		// Cleanup any remaining resources
    }
    return (0);
}
