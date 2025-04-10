/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/04/11 00:50:11 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles adding expanded values to commands
- Adds as new argument if preceded by a command
- Joins with existing argument if within an argument
- Removes the expansion node from the token list
Returns 1 if processed successfully, 0 otherwise
*/
int handle_adjacent_args(t_node *expansion_node, char *expanded_value, t_vars *vars)
{
	t_node *cmd_node;
	int is_adjacent;
	int last_arg_idx = 0;
	char *joined;
	
	fprintf(stderr, "DEBUG[handle_adjacent_args]: ENTER with expansion_node=%p, expanded_value='%s'\n", 
			(void*)expansion_node, expanded_value ? expanded_value : "NULL");
	
	if (!expansion_node || !expanded_value || !vars)
		return (0);
	
	// Find the command node this expansion belongs to
	cmd_node = find_cmd(vars->head, expansion_node, FIND_PREV, vars);
	if (!cmd_node)
	{
		fprintf(stderr, "DEBUG[handle_adjacent_args]: No command found for expansion\n");
		free(expanded_value);
		return (0);
	}
	
	// Check if expansion is adjacent to previous token
	is_adjacent = (expansion_node->prev && 
				  (expansion_node->prev->type == TYPE_ARGS));
	
	if (is_adjacent && expansion_node->prev->type != TYPE_CMD)
	{
		// Join with last argument if adjacent to an ARGS token
		while (expansion_node->prev->args[last_arg_idx + 1])
			last_arg_idx++;
		
		fprintf(stderr, "DEBUG[handle_adjacent_args]: Joining with arg: '%s'\n", 
				expansion_node->prev->args[last_arg_idx]);
				
		joined = ft_strjoin(expansion_node->prev->args[last_arg_idx], expanded_value);
		if (joined)
		{
			free(expansion_node->prev->args[last_arg_idx]);
			expansion_node->prev->args[last_arg_idx] = joined;
			free(expanded_value);
		}
	}
	else
	{
		// Add as new argument to the command
		fprintf(stderr, "DEBUG[handle_adjacent_args]: Adding as new argument to '%s'\n", 
				cmd_node->args[0]);
				
		append_arg(cmd_node, expanded_value, 0);
		free(expanded_value);
	}
	
	// Remove the expansion node from the token list
	fprintf(stderr, "DEBUG[handle_adjacent_args]: Cleaning up expansion node\n");
	
	// CRITICAL FIX: Store the next node before unlinking and freeing
	// t_node *next_node = expansion_node->next;
	
	if (expansion_node->prev)
		expansion_node->prev->next = expansion_node->next;
	if (expansion_node->next)
		expansion_node->next->prev = expansion_node->prev;
		
	if (expansion_node == vars->head)
		vars->head = expansion_node->next;
	
	fprintf(stderr, "DEBUG[handle_adjacent_args]: calling free_token_node\n");
	free_token_node(expansion_node);
	
	fprintf(stderr, "DEBUG[handle_adjacent_args]: EXIT\n");
	return (1);
}

/*
Expands a variable name to its value
Handles special cases like lone $, $?, and environment variables
Returns newly allocated string with the expanded value
*/
char	*expand_value(char *var_name, t_vars *vars)
{
	char	*expanded_value;
	fprintf(stderr, "DEBUG[expand_value]: Called with var_name='%s', vars=%p\n", 
		var_name ? var_name : "NULL", (void*)vars);
	if (!var_name)
		return (ft_strdup(""));
		
	// Handle different expansion types
	if (var_name[0] == '\0')
	{
		// Lone $ character
		expanded_value = ft_strdup("$");
		fprintf(stderr, "DEBUG: Handling lone $\n");
	}
	else if (ft_strcmp(var_name, "?") == 0)
	{
		// Special case for $?
		expanded_value = ft_itoa(vars->error_code);
		fprintf(stderr, "DEBUG: Handling $?, expanded to '%s'\n", expanded_value);
	}
	else
	{
		// Regular environment variable
		expanded_value = get_env_val(var_name, vars->env);
		if (!expanded_value)
			expanded_value = ft_strdup("");
		fprintf(stderr, "DEBUG: Expanded env var '%s' to '%s'\n", var_name, expanded_value);
	}
	return (expanded_value);
}

/*
Processes variable expansions in the token list
- First pass handles TYPE_EXPANSION tokens
- Second pass handles expansions in quoted args
- Updates token types and handles adjacent arguments
- Fixed to avoid use-after-free errors when removing nodes
*/
void process_expansions(t_vars *vars)
{
	t_node	*current;
	t_node	*next;  // Added to track next node safely
	t_node	*cmd_node;
	char	*var_name;
	char	*expanded_value;
	
	fprintf(stderr, "DEBUG: Starting process_expansions()\n");
	
	if (!vars || !vars->head)
		return;
		
	current = vars->head;
	
	// First pass: handle TYPE_EXPANSION tokens
	while (current)
	{
		// CRITICAL FIX: Save the next node before any potential freeing
		next = current->next;
		
		if (current->type == TYPE_EXPANSION || current->type == TYPE_EXIT_STATUS)
		{
			if (current->type == TYPE_EXIT_STATUS)
				var_name = "?";  // Special handling for $?
			else
				var_name = current->args[0] + 1;
				
			fprintf(stderr, "DEBUG: Process_expansions: calling expand_value\n");    
			expanded_value = expand_value(var_name, vars);
			cmd_node = find_cmd(vars->head, current, FIND_PREV, vars);
			
			// First token in command or expansion in command position should be TYPE_CMD
			if (current == vars->head || is_command_position(current, vars))
			{
				fprintf(stderr, "DEBUG: Expansion is in command position\n");
				
				// If the expansion resulted in an empty string
				if (!expanded_value || expanded_value[0] == '\0')
				{
					fprintf(stderr, "DEBUG: process_expansions : freeing empty value\n");
					// Free the empty value if it exists
					if (expanded_value)
						free(expanded_value);
					
					// Handle empty expansion logic...
					// (existing code for handling empty expansions)
				} 
				else 
				{
					fprintf(stderr, "DEBUG: process_expansions : handling normal case, non-empty command\n");
					// Normal case with non-empty command
					fprintf(stderr, "DEBUG: Freeing current->args[0]\n");
					free(current->args[0]);
					current->args[0] = expanded_value;
					current->type = TYPE_CMD;  // Always set to CMD when in command position
					fprintf(stderr, "DEBUG: Node is now TYPE_CMD with args[0]='%s'\n", 
							current->args[0]);
				}
			}
			// If adjacent to a command, add as argument
			else if (cmd_node)
			{
				fprintf(stderr, "DEBUG: Expansion is adjacent to command, adding as argument\n");
				handle_adjacent_args(current, expanded_value, vars);
				
				// CRITICAL FIX: After handle_adjacent_args, immediately use 'next'
				// since current has been freed
				current = next;
				continue;
			}
			else
			{
				fprintf(stderr, "DEBUG: Converting expansion to command, value='%s'\n", expanded_value);
				free(current->args[0]);
				current->args[0] = expanded_value;
				current->type = TYPE_CMD;  // When unsure, convert to command
			}
		}
		
		// CRITICAL FIX: Always advance using the saved 'next' pointer
		current = next;
	}
	
	// Second pass: handle expansions inside quoted args
	// (existing code for processing expansions inside arguments)
	
	// Final pass: ensure first token is always TYPE_CMD (important for $? case)
	if (vars->head && vars->head->type != TYPE_CMD)
	{
		fprintf(stderr, "DEBUG: Converting first token from type %d to TYPE_CMD\n", 
				vars->head->type);
		vars->head->type = TYPE_CMD;
	}
	
	// Debug trace the final token list
	fprintf(stderr, "DEBUG: Final token list after processing expansions:\n");
	current = vars->head;
	while (current)
	{
		// Added extra safety check
		char *arg_display = "NULL";
		if (current->args && current->args[0])
		{
			arg_display = current->args[0];
		}
		// Print the node type and first argument
		fprintf(stderr, "DEBUG: Node type=%d, args[0]='%s'\n", 
			current->type, arg_display);
		current = current->next;
	}
}

/*
Add this helper function to check if a node is in command position 
*/
int is_command_position(t_node *node, t_vars *vars)
{
	// First token in the list is always in command position
	if (node == vars->head)
		return 1;
		
	// If previous token is a pipe, this is in command position
	if (node->prev && node->prev->type == TYPE_PIPE)
		return 1;
		
	// If we're after a redirection and its target, this is in command position
	if (node->prev && node->prev->prev && 
		is_redirection(node->prev->prev->type) && 
		node->prev->prev->next == node->prev)
		return 1;
		
	return 0;
}
