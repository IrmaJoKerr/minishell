/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/03/15 08:31:32 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Function to read input line from the user using readline.
Displays the prompt "bleshell$ " and waits for user input.
Handles Ctrl+D (EOF) by calling builtin_exit.
Adds non-empty lines to history with add_history. Recall with arrow keys.
Returns: The input line from the user, or exits on EOF.
*/
char	*reader(t_vars *vars)
{
	char	*line;

	line = readline("bleshell$ ");
	if (!line)
	{
		builtin_exit(vars);
	}
	if (*line)
		add_history(line);
	return (line);
}

/*
Initialize the shell environment and settings.
This function:
1) Zeroes out all fields in s_vars struct
2) Loads environment variables using envp into vars->env
3) Loads command history from HISTORY_FILE
4) Loads signal handlers (SIGINT, SIGQUIT)
Called once when shell starts up. Works with main().
*/
void	init_shell(t_vars *vars, char **envp)
{	
	ft_memset(vars, 0, sizeof(t_vars));
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
	vars->quote_depth = 0;
	load_history();
	load_signals();
}

/*
Process the user command by:
1) Cleaning up previous token list.
2) Lexing the command into tokens.
3) Building the AST from the tokens.
*/
int process_command(char *command, t_vars *vars)
{
    char *processed_cmd;
    int  syntax_check;
    char *new_cmd;
    
    processed_cmd = command;
    
    // Clean up previous token list
    if (vars->head)
    {
        fprintf(stderr, "DEBUG: Cleaning up previous token list\n");
        cleanup_token_list(vars);
    }
    
    if (!processed_cmd)
        return (0);
        
    fprintf(stderr, "DEBUG: Processing command: '%s'\n", processed_cmd);
    
    // First tokenization
    tokenize(processed_cmd, vars);
    lexerlist(processed_cmd, vars);
    
    // Check for unclosed quotes
    if (vars->quote_depth > 0)
    {
        new_cmd = handle_unclosed_quotes(processed_cmd, vars);
        if (!new_cmd)
            return (1);
        
        if (new_cmd != processed_cmd)
        {
            if (processed_cmd != command)
                free(processed_cmd);
            processed_cmd = new_cmd;
        }
        
        // Re-tokenize with completed command
        cleanup_token_list(vars);
        tokenize(processed_cmd, vars);
        lexerlist(processed_cmd, vars);
    }
    
    // Check for pipe syntax errors
    syntax_check = check_pipe_syntax_errors(vars);
    if (syntax_check == 2)  // Pipe at end, need more input
    {
        new_cmd = handle_unfinished_pipe(processed_cmd, vars);
        if (!new_cmd)
            return (1);
            
        if (new_cmd != processed_cmd)
        {
            if (processed_cmd != command)
                free(processed_cmd);
            processed_cmd = new_cmd;
        }
        // handle_unfinished_pipe already re-tokenizes
    }
    else if (syntax_check == 1)  // Other syntax error
    {
        if (processed_cmd != command)
            free(processed_cmd);
        free(command);
        return (1);
    }
    
    print_token_list(vars);
    
    // Build AST
    vars->astroot = build_ast(vars);
    if (vars->astroot)
    {
        fprintf(stderr, "DEBUG: Built AST successfully\n");
        if (vars->astroot->args && vars->astroot->args[0])
            fprintf(stderr, "DEBUG: Root command: %s\n", vars->astroot->args[0]);
        execute_cmd(vars->astroot, vars->env, vars);
    }
    else
    {
        fprintf(stderr, "DEBUG: Failed to build AST\n");
    }
    
    // Free the processed command if different from original
    if (processed_cmd != command)
        free(processed_cmd);
    free(command);
    return (1);
}

/*
Start point for the minishell program.
1) Initialize the shell environment
2) Enter infinite loop ( while(1) )to read and process commands
3) Read user input with reader()
4) Process each command with process_command()
5) Repeat until process_command() returns 0 (on EOF or error)
6) Save command history before exiting
Suppresses unused parameter warnings for ac and av (needed by main)
because of readline.
Returns 0 on normal exit.
*/
int	main(int ac, char **av, char **envp)
{
	t_vars	vars;
	char	*command;

	(void)ac;
	(void)av;
	init_shell(&vars, envp);
	while (1)
	{
		command = reader(&vars); // Pass vars to reader
		if (!process_command(command, &vars))
			break ;
	}
	// This code is only reached if process_command returns 0, not through builtin_exit
    // So we DON'T need to save history here since builtin_exit already does that
	if (vars.env)
	{
		ft_free_2d(vars.env, ft_arrlen(vars.env));
		vars.env = NULL;
	}
	cleanup_exit(&vars);
	return (0);
}
