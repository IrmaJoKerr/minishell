/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 01:20:54 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Skips whitespace characters in the input string.
- Moves past spaces, tabs, and newlines.
- Updates vars->pos to point to the next non-whitespace.
Returns:
Nothing (void function).
Works with lexerlist() to handle token separation.

Example: Input "  echo hello"
- Skips the two spaces at the beginning
- Sets vars->pos to 2 (pointing at 'e')
*/
void	skip_whitespace(char *str, t_vars *vars)
{
    while (str[vars->pos] && (str[vars->pos] == ' '
            || str[vars->pos] == '\t' || str[vars->pos] == '\n'))
        vars->pos++;
}

/*
Prompts for additional input when input is incomplete.
- Used when quotes are unclosed or pipes need more input.
- Provides visual indicator of input continuation.
Returns:
Allocated string with user input or NULL if readline fails.
Works with handle_unclosed_quotes() and handle_unfinished_pipes().

Example: read_added_input("quote> ")
- Shows "quote> " prompt to user
- Returns user input as newly allocated string
*/
char	*read_added_input(char *prompt)
{
    char	*line;
    
    line = readline(prompt);
    if (!line)
        return (NULL);
    return (line);
}

/*
Processes a text segment before reaching a special character.
- Creates a command token for the text segment.
- Updates token list with the new command node.
Returns:
Nothing (void function).
Works with handle_operator_token().

Example: For input "echo hello | grep"
- When reaching pipe, processes "echo hello" as command
- Creates command node and adds to token list
*/
void	handle_text_chunk(char *str, t_vars *vars)
{
    char	*cmd_str;

    if (vars->pos <= vars->start)
        return ;
    cmd_str = ft_substr(str, vars->start, vars->pos - vars->start);
    if (!cmd_str)
        return ;
    vars->curr_type = TYPE_CMD;
    fprintf(stderr, "DEBUG: Processing text chunk: '%s'\n", cmd_str);
    process_cmd_token(cmd_str, vars);
    ft_safefree((void **)&cmd_str);
}

/*
Processes text and adds to token list with optional type override.
- Creates token from current text segment.
- Applies standard or override token type.
- Updates token list and position markers.
Returns:
Nothing (void function).
Works with handle_token_boundary() and handle_token().

Example: With first_token=1 and TYPE_CMD type
- Creates command token for current text segment
- Sets first_token to 0 after processing
*/
void process_text(char *str, t_vars *vars, int *first_token, t_tokentype override_type)
{
    char *token;
    int len;
    
    len = vars->pos - vars->start;
    token = ft_substr(str, vars->start, len);
    if (!token)
        return;
        
    fprintf(stderr, "DEBUG: process_text: len=%d, text='%s', first_token=%d\n", 
            len, token, *first_token);
            
    // Process as command if it's the first token after a pipe
    if (override_type != TYPE_NULL)
    {
        vars->curr_type = override_type;
        fprintf(stderr, "DEBUG: Using override type %d\n", override_type);
    }
    else if (*first_token || vars->prev_type == TYPE_PIPE)
    {
        vars->curr_type = TYPE_CMD;
        *first_token = 0;  // Reset first token flag
        fprintf(stderr, "DEBUG: Classifying as command (after pipe)\n");
    }
    else
    {
        vars->curr_type = TYPE_STRING;
        fprintf(stderr, "DEBUG: Classifying as STRING\n");
    }
    
    maketoken(token, vars);
    vars->start = vars->pos;
    vars->prev_type = vars->curr_type;  // Track previous token type
    free(token);
}

/*
Processes quoted content in the input string.
- Handles content inside single or double quotes.
- Updates quote depth and context tracking.
Returns:
Nothing (void function).
Works with handle_token().

Example: For input with "'Hello world'"
- Tracks single quote context
- Processes "Hello world" as quoted content
*/
void	handle_quote_content(char *str, t_vars *vars, int *first_token)
{
    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
    
    handle_quotes(str, &vars->pos, vars);
}

/*
Handles incomplete quoted input by reading additional lines.
- Provides continuation prompts with quote> prefix.
- Appends new input to existing command string.
- Re-tokenizes the combined input to check quote closure.
Returns:
- Modified string with complete quoted content
- NULL on memory allocation failure or input error
Works with lexerlist() for handling unclosed quotes.

Example: For input with unclosed quote "echo "hello
- Prompts user with "quote> "
- User enters "world""
- Function returns "echo "hello\nworld""
- Quote depth is reset after processing
*/
char *lexing_unclosed_quo(char *input, t_vars *vars)
{
    char *processed_cmd;
    char *addon;
    char *temp;
    int attempts;
    
    /* Initialize variables */
    attempts = 0;
    processed_cmd = input;
    
    /* Process quotes until they're all closed or max attempts reached */
    while (vars->quote_depth > 0 && attempts < 10)
    {
        /* Count this attempt */
        attempts++;
        
        /* Get additional input */
        addon = get_quote_input(vars);
        fprintf(stderr, "DEBUG: get_quo_put fr lex_uncl_quo\n");
        
        /* Check for EOF or error */
        if (!addon)
            return (NULL);
            
        fprintf(stderr, "DEBUG: get_quo_put fr lex_uncl_quo.before calling appnd\n");
        
        /* Join strings properly without losing characters */
        temp = append_input(processed_cmd, addon);
        
        /* Free addon right after using it */
        ft_safefree((void **)&addon);
        
        /* Check if append was successful */
        if (!temp)
            return (NULL);
            
        /* Update processed_cmd and free old one if needed */
        if (processed_cmd != input)
            ft_safefree((void **)&processed_cmd);
        processed_cmd = temp;
        
        fprintf(stderr, "DEBUG: get_quo_put fr lex_uncl_quo.after calling appnd\n");
        
        /* Reset parser state completely before retokenizing */
        cleanup_token_list(vars);
        vars->quote_depth = 0;  /* Reset quote depth before reprocessing */
        vars->head = NULL;
        vars->current = NULL;
        
        /* Debug output for the string being tokenized */
        fprintf(stderr, "DEBUG: Tokenizing combined string: '%s'\n", processed_cmd);
        
        /* CRITICAL FIX: Use lexerlist instead of tokenize - this ensures proper command structure */
        lexerlist(processed_cmd, vars);
        
        fprintf(stderr, "DEBUG: After tokenizing with added quote, depth=%d\n", 
                vars->quote_depth);
                
        /* If quotes are now balanced, exit the loop */
        if (vars->quote_depth == 0)
            break;
    }
    
    /* Handle too many unclosed quotes */
    if (vars->quote_depth > 0 && attempts >= 10)
    {
        fprintf(stderr, "DEBUG: Failed to close quotes after multiple attempts\n");
        print_error("Too many unclosed quotes, aborting input", NULL, 0);
    }
    
    /* Return processed command string */
    return (processed_cmd);
}

/*
Processes variable expansion tokens.
- Handles $VAR syntax for variable expansion.
- Creates appropriate expansion nodes.
Returns:
Nothing (void function).
Works with handle_token().

Example: For $HOME in input
- Creates expansion token for HOME variable
- Updates position to end of variable name
OLD VERSION
void	handle_expansion_token(char *str, t_vars *vars, int *first_token)
{
    char	*expanded;
    
    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
    expanded = handle_expansion(str, &vars->pos, vars);
    if (expanded)
        ft_safefree((void **)&expanded);
    
    vars->start = vars->pos;
}
*/
/*
Processes variable expansion tokens.
- Handles $VAR syntax for variable expansion.
- Creates appropriate expansion nodes.
- Special handling for $? to show exit status
Returns:
Nothing (void function).
Works with handle_token().
*/
void handle_expansion_token(char *str, t_vars *vars, int *first_token)
{
    char *expanded;
    t_node *cmd_node;
    
    // Process any text before the $ if needed
    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
    
    // Special handling for $?
    if (str[vars->pos] == '$' && str[vars->pos + 1] == '?')
    {
        // Find the most recent command node if we're not the first token
        if (!(*first_token) && vars->head)
        {
            // Try to find the last command node to add $? as an argument
            cmd_node = find_last_command(vars->head);
            
            if (cmd_node && cmd_node->type == TYPE_CMD)
            {
                // Append $? as an argument to the command
                fprintf(stderr, "DEBUG: Adding $? as argument to command '%s'\n", 
                       cmd_node->args[0]);
                append_arg(cmd_node, "$?");
            }
            else
            {
                // If we can't find a command to attach to, create a token with proper type
                vars->curr_type = TYPE_EXIT_STATUS;  // Use proper type
                maketoken("$?", vars);
                fprintf(stderr, "DEBUG: Created standalone $? token with TYPE_EXIT_STATUS\n");
            }
        }
        else
        {
            // First token - create as command if it's the first thing in the input
            vars->curr_type = *first_token ? TYPE_CMD : TYPE_EXIT_STATUS;
            maketoken("$?", vars);
            *first_token = 0;  // No longer the first token
            fprintf(stderr, "DEBUG: Created $? token as %s\n", 
                  *first_token ? "command" : "exit status");
        }
        
        vars->pos += 2; // Skip past $?
    }
    else
    {
        // Regular variable expansion
        expanded = handle_expansion(str, &vars->pos, vars);
        
        if (expanded)
        {
            // Similar logic as for $? - if we're not first token and have a command,
            // add as an argument to that command
            if (!(*first_token) && vars->head)
            {
                cmd_node = find_last_command(vars->head);
                if (cmd_node && cmd_node->type == TYPE_CMD)
                {
                    fprintf(stderr, "DEBUG: Adding expanded value '%s' as argument to command '%s'\n", 
                           expanded, cmd_node->args[0]);
                    append_arg(cmd_node, expanded);
                }
                else
                {
                    // Otherwise create a standalone token
                    vars->curr_type = TYPE_STRING;
                    maketoken(expanded, vars);
                    fprintf(stderr, "DEBUG: Created standalone token from expanded value: '%s'\n", expanded);
                }
            }
            else
            {
                // First token - create as command
                vars->curr_type = *first_token ? TYPE_CMD : TYPE_STRING;
                maketoken(expanded, vars);
                *first_token = 0;  // No longer the first token
            }
            
            ft_safefree((void **)&expanded);
        }
        else
        {
            // Empty expansion
            vars->curr_type = TYPE_STRING;
            maketoken("", vars);
            fprintf(stderr, "DEBUG: Created empty token for undefined variable\n");
        }
    }
    
    vars->start = vars->pos;
}

/*
Handles token boundary at whitespace or end of input.
- Processes any accumulated text before boundary.
- Updates position markers for next token.
Returns:
Nothing (void function).
Works with handle_token().

Example: For "echo hello" when reaching space
- Processes "echo" as command token
- Updates position markers to start after space
*/
void	handle_token_boundary(char *str, t_vars *vars, int *first_token)
{
    if (vars->pos > vars->start)
    {
        process_text(str, vars, first_token, 0);
        skip_whitespace(str, vars);
        vars->start = vars->pos;
    }
    else
    {
        skip_whitespace(str, vars);
        vars->start = vars->pos;
    }
}

/*
Creates an operator token and adds to token list.
- Handles different operator types (pipe, redirections).
- Creates appropriate node based on operator type.
Returns:
Nothing (void function).
Works with handle_token().

Example: For | operator
- Creates TYPE_PIPE node
- Adds to token linked list
*/
void	create_operator_token(t_vars *vars, t_tokentype type, char *symbol)
{
    t_node	*operator_node;
    
    operator_node = initnode(type, symbol);
    if (!operator_node)
        return ;
    vars->curr_type = type;
    build_token_linklist(vars, operator_node);
    fprintf(stderr, "DEBUG: Added %s operator token\n", 
        get_token_str(type));
}

/*
Processes operator tokens like pipe and redirections.
- Handles single and double character operators.
- Updates position and token list appropriately.
Returns:
Nothing (void function).
Works with handle_token().

Example: For input with ">" operator
- Creates output redirection token
- Updates position past the operator
*/
void	handle_operator_token(char *str, t_vars *vars, int *first_token)
{
    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
        
    if (str[vars->pos] == '|')
    {
        create_operator_token(vars, TYPE_PIPE, "|");
        fprintf(stderr, "DEBUG: Created pipe operator token with type=%d\n", TYPE_PIPE);
    }
    else if (str[vars->pos] == '>' && str[vars->pos + 1] == '>')
    {
        create_operator_token(vars, TYPE_APPEND_REDIRECT, ">>");
        vars->pos++;
    }
    else if (str[vars->pos] == '<' && str[vars->pos + 1] == '<')
    {
        create_operator_token(vars, TYPE_HEREDOC, "<<");
        vars->pos++;
    }
    else if (str[vars->pos] == '>')
        create_operator_token(vars, TYPE_OUT_REDIRECT, ">");
    else if (str[vars->pos] == '<')
        create_operator_token(vars, TYPE_IN_REDIRECT, "<");
        
    vars->pos++;
    vars->start = vars->pos;
}

/*
Processes a single token in the input string.
- Handles different token types: operators, quotes, expansions.
- Updates position and token list as tokens are processed.
Returns:
Nothing (void function).
Works with lexerlist().

Example: For input "echo $HOME"
- Processes "echo" as command token
- Processes "$HOME" as expansion token
*/
void	handle_token(char *str, t_vars *vars)
{
    int	first_token;
    
    first_token = 1;
    while (str && str[vars->pos])
    {
        if (str[vars->pos] == ' ' || str[vars->pos] == '\t' 
            || str[vars->pos] == '\n')
            handle_token_boundary(str, vars, &first_token);
        else if (vars->quote_depth == 0 && (str[vars->pos] == '|' 
            || str[vars->pos] == '>' || str[vars->pos] == '<'))
            handle_operator_token(str, vars, &first_token);
        else if ((str[vars->pos] == '\'' || str[vars->pos] == '"') 
            && vars->quote_depth == 0)
            handle_quote_content(str, vars, &first_token);
        else if (str[vars->pos] == '$' && vars->quote_depth == 0)
            handle_expansion_token(str, vars, &first_token);
        else
            vars->pos++;
    }
    if (vars->pos > vars->start)
        process_text(str, vars, &first_token, 0);
}




/*
Main lexical analysis function.
- Converts input string into linked list of tokens.
- Initializes lexer state and processes all tokens.
- Handles special cases like unclosed quotes.
Returns:
Nothing (void function).
Works with verify_input() and other high-level functions.

Example: Input "echo hello | grep world"
- Creates tokens: TYPE_CMD(echo, hello), TYPE_PIPE, TYPE_CMD(grep, world)
- Builds linked list in vars->head
*/
void	lexerlist(char *str, t_vars *vars)
{
    vars->pos = 0;
    vars->start = 0;
    vars->head = NULL;
    vars->current = NULL;
    
    fprintf(stderr, "DEBUG: Starting lexer list for: '%s'\n", str);
    
    if (!str || !*str)
        return ;
    
    handle_token(str, vars);
    
    if (vars->quote_depth > 0 && str)
        str = lexing_unclosed_quo(str, vars);
    
    if (vars->head)
        fprintf(stderr, "DEBUG: Tokens created, first: %s\n", 
            get_token_str(vars->head->type));
    else
        fprintf(stderr, "DEBUG: No tokens created!\n");
}
