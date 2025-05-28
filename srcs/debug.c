/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:18:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 05:54:20 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// In your append_args.c or similar function
void debug_args_before_after(t_node *cmd_node, char *new_arg, int is_delim) {
    char **args = cmd_node->args;
    int i = 0;
    
    write(2, "DEBUG-ARGS: Before append - [", 29);
    while (args && args[i]) {
        write(2, args[i], strlen(args[i]));
        write(2, " ", 1);
        i++;
    }
    write(2, "]\n", 2);
    
    write(2, "DEBUG-ARGS: Appending: '", 24);
    write(2, new_arg, strlen(new_arg));
    write(2, is_delim ? "' (DELIM)\n" : "'\n", is_delim ? 10 : 2);
}

// In your process_heredoc or similar function
void debug_heredoc_content(const char *content, size_t length) {
    write(2, "DEBUG-HEREDOC: Content (", 24);
    char len_str[20];
    sprintf(len_str, "%zu", length);
    write(2, len_str, strlen(len_str));
    write(2, " bytes):\n", 10);
    write(2, content, length > 100 ? 100 : length);
    if (length > 100)
        write(2, "...(truncated)", 14);
    write(2, "\n", 1);
}

// Helper function to check if token is a redirection operator
int is_redirection_operator(char *token)
{
	if (!token)
		return 0;
	if (ft_strcmp(token, "<") == 0 || ft_strcmp(token, ">") == 0 || 
		ft_strcmp(token, ">>") == 0 || ft_strcmp(token, "<<") == 0)
		return 1;
	return 0;
}

// Helper function to get redirection type
int get_redirection_type(char *token)
{
	if (!token)
		return 0;
	if (ft_strcmp(token, "<") == 0)
		return TYPE_IN_REDIRECT;
	if (ft_strcmp(token, ">") == 0)
		return TYPE_OUT_REDIRECT;
	if (ft_strcmp(token, ">>") == 0)
		return TYPE_APPEND_REDIRECT;
	if (ft_strcmp(token, "<<") == 0)
		return TYPE_HEREDOC;
	return 0;
}

// Helper function to check if token is a pipe operator
int is_pipe_operator(char *token)
{
	if (!token)
		return 0;
	return (ft_strcmp(token, "|") == 0);
}

// Add this debug function to track tokenization state
void debug_tokenize_state(const char *input, int pos, int token_type, const char *context)
{
	fprintf(stderr, "DEBUG-TOKENIZE-STATE: pos=%d, char='%c', context='%s', determined_type=%s\n",
			pos, 
			input[pos] ? input[pos] : '\0',  // Fixed: single character constant
			context,
			get_token_str(token_type));
	
	// Show surrounding context
	int start = (pos >= 5) ? pos - 5 : 0;
	int end = pos + 5;
	fprintf(stderr, "DEBUG-TOKENIZE-STATE: context='");
	for (int i = start; i < end && input[i]; i++) {
		if (i == pos) fprintf(stderr, "[%c]", input[i]);
		else fprintf(stderr, "%c", input[i]);
	}
	fprintf(stderr, "'\n");
}

// Add this to track what triggers CMD vs ARGS classification
int determine_token_type_with_debug(char *token, t_vars *vars, int position)
{
	int result_type;
	static int last_token_type = -1;
	static int expecting_command = 1;  // Start expecting a command
	static int after_redirection = 0;
	
	(void)vars;  // Mark as intentionally unused
	
	fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: Analyzing token '%s' at position %d\n", token, position);
	fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: State: expecting_command=%d, after_redirection=%d, last_token=%s\n",
			expecting_command, after_redirection, 
			last_token_type >= 0 ? get_token_str(last_token_type) : "NONE");
	
	// Check if this is a redirection operator
	if (is_redirection_operator(token)) {
		fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: '%s' is redirection operator\n", token);
		result_type = get_redirection_type(token);
		after_redirection = 1;  // Next token will be filename
		expecting_command = 0;  // Not expecting command next
	}
	// Check if this is a pipe
	else if (is_pipe_operator(token)) {
		fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: '%s' is pipe operator\n", token);
		result_type = TYPE_PIPE;
		after_redirection = 0;
		expecting_command = 1;  // After pipe, expect command
	}
	// Check if we're expecting a filename after redirection
	else if (after_redirection) {
		fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: '%s' is redirection filename (after_redirection=1)\n", token);
		result_type = TYPE_ARGS;  // Redirection filename
		after_redirection = 0;
		expecting_command = 1;    // CRITICAL: After filename, expect command again
	}
	// Check if we should treat this as a command
	else if (expecting_command) {
		fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: '%s' classified as CMD (expecting_command=1)\n", token);
		result_type = TYPE_CMD;
		expecting_command = 0;    // After command, expect args
	}
	// Otherwise it's an argument
	else {
		fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: '%s' classified as ARGS (default)\n", token);
		result_type = TYPE_ARGS;
	}
	
	fprintf(stderr, "DEBUG-TOKEN-CLASSIFY: Final classification: '%s' -> %s\n", 
			token, get_token_str(result_type));
	
	last_token_type = result_type;
	return result_type;
}

// Add this to track the tokenization process step by step
void debug_token_creation(char *token_str, int token_type, int position)
{
	fprintf(stderr, "DEBUG-TOKEN-CREATE: Creating token[%d]: '%s' as %s\n",
			position, token_str, get_token_str(token_type));
}

// Add this to the main tokenization loop
void debug_tokenization_loop(char *input, int start, int end, int token_count)
{
	char temp_char = input[end];
	input[end] = '\0';  // Temporarily null-terminate
	
	fprintf(stderr, "DEBUG-TOKENIZE-LOOP: Token %d: extracting '%s' from position %d-%d\n",
			token_count, &input[start], start, end-1);
	
	input[end] = temp_char;  // Restore original character
}

// Add this function to track pipeline context during tokenization
void debug_pipeline_awareness(t_vars *vars, int token_position)
{
    int has_pipe_before = 0;
    int has_redirection_before = 0;
    int redirection_after_pipe = 0;
    t_node *current = vars->head;
    t_node *last_pipe = NULL;
    
    fprintf(stderr, "DEBUG-PIPELINE-AWARE: Checking context for token position %d\n", token_position);
    
    // Count previous tokens and track pipeline structure
    int count = 0;
    while (current && count < token_position) {
        if (current->type == TYPE_PIPE) {
            has_pipe_before = 1;
            last_pipe = current;
            fprintf(stderr, "DEBUG-PIPELINE-AWARE: Found pipe at position %d\n", count);
        }
        if (is_redirection(current->type)) {
            has_redirection_before = 1;
            // Check if this redirection comes after the last pipe
            if (last_pipe && current_comes_after_node(current, last_pipe, vars->head)) {
                redirection_after_pipe = 1;
            }
            fprintf(stderr, "DEBUG-PIPELINE-AWARE: Found redirection %s at position %d (after_pipe=%d)\n",
                    get_token_str(current->type), count, redirection_after_pipe);
        }
        current = current->next;
        count++;
    }
    
    fprintf(stderr, "DEBUG-PIPELINE-AWARE: Context summary: has_pipe_before=%d, has_redirection_before=%d, redirection_after_pipe=%d\n",
            has_pipe_before, has_redirection_before, redirection_after_pipe);
}

// Helper function to check if node1 comes after node2 in the list
int current_comes_after_node(t_node *node1, t_node *node2, t_node *head)
{
    t_node *current = head;
    int found_node2 = 0;
    
    while (current) {
        if (current == node2) {
            found_node2 = 1;
        }
        if (found_node2 && current == node1) {
            return 1;
        }
        current = current->next;
    }
    return 0;
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

// Internal helper function with cycle detection
static void print_ast_node_helper(FILE *fp, t_node *node, int indent_level, 
								 void **visited, int visited_count)
{
	int i;

	if (!node || !fp)
		return;
	
	// Prevent infinite recursion with depth limit
	if (visited_count >= 100) {
		fprintf(fp, " [MAX DEPTH EXCEEDED]\n");
		return;
	}
	
	// Check for cycles in the AST
	for (i = 0; i < visited_count; i++) {
		if (visited[i] == node) {
			fprintf(fp, " [CYCLE DETECTED]\n");
			return;
		}
	}
	
	// Add this node to visited array
	visited[visited_count++] = node;
	
	// Original function implementation
	i = 0;
	while (i < indent_level) {
		fprintf(fp, "  ");
		i++;
	}
	print_node_content(fp, node);
	fprintf(fp, "\n");
	
	if (node->redir) {
		i = 0;
		while (i < indent_level) {
			fprintf(fp, "  ");
			i++;
		}
		fprintf(fp, "├─(redir)-> ");
		print_ast_node_helper(fp, node->redir, 0, visited, visited_count);
	}
	
	if (node->left) {
		i = 0;
		while (i < indent_level) {
			fprintf(fp, "  ");
			i++;
		}
		fprintf(fp, "├─(left)-> ");
		print_ast_node_helper(fp, node->left, indent_level + 1, visited, visited_count);
	}
	
	if (node->right) {
		i = 0;
		while (i < indent_level) {
			fprintf(fp, "  ");
			i++;
		}
		fprintf(fp, "└─(right)-> ");
		print_ast_node_helper(fp, node->right, indent_level + 1, visited, visited_count);
	}
}

void print_ast_node(FILE *fp, t_node *node, int indent_level)
{
	// Create an array to track visited nodes for cycle detection
	void *visited[100] = {0};
	int visited_count = 0;
	
	// Call internal helper with cycle detection
	print_ast_node_helper(fp, node, indent_level, visited, visited_count);
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
	// fprintf(fp, "=END=\n");
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
	fprintf(fp, "\n");
	if (filename)
		fclose(fp);
}

/*
Prints complete details about a node's structure and contents
*/
void	print_node_debug(t_node *node, const char *prefix, const char *location)
{
	fprintf(stderr, "DEBUG-NODE[%s@%s]: ", prefix, location);

	if (!node)
	{
		fprintf(stderr, "NULL node\n");
		return ;
	}
	// Print node type
	fprintf(stderr, "Type=%s", get_token_str(node->type));
	// Print arguments
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
}

/*
Prints detailed information about a node's arguments
*/
void	print_node_args(t_node *node, const char *prefix)
{
	int	i;

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
void print_ast_detailed(t_node *root, const char *prefix)
{
	if (!root)
	{
		fprintf(stderr, "DEBUG-%s: Empty AST (no root node)\n", prefix);
		return;
	}
	fprintf(stderr, "DEBUG-%s: = DETAILED AST =\n", prefix);
	
	// Queue for breadth-first traversal
	t_node **queue = malloc(sizeof(t_node *) * 1000);  // Assuming max 1000 nodes
	if (!queue)
	{
		fprintf(stderr, "DEBUG-%s: Memory allocation failed for AST traversal\n", prefix);
		return;
	}
	
	// Visited nodes tracking to prevent cycles
	void **visited = malloc(sizeof(void *) * 1000);
	if (!visited)
	{
		fprintf(stderr, "DEBUG-%s: Memory allocation failed for cycle detection\n", prefix);
		free(queue);
		return;
	}
	int visited_count = 0;
	
	int front = 0, rear = 0;
	queue[rear++] = root;
	
	while (front < rear)
	{
		t_node *node = queue[front++];
		
		// Check for cycles
		int is_cycle = 0;
		for (int i = 0; i < visited_count; i++)
		{
			if (visited[i] == node)
			{
				fprintf(stderr, "DEBUG-%s: [CYCLE DETECTED] Node(%p)\n", prefix, (void*)node);
				is_cycle = 1;
				break;
			}
		}
		
		if (is_cycle)
			continue;
			
		// Add to visited
		visited[visited_count++] = node;
		
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
		fprintf(stderr, ", Connections: {");
		if (node->left)
			fprintf(stderr, " left->%p", (void*)node->left);
		if (node->right)
			fprintf(stderr, " right->%p", (void*)node->right);
		if (node->redir)
			fprintf(stderr, " redir->%p", (void*)node->redir);
		if (node->prev)
			fprintf(stderr, " prev->%p", (void*)node->prev);
		if (node->next)
			fprintf(stderr, " next->%p", (void*)node->next);
		fprintf(stderr, " }\n");
		
		// Add children to queue
		if (node->left)
			queue[rear++] = node->left;
		if (node->right)
			queue[rear++] = node->right;
		if (node->redir)
			queue[rear++] = node->redir;
	}
	
	free(visited);
	free(queue);
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