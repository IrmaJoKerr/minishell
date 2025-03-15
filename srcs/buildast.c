/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/03/15 09:03:18 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Process the token list from vars->head to identify commands
and connect pipe nodes.
OLD VERSION
t_node	*process_token_list(t_vars *vars)
{
    t_node	*current;
    t_node  *pipe_node;
    
    vars->cmd_count = 0;
    current = vars->head;
    pipe_node = NULL;
    
    while (current)
    {
        // Store command nodes for easy access
        if (current->type == TYPE_CMD)
        {
            if (vars->cmd_count < 100)  // Avoid buffer overflow
                vars->cmd_nodes[vars->cmd_count++] = current;
        }
        // Find pipe nodes and link commands
        else if (current->type == TYPE_PIPE)
        {
            // Only process the pipe if there are commands before and after it
            if (current->prev && current->next && 
                current->prev->type == TYPE_CMD &&
                (current->next->type == TYPE_CMD || current->next->type == TYPE_PIPE))
            {
                current->left = current->prev;
                current->right = current->next;
                
                fprintf(stderr, "DEBUG: Created pipe: %s | %s\n", 
                    current->prev->args[0], 
                    current->next->args[0]);
                
                // Track the first pipe node
                if (!pipe_node)
                    pipe_node = current;
            }
        }
        current = current->next;
    }
    
    if (pipe_node)
        fprintf(stderr, "DEBUG: AST built with %d commands\n", vars->cmd_count);
    
    return pipe_node;
}
*/
/*
Process the token list from vars->head to identify commands
and connect pipe nodes.
OLD VERSION
t_node *process_token_list(t_vars *vars)
{
    t_node *current;
    t_node *pipe_root = NULL;
    t_node *last_pipe = NULL;
    t_node *last_cmd = NULL;
    
    vars->cmd_count = 0;
    
    if (!vars || !vars->head)
    {
        fprintf(stderr, "DEBUG: No tokens to process\n");
        return NULL;
    }
    
    current = vars->head;
    fprintf(stderr, "DEBUG: Processing token list for AST\n");
    
    while (current)
    {
        fprintf(stderr, "DEBUG: Processing node type: %d\n", current->type);
        
        // Store command nodes for easy access
        if (current->type == TYPE_CMD)
        {
            if (vars->cmd_count < 100) // Avoid buffer overflow
            {
                vars->cmd_nodes[vars->cmd_count++] = current;
                fprintf(stderr, "DEBUG: Added command node: '%s'\n", 
                       current->args[0]);
                last_cmd = current;
            }
        }
        else if (current->type == TYPE_PIPE)
        {
            fprintf(stderr, "DEBUG: Found pipe node\n");
            
            if (!pipe_root)
            {
                // First pipe in the chain
                pipe_root = current;
                
                // Connect commands before and after pipe
                if (last_cmd && current->next)
                {
                    pipe_root->left = last_cmd;
                    pipe_root->right = current->next;
                    fprintf(stderr, "DEBUG: Setting pipe root with left: %s\n", 
                           last_cmd->args ? last_cmd->args[0] : "NULL");
                }
                last_pipe = pipe_root;
            }
            else
            {
                // Additional pipes - create a cascading structure
                if (current->next)
                {
                    t_node *new_pipe = initnode(TYPE_PIPE, "|");
                    if (!new_pipe)
                        continue;
                    
                    // Link new pipe to the chain
                    last_pipe->right = new_pipe;
                    new_pipe->left = last_cmd;
                    new_pipe->right = current->next;
                    last_pipe = new_pipe;
                    
                    fprintf(stderr, "DEBUG: Added additional pipe with left: %s\n",
                           last_cmd->args ? last_cmd->args[0] : "NULL");
                }
            }
        }
        current = current->next;
    }
    
    return pipe_root;
}
*/
t_node *process_token_list(t_vars *vars)
{
    t_node *current;
    t_node *pipe_root = NULL;
    t_node *last_pipe = NULL;
    t_node *last_cmd = NULL;
    
    vars->cmd_count = 0;
    
    if (!vars || !vars->head)
    {
        fprintf(stderr, "DEBUG: No tokens to process\n");
        return NULL;
    }
    
    current = vars->head;
    fprintf(stderr, "DEBUG: Processing token list for AST\n");
    
    while (current)
    {
        fprintf(stderr, "DEBUG: Processing node type: %d\n", current->type);
        
        // Store command nodes for easy access
        if (current->type == TYPE_CMD)
        {
            if (vars->cmd_count < 100) // Avoid buffer overflow
            {
                vars->cmd_nodes[vars->cmd_count++] = current;
                fprintf(stderr, "DEBUG: Added command node: '%s'\n", 
                       current->args[0]);
                last_cmd = current;
            }
        }
        else if (current->type == TYPE_PIPE)
        {
            fprintf(stderr, "DEBUG: Found pipe node\n");
            
            if (!pipe_root)
            {
                // First pipe in the chain
                pipe_root = current;
                
                // Connect commands before and after pipe
                if (current->prev && current->prev->type == TYPE_CMD)
                {
                    pipe_root->left = current->prev;
                    
                    // Find next command
                    t_node *next_cmd = current->next;
                    while (next_cmd && next_cmd->type != TYPE_CMD)
                        next_cmd = next_cmd->next;
                        
                    if (next_cmd)
                    {
                        pipe_root->right = next_cmd;
                        fprintf(stderr, "DEBUG: Setting pipe root with left: %s\n", 
                               current->prev->args[0]);
                    }
                }
                last_pipe = pipe_root;
            }
            else
            {
                // Additional pipes - create a cascading structure
                t_node *next_cmd = current->next;
                while (next_cmd && next_cmd->type != TYPE_CMD)
                    next_cmd = next_cmd->next;
                    
                if (last_cmd && next_cmd)
                {
                    current->left = last_cmd;
                    current->right = next_cmd;
                    
                    // Link this pipe to the previous pipe's right
                    last_pipe->right = current;
                    
                    fprintf(stderr, "DEBUG: Added additional pipe with left: %s\n",
                           last_cmd->args ? last_cmd->args[0] : "NULL");
                           
                    last_pipe = current;
                }
            }
        }
        else if (current->type == TYPE_OUT_REDIRECT || 
                 current->type == TYPE_APPEND_REDIRECT ||
                 current->type == TYPE_IN_REDIRECT ||
                 current->type == TYPE_HEREDOC)
        {
            fprintf(stderr, "DEBUG: Processing redirection node\n");
            
            // Find the command this redirection applies to
            t_node *target_cmd = last_cmd;
            
            // Make sure we have both the redirection and the target file
            if (target_cmd && current->next && current->next->type == TYPE_CMD)
            {
                // Set up redirection
                current->left = target_cmd;
                current->right = current->next;
                
                fprintf(stderr, "DEBUG: Created redirection from %s to %s\n",
                       target_cmd->args[0], current->next->args[0]);
                
                // If this is part of a pipe structure, update accordingly
                if (last_pipe && last_pipe->right == target_cmd)
                {
                    last_pipe->right = current;
                }
                // If this is the first operation overall, make it the root
                else if (!pipe_root)
                {
                    pipe_root = current;
                }
            }
        }
        
        current = current->next;
    }
    
    return pipe_root;
}

/*
Determine the root node of the AST.
If a pipe was found, use it as root. Otherwise use the first command.
*/
t_node	*set_ast_root(t_node *pipe_node, t_vars *vars)
{
    // If we found a pipe, use it as the root
    if (pipe_node) 
    {
        fprintf(stderr, "DEBUG: Built AST successfully with root type %d\n", pipe_node->type);
        return pipe_node;
    }
    
    // If no pipe but we have commands, use the first command
    if (vars->cmd_count > 0) 
    {
        fprintf(stderr, "DEBUG: Built AST with single command\n");
        return vars->cmd_nodes[0];
    }
    
    // No valid nodes found
    fprintf(stderr, "DEBUG: No valid nodes found for AST\n");
    return NULL;
}

/*
Check if the token list has valid pipe syntax.
Returns:
  0 - valid pipe syntax
  1 - invalid pipe syntax (error)
  2 - pipe at end of input (needs more input)
OLD VERSION
int check_pipe_syntax_errors(t_vars *vars)
{
    t_node *current;
    
    if (!vars->head)
        return 0;  // No tokens, no errors
    
    // Check if first token is a pipe
    if (vars->head->type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Syntax error: pipe at beginning\n");
        return 1;
    }
    
    current = vars->head;
    while (current)
    {
        // Check for consecutive pipes
        if (current->type == TYPE_PIPE && current->next && current->next->type == TYPE_PIPE)
        {
            fprintf(stderr, "DEBUG: Syntax error: consecutive pipes\n");
            return 1;
        }
        
        // Check for pipe at the end
        if (current->type == TYPE_PIPE && !current->next)
        {
            fprintf(stderr, "DEBUG: Pipe at end, needs more input\n");
            return 2;
        }
        
        current = current->next;
    }
    
    return 0;
}
*/
int check_pipe_syntax_errors(t_vars *vars)
{
    t_node *current;
    int consecutive_pipes = 0;
    
    if (!vars->head)
    {
        fprintf(stderr, "DEBUG: No tokens to check pipe syntax\n");
        return 0;
    }
    
    // Check if first token is a pipe
    if (vars->head->type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Error: Pipe at start of input\n");
        ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
        vars->error_code = 258; // Standard shell syntax error code
        return 1;
    }
    
    current = vars->head;
    while (current)
    {
        // Check for consecutive pipes
        if (current->type == TYPE_PIPE)
        {
            consecutive_pipes++;
            if (consecutive_pipes > 1)
            {
                fprintf(stderr, "DEBUG: Error: Multiple consecutive pipes\n");
                ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
                vars->error_code = 258;
                return 1;
            }
            
            // Check if pipe is the last token
            if (!current->next)
            {
                fprintf(stderr, "DEBUG: Pipe at end of input, need more input\n");
                return 2;
            }
            
            // Check if next token is a pipe (which would make consecutive pipes)
            if (current->next && current->next->type == TYPE_PIPE)
            {
                fprintf(stderr, "DEBUG: Error: Multiple consecutive pipes\n");
                ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
                vars->error_code = 258;
                return 1;
            }
        }
        else
        {
            consecutive_pipes = 0;
        }
        
        current = current->next;
    }
    
    return 0;
}

/*
Handle a command with a pipe at the end.
Prompt user for more input and append it to the original command.
*/
char *handle_unfinished_pipe(char *input, t_vars *vars)
{
    char *line;
    char *new_input;
    char *temp;
    
    fprintf(stderr, "DEBUG: Processing unfinished pipe\n");
    
    // Clean up current token list before getting new input
    cleanup_token_list(vars);
    
    line = readline("COMMAND> ");
    if (!line)
        return input;  // EOF, return original input
    
    // Add the new command to history
    if (*line)
        add_history(line);
        
    // Combine original input and new input
    temp = ft_strjoin(input, " ");
    if (!temp)
    {
        free(line);
        return input;
    }
    
    new_input = ft_strjoin(temp, line);
    free(temp);
    free(line);
    
    if (!new_input)
        return input;
        
    fprintf(stderr, "DEBUG: Processing command: '%s'\n", new_input);
    
    // Re-tokenize the combined input
    tokenize(new_input, vars);
    lexerlist(new_input, vars);
    
    return new_input;
}

/*
Master control function to build AST from tokens.
OLD VERSION
t_node	*build_ast(t_vars *vars)
{
    t_node *pipe_node;
    
    fprintf(stderr, "DEBUG: Building AST for commands and redirections\n");
    
    if (!vars || !vars->head)
    {
        fprintf(stderr, "DEBUG: No tokens (vars->head is NULL)!\n");
        return NULL;
    }
    
    // Process token list to identify commands and pipes
    pipe_node = process_token_list(vars);
    
    // Set the root node for the AST
    return set_ast_root(pipe_node, vars);
}
*/
t_node *build_ast(t_vars *vars)
{
    t_node *root = NULL;
    
    fprintf(stderr, "DEBUG: Building AST for commands and redirections\n");
    
    if (!vars || !vars->head)
    {
        fprintf(stderr, "DEBUG: No tokens (vars->head is NULL)!\n");
        return NULL;
    }
    
    // Process token list to identify commands and pipes
    root = process_token_list(vars);
    
    // If no pipe was found but we have at least one command, use first command as root
    if (!root && vars->cmd_count > 0)
    {
        root = vars->cmd_nodes[0];
        fprintf(stderr, "DEBUG: Using first command as root: %s\n", 
               root->args ? root->args[0] : "NULL");
    }
    
    return root;
}

/*
DEBUG FUNCTION: Print the token list for debugging.
*/
void	print_token_list(t_vars *vars)
{
    t_node *current;
    int i;
    int j;
    
    if (!vars || !vars->head)
        return;
    
    fprintf(stderr, "\n=== TOKEN LIST ===\n");
    
    current = vars->head;
    i = 0;
    while (current)
    {
        fprintf(stderr, "Token %d: Type=%d (%s), Value='%s', Args=[", 
                i, current->type, get_token_str(current->type), 
                current->args ? current->args[0] : "NULL");
        
        if (current->args)
        {
            j = 0;
            while (current->args[j])
            {
                fprintf(stderr, "'%s'", current->args[j]);
                if (current->args[j+1])
                    fprintf(stderr, ", ");
                j++;
            }
        }
        fprintf(stderr, "]\n");
        
        i++;
        current = current->next;
    }
    
    fprintf(stderr, "=== END TOKEN LIST ===\n\n");
}
