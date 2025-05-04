/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:18:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/05 05:02:39 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	setup_debug_flags(void)
{
	char	*debug_env;

	debug_env = getenv("MINISHELL_DEBUG");
	if (!debug_env)
		return ;
	if (strstr(debug_env, "ast") || strstr(debug_env, "all"))
		#undef DEBUG_AST
		#define DEBUG_AST 1
	if (strstr(debug_env, "exec") || strstr(debug_env, "all"))
		#undef DEBUG_EXEC
		#define DEBUG_EXEC 1
	if (strstr(debug_env, "pipes") || strstr(debug_env, "all"))
		#undef DEBUG_PIPES
		#define DEBUG_PIPES 1
	if (strstr(debug_env, "redir") || strstr(debug_env, "all"))
		#undef DEBUG_REDIR
		#define DEBUG_REDIR 1
	if (strstr(debug_env, "error") || strstr(debug_env, "all"))
		#undef DEBUG_ERROR
		#define DEBUG_ERROR 1
	if (strstr(debug_env, "expand") || strstr(debug_env, "all"))
		#undef DEBUG_EXPAND
		#define DEBUG_EXPAND 1
	if (strstr(debug_env, "verbose") || strstr(debug_env, "all"))
		#undef DEBUG_VERBOSE
		#define DEBUG_VERBOSE 1
	if (strstr(debug_env, "none"))
	{
		#undef DEBUG_AST
		#define DEBUG_AST 0
		#undef DEBUG_EXEC
		#define DEBUG_EXEC 0
		#undef DEBUG_PIPES
		#define DEBUG_PIPES 0
		#undef DEBUG_REDIR
		#define DEBUG_REDIR 0
		#undef DEBUG_ERROR
		#define DEBUG_ERROR 0
		#undef DEBUG_EXPAND
		#define DEBUG_EXPAND 0
		#undef DEBUG_VERBOSE
		#define DEBUG_VERBOSE 0
	}
}

/*
Prints a node's content to the specified file.
*/
void	print_node_content(FILE *fp, t_node *node)
{
	int	i;

	if (!node || !fp)
		return ;
	fprintf(fp, "%s [", get_token_str(node->type));
	if (node->args)
	{
		i = 0;
		while (node->args[i])
		{
			fprintf(fp, "%s", node->args[i]);
			if (node->args[i + 1])
				fprintf(fp, " ");
			i++;
		}
	}
	fprintf(fp, "]");
}

/*
Recursively prints the AST starting from the given node.
indent_level controls the indentation for tree visualization.
*/
void	print_ast_node(FILE *fp, t_node *node, int indent_level)
{
	int	i;

	if (!node || !fp)
		return ;
	for (i = 0; i < indent_level; i++)
		fprintf(fp, "  ");
	print_node_content(fp, node);
	fprintf(fp, "\n");
	if (node->redir)
	{
		for (i = 0; i < indent_level; i++)
			fprintf(fp, "  ");
		fprintf(fp, "├─(redir)-> ");
		print_ast_node(fp, node->redir, 0);
	}
	if (node->left)
	{
		for (i = 0; i < indent_level; i++)
			fprintf(fp, "  ");
		fprintf(fp, "├─(left)-> ");
		print_ast_node(fp, node->left, indent_level + 1);
	}
	if (node->right)
	{
		for (i = 0; i < indent_level; i++)
			fprintf(fp, "  ");
		fprintf(fp, "└─(right)-> ");
		print_ast_node(fp, node->right, indent_level + 1);
	}
}

/*
Prints the entire AST to the specified file.
If filename is NULL, prints to stdout.
*/
void	print_ast(t_node *root, const char *filename)
{
	FILE	*fp;

	if (!root)
	{
		printf("Empty AST (no root node)\n");
		return ;
	}
	if (filename)
	{
		fp = fopen(filename, "w");
		if (!fp)
		{
			perror("Failed to open file for AST output");
			return ;
		}
	}
	else
		fp = stdout;
	fprintf(fp, "=== ABSTRACT SYNTAX TREE ===\n");
	print_ast_node(fp, root, 0);
	fprintf(fp, "===========================\n");
	if (filename)
		fclose(fp);
}

/*
Prints the token list (before AST construction) to the specified file.
If filename is NULL, prints to stdout.
*/
void	print_token_list(t_node *head, const char *filename)
{
	FILE	*fp;
	t_node	*current;
	int		count;

	if (!head)
	{
		printf("Empty token list\n");
		return ;
	}
	if (filename)
	{
		fp = fopen(filename, "w");
		if (!fp)
		{
			perror("Failed to open file for token list output");
			return ;
		}
	}
	else
		fp = stdout;
	fprintf(fp, "=== TOKEN LIST ===\n");
	current = head;
	count = 0;
	while (current)
	{
		fprintf(fp, "[%d] ", count++);
		print_node_content(fp, current);
		fprintf(fp, "\n");
		current = current->next;
	}
	fprintf(fp, "=================\n");
	if (filename)
		fclose(fp);
}
