/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:04:06 by bleow             #+#    #+#             */
/*   Updated: 2025/04/06 20:47:08 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Validates if all quotes in a command are properly closed
Returns true if all quotes are balanced, false otherwise
Also updates quote_ctx and quote_depth in vars
*/
int validate_quotes(char *input, t_vars *vars)
{
    int i;
    char in_quote;
     
    i = 0;
    in_quote = 0;
    vars->quote_depth = 0; 
    while (input[i])
    {
        if (!in_quote && (input[i] == '\'' || input[i] == '"'))
        {
            in_quote = input[i];
            if (vars->quote_depth < 32)
            {
                vars->quote_ctx[vars->quote_depth].type = in_quote;
                vars->quote_ctx[vars->quote_depth].start_pos = i;
                vars->quote_ctx[vars->quote_depth].depth = vars->quote_depth + 1;
                vars->quote_depth++;
            }
        }
        else if (in_quote && input[i] == in_quote)
        {
            in_quote = 0;
            vars->quote_depth--; // Pop completed quote
        }
        i++;
    } 
    // If we're still in a quote at the end, quotes are incomplete
    return (in_quote == 0);
}

/*
Reads additional input to complete unclosed quotes
Returns completed input or NULL on error
*/
char *fix_open_quotes(char *original_input, t_vars *vars)
{
    char *prompt;
    char *additional_input;
    char *combined_input;
    
    // Check if we need quote completion
    if (!vars->quote_depth)
        return (ft_strdup(original_input));
    
    // Create appropriate prompt based on quote type
    if (vars->quote_ctx[vars->quote_depth - 1].type == '\'')
        prompt = "SQUOTE> ";
    else
        prompt = "DQUOTE> ";
    
    // Get additional input
    additional_input = readline(prompt);
    
    if (!additional_input)
        return (NULL);
    
    // Add to history if not empty
    if (additional_input && *additional_input)
        add_history(additional_input);
    
    // Combine inputs
    combined_input = append_input(original_input, additional_input);
    free(additional_input);
    
    if (!combined_input)
        return (NULL);
    
    // Check if we have resolved all quotes
    if (!validate_quotes(combined_input, vars))
    {
        // Still have unclosed quotes, recursively get more input
        char *temp = combined_input;
        combined_input = fix_open_quotes(temp, vars);
        free(temp);
    }
    
    return (combined_input);
}

/*
 * Processes quotes in argument tokens
 * - Removes the outside quotes but preserves the content
 * - For single quotes: preserves literal contents including $ signs
 * - For double quotes: keeps content for later expansion
 * - Handles adjacent quoted text by joining with previous arg when needed
 */
void process_arg_quotes(char **arg)
{
    char *str;
    size_t len;
    char *new_str;
    
    if (!arg || !*arg)
        return;
        
    str = *arg;
    len = ft_strlen(str);
    
    DBG_PRINTF(DEBUG_QUOTES, "process_arg_quotes: Processing '%s'\n", str);
    
    // Need at least 2 chars for quotes
    if (len < 2)
        return;
        
    // Check for matching quotes at start and end
    if ((str[0] == '"' && str[len - 1] == '"') || 
        (str[0] == '\'' && str[len - 1] == '\''))
    {
        DBG_PRINTF(DEBUG_QUOTES, "Removing quotes from '%s'\n", str);
        new_str = ft_substr(str, 1, len - 2);
        if (new_str)
        {
            free(*arg);  // Free the string content, not the pointer
            *arg = new_str;
            DBG_PRINTF(DEBUG_QUOTES, "After removing quotes: '%s'\n", *arg);
        }
    }
}
