/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_example.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 01:30:00 by bleow             #+#    #+#             */
/*   Updated: 2025/05/26 02:10:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

/*
Example demonstrating how to use the refactored debug functions.
This shows how to call debug functions directly without DEBUG flags.

To use these functions in your code:
1. Include the header file
2. Call any debug function directly
3. No need to check DEBUG flags anymore

Examples:
- debug_print_ast(vars->astroot);           // Print AST to stdout
- debug_print_tokens(vars->head);           // Print token list to stdout  
- debug_analyze_ast(vars->astroot);         // Detailed AST analysis to stderr
- debug_print_node(node, "parsing");       // Print single node info
- debug_save_ast(vars->astroot, "ast.txt"); // Save AST to file
*/

void	demonstrate_debug_functions(t_vars *vars)
{
	printf("=== DEMONSTRATION OF REFACTORED DEBUG FUNCTIONS ===\n\n");
	// Check if we have tokens to display
	if (vars->head)
	{
		printf("1. Printing token list to stdout:\n");
		debug_print_tokens(vars->head);
		printf("\n");
		printf("2. Analyzing token list (output to stderr):\n");
		debug_analyze_list(vars->head);
		printf("\n");
	}
	// Check if we have an AST to display
	if (vars->astroot)
	{
		printf("3. Printing AST to stdout:\n");
		debug_print_ast(vars->astroot);
		printf("\n");
		printf("4. Detailed AST analysis (output to stderr):\n");
		debug_analyze_ast(vars->astroot);
		printf("\n");
		printf("5. Saving AST to file 'debug_ast_output.txt':\n");
		debug_save_ast(vars->astroot, "debug_ast_output.txt");
		printf("AST saved to file.\n\n");
	}
	// Demonstrate single node debugging
	if (vars->head)
	{
		printf("6. Debugging a single node:\n");
		debug_print_node(vars->head, "example_context");
		printf("\n");
	}
	printf("=== END DEMONSTRATION ===\n");
}

/*
To integrate these debug functions into your minishell:

1. At any point in your code where you want to debug, simply call:
   debug_print_ast(vars->astroot);
   
2. For detailed analysis:
   debug_analyze_ast(vars->astroot);
   debug_analyze_list(vars->head);
   
3. To save debug output to files:
   debug_save_ast(vars->astroot, "debug_ast.txt");
   debug_save_tokens(vars->head, "debug_tokens.txt");
   
4. For single node inspection:
   debug_print_node(current_node, "function_name");

No more need for DEBUG flags or setup_debug_flags() system!
*/
