/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:18:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/26 02:14:52 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
	i = 0;
	while (i < indent_level)
	{
		fprintf(fp, "  ");
		i++;
	}
	print_node_content(fp, node);
	fprintf(fp, "\n");
	if (node->redir)
	{
		i = 0;
		while (i < indent_level)
		{
			fprintf(fp, "  ");
			i++;
		}
		fprintf(fp, "├─(redir)-> ");
		print_ast_node(fp, node->redir, 0);
	}
	if (node->left)
	{
		i = 0;
		while (i < indent_level)
		{
			fprintf(fp, "  ");
			i++;
		}
		fprintf(fp, "├─(left)-> ");
		print_ast_node(fp, node->left, indent_level + 1);
	}
	if (node->right)
	{
		i = 0;
		while (i < indent_level)
		{
			fprintf(fp, "  ");
			i++;
		}
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
	fprintf(fp, "=END=\n");
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
	fprintf(fp, "= TOKEN LIST =\n");
	current = head;
	count = 0;
	while (current)
	{
		fprintf(fp, "[%d] ", count++);
		print_node_content(fp, current);
		fprintf(fp, "\n");
		current = current->next;
	}
	fprintf(fp, "=END=\n");
	if (filename)
		fclose(fp);
}

/*
Prints complete details about a node's structure and contents
*/
void print_node_debug(t_node *node, const char *prefix, const char *location)
{
	fprintf(stderr, "DEBUG-NODE[%s@%s]: ", prefix, location);
	
	if (!node) {
		fprintf(stderr, "NULL node\n");
		return;
	}
	
	// Print node type
	fprintf(stderr, "Type=%s", get_token_str(node->type));
	
	// Print arguments
	fprintf(stderr, ", Args=[");
	if (node->args) {
		int i = 0;
		while (node->args[i]) {
			fprintf(stderr, "'%s'", node->args[i]);
			if (node->args[i+1])
				fprintf(stderr, ", ");
			i++;
		}
	} else {
		fprintf(stderr, "NULL");
	}
	fprintf(stderr, "]");
	
	// Print connections
	fprintf(stderr, ", Connections: left=%p, right=%p, redir=%p, prev=%p, next=%p\n",
		(void*)node->left, (void*)node->right, (void*)node->redir, 
		(void*)node->prev, (void*)node->next);
}

/*
Prints detailed information about a node's arguments
*/
void	print_node_args(t_node *node, const char *prefix)
{
	int i;

	if (!node)
	{
		fprintf(stderr, "DEBUG-%s: NULL node\n", prefix);
		return ;
	}
	fprintf(stderr, "DEBUG-%s: Node type=%s, Args=[", 
			prefix, get_token_str(node->type));
	if (node->args)
	{
		i = 0;
		while (node->args[i])
		{
			fprintf(stderr, "'%s'", node->args[i]);
			if (node->args[i+1])
				fprintf(stderr, ", ");
			i++;
		}
	}
	else
	{
		fprintf(stderr, "NULL");
	}
	fprintf(stderr, "]\n");
}

/*
Prints a visualization of a linked list of nodes
*/
void	print_node_linked_list(t_node *head, const char *prefix)
{
	t_node	*current;
	int		count;

	if (!head)
	{
		fprintf(stderr, "DEBUG-%s: Empty linked list\n", prefix);
		return ;
	}
	fprintf(stderr, "DEBUG-%s: = LINKED LIST =\n", prefix);
	current = head;
	count = 0;
	while (current)
	{
		fprintf(stderr, "DEBUG-%s: [%d] ", prefix, count++);
		print_node_debug(current, prefix, "linked_list");
		current = current->next;
	}
	fprintf(stderr, "DEBUG-%s: = END LINKED LIST (%d nodes) =\n", prefix, count);
}

/*
Prints a detailed tree visualization of the AST with pointer connections
*/
void	print_ast_detailed(t_node *root, const char *prefix)
{
	if (!root)
	{
		fprintf(stderr, "DEBUG-%s: Empty AST (no root node)\n", prefix);
		return ;
	}
	fprintf(stderr, "DEBUG-%s: = DETAILED AST =\n", prefix);
	// Queue for breadth-first traversal
	t_node **queue = malloc(sizeof(t_node *) * 1000);  // Assuming max 1000 nodes
	if (!queue)
	{
		fprintf(stderr, "DEBUG-%s: Memory allocation failed for AST traversal\n", prefix);
		return ;
	}
	int front = 0, rear = 0;
	queue[rear++] = root;
	while (front < rear)
	{
		t_node *node = queue[front++];
		// Print current node details
		fprintf(stderr, "DEBUG-%s: Node(%p):", prefix, (void*)node);
		fprintf(stderr, " Type=%s", get_token_str(node->type));
		// Print args
		fprintf(stderr, ", Args=[");
		if (node->args)
		{
			int i = 0;
			while (node->args[i])
			{
				fprintf(stderr, "'%s'", node->args[i]);
				if (node->args[i+1])
					fprintf(stderr, ", ");
				i++;
			}
		}
		else
		{
			fprintf(stderr, "NULL");
		}
		fprintf(stderr, "]");
		// Print connections
		fprintf(stderr, ", Connections: left=%p, right=%p, redir=%p, prev=%p, next=%p\n",
			(void*)node->left, (void*)node->right, (void*)node->redir, 
			(void*)node->prev, (void*)node->next);
		// Add children to queue
		if (node->left)
			queue[rear++] = node->left;
		if (node->right)
			queue[rear++] = node->right;
		if (node->redir)
			queue[rear++] = node->redir;
	}
	free(queue);
	fprintf(stderr, "DEBUG-%s: = END DETAILED AST =\n", prefix);
}

/*
Convenience wrapper functions for easy debugging
*/

/*
Quick print AST to stdout
*/
void debug_print_ast(t_node *root)
{
	print_ast(root, NULL);
}

/*
Quick print token list to stdout  
*/
void debug_print_tokens(t_node *head)
{
	print_token_list(head, NULL);
}

/*
Quick print detailed AST analysis to stderr
*/
void debug_analyze_ast(t_node *root)
{
	print_ast_detailed(root, "AST");
}

/*
Quick print linked list analysis to stderr
*/
void debug_analyze_list(t_node *head)
{
	print_node_linked_list(head, "LIST");
}

/*
Quick print single node details to stderr
*/
void debug_print_node(t_node *node, const char *context)
{
	print_node_debug(node, "NODE", context ? context : "debug");
}

/*
Print AST to file
*/
void debug_save_ast(t_node *root, const char *filename)
{
	print_ast(root, filename);
}

/*
Print token list to file
*/
void debug_save_tokens(t_node *head, const char *filename)
{
	print_token_list(head, filename);
}
