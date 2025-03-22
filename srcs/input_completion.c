/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 11:30:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check if a command is complete.
Returns 1 if complete, 0 if more input is needed.
Complete means no unclosed quotes and no dangling pipes.
*/
int	is_input_complete(t_vars *vars)
{
	t_node	*current;
	int		expecting_command;
	
	expecting_command = 0;
	current = vars->head;
	
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			expecting_command = 1;
		}
		else if (expecting_command && current->type != TYPE_STRING 
			&& current->type != TYPE_CMD)
		{
			expecting_command = 1;
		}
		else if (expecting_command)
		{
			expecting_command = 0;
		}
		current = current->next;
	}
	return (!expecting_command);
}

/*
Check if there's an unfinished pipe at the end of input.
Returns:
- 1 if there's an unfinished pipe needing completion
- 0 if there's no unfinished pipe
Works with handle_unfinished_pipes().
*/
int	check_unfinished_pipe(t_vars *vars, t_ast *ast)
{
	t_node	*last_token;
	t_node	*current;
	
	last_token = NULL;
	current = vars->head;
	// Find the last token
	while (current)
	{
		last_token = current;
		current = current->next;
	}
	// Check if the last token is a pipe
	if (last_token && last_token->type == TYPE_PIPE)
	{
		// If we have an AST struct, update its state
		if (ast)
			ast->pipe_at_end = 1;
		return 1;
	}
	// Also check the AST pipe_at_end flag if already set
	if (ast && ast->pipe_at_end)
		return 1;
	
	return 0;
}

/*
Check for unfinished pipes in input and handle them.
Prompts for additional input as needed.
Returns:
- 1 if pipes were handled and modifications were made
- 0 if no unfinished pipes found
- -1 if an error occurred
OLD VERSION
int	handle_unfinished_pipes(char **processed_cmd, t_vars *vars, t_ast *ast)
{
	char *additional;
	
	if (!check_unfinished_pipe(vars, ast))
		return (0);
	
	print_error("Pipe at end of input", NULL, 0);
	additional = readline("pipe> ");
	
	if (!additional)
	{
		// User pressed Ctrl+D during pipe input
		ft_safefree((void **)processed_cmd);
		return (-1);
	}
	*processed_cmd = append_new_input(*processed_cmd, additional);
	ft_safefree((void **)&additional);
	
	if (!*processed_cmd)
		return (-1);
	return (1);
}
*/
int handle_unfinished_pipes(char **processed_cmd, t_vars *vars, t_ast *ast)
{
    char *addon_input = NULL;
    char *tmp = NULL;
    
    fprintf(stderr, "DEBUG: Checking for unfinished pipe\n");
    if (!check_unfinished_pipe(vars, ast))
        return (0);
    fprintf(stderr, "DEBUG: Found pipe at end, prompting for more input\n");
    ft_putstr_fd("bleshell: Pipe at end of input\n", 2);
    addon_input = readline("PIPE> ");
    if (!addon_input)
	{
        fprintf(stderr, "DEBUG: EOF at pipe prompt, aborting\n");
        return -1;
    }
    tmp = ft_strtrim(addon_input, " \t\n");
    free(addon_input); // Free original before reassignment
    addon_input = tmp;
    if (!addon_input || addon_input[0] == '\0') {
        fprintf(stderr, "DEBUG: Empty input, exiting\n");
        free(addon_input); // Free if empty
        return handle_unfinished_pipes(processed_cmd, vars, ast); // Try again
    }
    fprintf(stderr, "DEBUG: Appending new input: '%s'\n", addon_input);
    tmp = ft_strjoin(*processed_cmd, " ");
    if (!tmp) {
        free(addon_input);
        return -1;
    }
    char *combined = ft_strjoin(tmp, addon_input);
    free(tmp);         // Free intermediate string
    free(addon_input); // Free additional input
    if (!combined)
        return (-1);
    free(*processed_cmd);
    *processed_cmd = combined;
    fprintf(stderr, "DEBUG: Successfully appended new input\n");
    fprintf(stderr, "DEBUG: New combined command: '%s'\n", *processed_cmd);
    cleanup_token_list(vars);
    tokenize(*processed_cmd, vars);
    lexerlist(*processed_cmd, vars);
    return (1);
}

/*
Gets additional input for unclosed quotes with appropriate prompt.
Returns:
- The input string.
- NULL on EOF/error.
OLD VERSION
char	*get_quote_input(t_vars *vars)
{
	char	*addon;
	char	*prompt;
	char	quote_char;
	
	// Determine quote char and type for proper prompt and debugging
	quote_char = vars->quote_ctx[vars->quote_depth - 1].type;
	// Output debug information directly
	fprintf(stderr, "DEBUG: Unclosed %s quote detected (depth: %d)\n",
		(quote_char == '\'' ? "single" : "double"), vars->quote_depth);
	print_error("Unclosed quotes detected", NULL, 0);
	// Set up proper bash-style prompt
	if (quote_char == '\'')
		prompt = "SQUOTE> ";
	else
		prompt = "DQUOTE> ";
	// Read additional input
	addon = readline(prompt);
	if (!addon)
	{
		fprintf(stderr, "DEBUG: Received EOF during quote completion\n");
		return (NULL);
	}
	fprintf(stderr, "DEBUG: addon input for quote by get_quo_iput: '%s'\n", addon);
	return (addon);
}
*/
char *get_quote_input(t_vars *vars)
{
    char *addon;
    char *prompt;
    
    fprintf(stderr, "DEBUG: [get_quote_input] ENTRY point\n");
    
    if (!vars) {
        fprintf(stderr, "DEBUG: [get_quote_input] vars is NULL\n");
        ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
        return NULL;
    }
    
    if (vars->quote_depth <= 0) {
        fprintf(stderr, "DEBUG: [get_quote_input] Invalid quote depth: %d\n", vars->quote_depth);
        ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
        return NULL;
    }
    
    // CRITICAL FIX: Add explicit bounds check before accessing quote_ctx array
    if (vars->quote_depth > 32 || vars->quote_depth < 1) {
        fprintf(stderr, "DEBUG: [get_quote_input] Quote depth %d out of bounds\n", vars->quote_depth);
        vars->quote_depth = 0; // Reset to prevent future issues
        ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
        return NULL;
    }
    
    // Set default prompt in case we fail to determine the quote type
    prompt = "QUOTE> ";
    
    // Safely determine quote char and type for proper prompt
    char quote_char = vars->quote_ctx[vars->quote_depth - 1].type;
    fprintf(stderr, "DEBUG: [get_quote_input] Quote char: '%c', depth: %d\n", 
            quote_char, vars->quote_depth);
    
    // Output debug information directly
    if (quote_char == '\'')
        fprintf(stderr, "DEBUG: [get_quote_input] Unclosed single quote detected\n");
    else if (quote_char == '"')
        fprintf(stderr, "DEBUG: [get_quote_input] Unclosed double quote detected\n");
    else
        fprintf(stderr, "DEBUG: [get_quote_input] Unclosed unknown quote type detected\n");
    
    // CRITICAL FIX: Use direct error message instead of print_error function
    ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
    
    // Set up proper bash-style prompt
    if (quote_char == '\'')
        prompt = "SQUOTE> ";
    else if (quote_char == '"')
        prompt = "DQUOTE> ";
    
    fprintf(stderr, "DEBUG: [get_quote_input] About to call readline with prompt: '%s'\n", prompt);
    
    // Read additional input
    addon = readline(prompt);
    
    fprintf(stderr, "DEBUG: [get_quote_input] readline returned: %p\n", (void*)addon);
    
    if (!addon) {
        fprintf(stderr, "DEBUG: [get_quote_input] Received EOF during quote completion\n");
        return NULL;
    }
    
    fprintf(stderr, "DEBUG: [get_quote_input] addon input: '%s'\n", addon);
    return addon;
}

/*
Process a single quote completion cycle.
Returns:
- 1 if succeeded and more processing needed
- 0 if succeeded and no more processing needed
- -1 if an error occurred
*/
int	chk_quotes_closed(char **processed_cmd, t_vars *vars)
{
	char	*addon;
	int		result;
	
	addon = get_quote_input(vars);
	fprintf(stderr, "DEBUG: get_quo_put fr chk_quo_clsd\n");
	if (!addon)
	{
		ft_safefree((void **)processed_cmd);
		return (-1);
	}
	// Append the addon input to the current command
	*processed_cmd = append_new_input(*processed_cmd, addon);
	ft_safefree((void **)&addon);
	
	if (!*processed_cmd)
	{
		fprintf(stderr, "DEBUG: Failed to append addon input\n");
		return (-1);
	}
	// Re-tokenize to check if quotes are now closed
	if (tokenize_to_test(*processed_cmd, vars) < 0)
	{
		fprintf(stderr, "DEBUG: Tokenization failed after adding quote input\n");
		return (-1);
	}
	// Determine if more processing is needed
	if (vars->quote_depth > 0)
		result = 1;
	else
		result = 0;
	return (result);
}

/*
Checks if quotes in a string are properly balanced.
- Handles both double and single quotes
- Properly tracks quote context
- Returns 1 if balanced, 0 if unbalanced
*/
int	quotes_are_closed(const char *str)
{
    int in_double_quote = 0;
    int in_single_quote = 0;
    int i = 0;
    
    in_double_quote = 0;
    in_single_quote = 0;
    i = 0;
    if (!str)
        return (1);
    while (str[i])
    {
        if (in_single_quote && str[i] == '\'')
            in_single_quote = 0;
        else if (in_double_quote && str[i] == '"')
            in_double_quote = 0;
        else if (!in_single_quote && !in_double_quote)
        {
            if (str[i] == '\'')
                in_single_quote = 1;
            else if (str[i] == '"')
                in_double_quote = 1;
        }
        i++;
    }
    return (!in_single_quote && !in_double_quote);
}

/*
Check for unclosed quotes in input and handle them.
Prompts for addon input as needed until all quotes are closed.
Returns:
- 1 if quotes were handled and modifications were made
- 0 if no unclosed quotes found
- -1 if an error occurred
OLD VERSION
int handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
    char *addon;
    char *new_cmd;
    int quote_handled;
    
    // Check if quotes are already balanced
    if (quotes_are_closed(*processed_cmd))
    {
        fprintf(stderr, "DEBUG: Quotes are balanced in command, no addon needed\n");
        vars->quote_depth = 0;
        return (0);
    }
    
    fprintf(stderr, "DEBUG: Unbalanced quotes detected, prompting for more input\n");
    
    // Initialize quote handling flag
    quote_handled = 0;
    
    // Continue handling quotes until all are closed
    while (vars->quote_depth > 0)
    {
        // Get additional input for the quote
        addon = get_quote_input(vars);
        fprintf(stderr, "DEBUG: get_quo_put fr hdle_uncl_quo\n");
        
        // Handle EOF or error
        if (!addon)
        {
            fprintf(stderr, "DEBUG: Failed to get quote input\n");
            ft_safefree((void **)processed_cmd);
            return (-1);
        }
        
        fprintf(stderr, "DEBUG: addon input for quote: '%s'\n", addon);
        
        // Create combined command
        new_cmd = append_input(*processed_cmd, addon);
        
        // Free addon to prevent double free
        ft_safefree((void **)&addon);
        
        // Handle append failure
        if (!new_cmd)
        {
            fprintf(stderr, "DEBUG: Failed to append addon input\n");
            return (-1);
        }
        
        // Update command with new combined version
        ft_safefree((void **)processed_cmd);
        *processed_cmd = new_cmd;
        
        // Re-tokenize to check if quotes are now closed
        if (tokenize_to_test(*processed_cmd, vars) < 0)
        {
            fprintf(stderr, "DEBUG: Tokenization failed after adding quote input\n");
            return (-1);
        }
        
        // Mark that we handled quotes
        quote_handled = 1;
        
        // Exit if no more quotes to process
        if (vars->quote_depth == 0)
        {
            break;
        }
    }
    
    // Return result based on whether quotes were handled
    if (quote_handled)
    {
        fprintf(stderr, "DEBUG: All quotes handled, input modified\n");
        return (1);
    }
    
    fprintf(stderr, "DEBUG: No unclosed quotes detected\n");
    return (0);
}
*/
int handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
    char *addon;
    char *new_cmd;
    int quote_handled;
    
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] ENTRY with processed_cmd=%p, vars=%p\n", 
            (void*)*processed_cmd, (void*)vars);
    
    if (!processed_cmd || !*processed_cmd) {
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] NULL processed_cmd\n");
        return -1;
    }
    
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Input: '%s'\n", *processed_cmd);
    
    /* Check if quotes are already balanced */
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Checking if quotes are closed\n");
    if (quotes_are_closed(*processed_cmd)) {
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Quotes are balanced\n");
        vars->quote_depth = 0;
        return 0;
    }
    
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Unbalanced quotes detected, depth=%d\n", 
            vars->quote_depth);
    
    /* CRITICAL FIX: Clean up any partially built token list before prompting */
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Cleaning up token list\n");
    cleanup_token_list(vars);
    
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Setting head and current to NULL\n");
    vars->head = NULL;
    vars->current = NULL;
    
    /* Display error message for unclosed quotes */
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Displaying error message\n");
    ft_putendl_fd("bleshell: Unclosed quotes detected", 2);
    
    /* Initialize quote handling flag */
    quote_handled = 0;
    
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Entering while loop, quote_depth=%d\n", 
            vars->quote_depth);
    
    /* Continue handling quotes until all are closed */
    while (vars->quote_depth > 0) {
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Top of while loop, quote_depth=%d\n", 
                vars->quote_depth);
        
        /* Get additional input for the quote */
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Calling get_quote_input()\n");
        addon = get_quote_input(vars);
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] get_quote_input() returned %p\n", 
                (void*)addon);
        
        /* Handle EOF or error */
        if (!addon) {
            fprintf(stderr, "DEBUG: [handle_unclosed_quotes] addon is NULL, freeing processed_cmd\n");
            ft_safefree((void **)processed_cmd);
            return -1;
        }
        
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Got addon: '%s'\n", addon);
        
        /* Create combined command */
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Calling append_input()\n");
        new_cmd = append_input(*processed_cmd, addon);
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] append_input() returned %p\n", 
                (void*)new_cmd);
        
        /* Free addon to prevent double free */
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Freeing addon\n");
        ft_safefree((void **)&addon);
        
        /* Handle append failure */
        if (!new_cmd) {
            fprintf(stderr, "DEBUG: [handle_unclosed_quotes] new_cmd is NULL\n");
            return -1;
        }
        
        /* Update command with new combined version */
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Freeing old processed_cmd\n");
        ft_safefree((void **)processed_cmd);
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Setting processed_cmd to new_cmd\n");
        *processed_cmd = new_cmd;
        
        /* CRITICAL FIX: Reset token list before re-tokenizing */
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Cleaning up token list again\n");
        cleanup_token_list(vars);
        vars->head = NULL;
        vars->current = NULL;
        
        /* Re-tokenize to check if quotes are now closed */
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Calling tokenize_to_test()\n");
        if (tokenize_to_test(*processed_cmd, vars) < 0) {
            fprintf(stderr, "DEBUG: [handle_unclosed_quotes] tokenize_to_test() failed\n");
            return -1;
        }
        fprintf(stderr, "DEBUG: [handle_unclosed_quotes] After tokenize_to_test(), quote_depth=%d\n", 
                vars->quote_depth);
        
        /* Mark that we handled quotes */
        quote_handled = 1;
        
        /* Exit if no more quotes to process */
        if (vars->quote_depth == 0) {
            fprintf(stderr, "DEBUG: [handle_unclosed_quotes] All quotes closed, breaking loop\n");
            break;
        }
    }
    
    /* Return result based on whether quotes were handled */
    fprintf(stderr, "DEBUG: [handle_unclosed_quotes] Returning %d\n", quote_handled ? 1 : 0);
    return quote_handled ? 1 : 0;
}

/*
Append new input to existing input with a newline.
Returns the combined string or NULL on error.
Simple string append function that joins two strings with a newline in between.
OLD VERSION
char *append_input(const char *first, const char *second)
{
    size_t	len1;
    size_t	len2;
    char	*result;
    
    // Handle NULL inputs
    if (!first)
        return (ft_strdup(second));
    if (!second)
        return (ft_strdup(first));
        
    // Calculate string lengths
    len1 = ft_strlen(first);
    len2 = ft_strlen(second);
    
    // Allocate space for both strings plus newline and null terminator
    result = malloc(len1 + len2 + 2);
    if (!result)
        return (NULL);
    ft_memcpy(result, first, len1);
    result[len1] = '\n';
    ft_memcpy(result + len1 + 1, second, len2);
    result[len1 + len2 + 1] = '\0';
    fprintf(stderr, "DEBUG: append_input created: '%s'\n", result);
    return (result);
}
*/
char *append_input(const char *first, const char *second)
{
    size_t len1;
    size_t len2;
    char *result;
    
    fprintf(stderr, "DEBUG: [append_input] ENTRY with first=%p, second=%p\n", 
            (void*)first, (void*)second);
    
    /* Handle NULL inputs */
    if (!first) {
        fprintf(stderr, "DEBUG: [append_input] first is NULL, duplicating second\n");
        return ft_strdup(second);
    }
    if (!second) {
        fprintf(stderr, "DEBUG: [append_input] second is NULL, duplicating first\n");
        return ft_strdup(first);
    }
    
    fprintf(stderr, "DEBUG: [append_input] first='%s', second='%s'\n", first, second);
    
    /* Calculate string lengths */
    len1 = ft_strlen(first);
    len2 = ft_strlen(second);
    
    fprintf(stderr, "DEBUG: [append_input] Allocating %zu + %zu + 2 bytes\n", len1, len2);
    
    /* Allocate space for both strings plus newline and null terminator */
    result = malloc(len1 + len2 + 2);
    if (!result) {
        fprintf(stderr, "DEBUG: [append_input] malloc failed\n");
        return NULL;
    }
    
    fprintf(stderr, "DEBUG: [append_input] Copying first string\n");
    ft_memcpy(result, first, len1);
    result[len1] = '\n';
    
    fprintf(stderr, "DEBUG: [append_input] Copying second string\n");
    ft_memcpy(result + len1 + 1, second, len2);
    result[len1 + len2 + 1] = '\0';
    
    fprintf(stderr, "DEBUG: [append_input] Result: '%s'\n", result);
    return result;
}
