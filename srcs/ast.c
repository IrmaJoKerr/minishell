/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:43:35 by bleow             #+#    #+#             */
/*   Updated: 2025/03/14 01:41:31 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Function to initialize the head node for the AST.
Uses initnode with TYPE_HEAD to create the node.
Example: ls | wc -l > file.txt
Head node is the first node "ls".
*/
/*
t_node	*init_head_node(t_vars *vars)
{
	t_node	*head;

	if (!vars)
		return (NULL);
	head = malloc(sizeof(t_node));
	if (!head)
	{
		ft_error(vars);
		return (NULL);
	}
	head->type = TYPE_HEAD;
	head->args = NULL;
	head->next = NULL;
	head->prev = NULL;
	head->left = NULL;
	head->right = NULL;
	return (head);
}
*/
/*
t_node *init_head_node(t_vars *vars)
{
    t_node *head;
    
    if (!vars)
        return (NULL);
    
    head = initnode(TYPE_HEAD, NULL);
    if (!head)
        ft_error(vars);
	(void)vars;
	return (NULL);
}
*/

/*
Function to build AST from tokens.
Builds pipe chains and redirections based on type using 2 while loops.
Example: ls | wc -l > file.txt
*/
/*
t_node *build_ast(t_vars *vars)
{
	t_node *current;
	t_node *root;

	printf("DEBUG: Entering build_ast\n");
	if (!vars)
	{
		printf("DEBUG: vars is NULL\n");
		return NULL;
	}
	if (!vars->head)
	{
		printf("DEBUG: vars->head is NULL\n");
		return NULL;
	}
	printf("DEBUG: First node type: %d\n", vars->head->type);
	if (vars->head->args && vars->head->args[0])
	{
		printf("DEBUG: First node args[0]: %s\n", vars->head->args[0]);
	}
	else
	{
		printf("DEBUG: First node has no args\n");
	}
	
	if (vars->head->type == TYPE_CMD && !vars->head->next)
	{
		printf("DEBUG: Simple command AST - using head as root\n");
		return vars->head;
	}
	current = vars->head;
	while (current && current->type != TYPE_CMD)
		current = current->next;
		
	if (current && current->type == TYPE_CMD)
	{
		printf("DEBUG: Found command node for AST root: ");
		if (current->args)
		{
			printf("%s\n", current->args[0]);
		}
		else
		{
			printf("NULL\n");
		}
		root = current;
	}
	else
	{
		printf("DEBUG: No command node found for AST\n");
		root = NULL;
	}
	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_PIPE)
			handle_pipe_node(&root, current);
		current = current->next;
	}
	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
			redirection_node(root, current);
		current = current->next;
	}
	if (root)
	{
		printf("DEBUG: Built AST with root command: ");
		if (root->args)
		{
			printf("%s\n", root->args[0]);
		}
		else
		{
			printf("NULL\n");
		}
	}
	else
	{
		printf("DEBUG: Failed to build AST - no root node\n");
	}
	return (root);
}
*/
/*
t_node *build_ast(t_vars *vars)
{
    t_node *current;
    t_node *root = NULL;
    t_node *cmd_nodes[100];  // Store command nodes
    t_node *pipe_nodes[100]; // Store pipe nodes
    int cmd_count = 0;
    int pipe_count = 0;

    printf("DEBUG: Entering build_ast\n");
    if (!vars || !vars->head)
    {
        printf("DEBUG: vars or vars->head is NULL\n");
        return NULL;
    }
    
    // Fast path for simple commands
    if (vars->head->type == TYPE_CMD)
    {
        printf("DEBUG: Simple command AST - using head as root\n");
        printf("DEBUG: First node type: %d\n", vars->head->type);
        if (vars->head->args && vars->head->args[0])
            printf("DEBUG: First node args[0]: %s\n", vars->head->args[0]);
        return vars->head;
    }

    // Collect all command and pipe nodes
    current = vars->head; // No longer need to check for TYPE_HEAD
    
    while (current)
    {
        if (current->type == TYPE_CMD || current->type == TYPE_STRING)
        {
            if (current->type == TYPE_STRING)
                current->type = TYPE_CMD;  // Convert string to command
                
            cmd_nodes[cmd_count++] = current;
            printf("DEBUG: Found command node: %s\n", 
                current->args ? current->args[0] : "null");
                
            if (!root)
                root = current;
        }
        else if (current->type == TYPE_PIPE)
        {
            pipe_nodes[pipe_count++] = current;
            printf("DEBUG: Found pipe node\n");
        }
        current = current->next;
    }
    
    // Process the pipe nodes to build the AST
    for (int i = 0; i < pipe_count; i++)
    {
        if (i < cmd_count - 1)
        {
            pipe_nodes[i]->left = cmd_nodes[i];
            pipe_nodes[i]->right = cmd_nodes[i+1];
            
            if (i == 0)
                root = pipe_nodes[i];  // First pipe becomes root
            else
            {
                // Chain pipes together
                pipe_nodes[i-1]->right = pipe_nodes[i];
                pipe_nodes[i]->left = pipe_nodes[i-1]->right;
            }
        }
    }
    // Process redirections
    current = vars->head;
    while (current)
    {
        if (is_redirection(current->type))
            redirection_node(root, current);
        current = current->next;
    }
    if (!root)
        printf("DEBUG: Failed to build AST - no root node\n");
    else
        printf("DEBUG: Built AST successfully\n");

    return root;
}
*/
/*
t_node *build_ast(t_vars *vars)
{
    t_node *current;
    t_node *root = NULL;
    t_node *cmd_nodes[100];  // Store command nodes
    t_node *pipe_nodes[100]; // Store pipe nodes
    int cmd_count = 0;
    int pipe_count = 0;

    printf("DEBUG: Entering build_ast\n");
    if (!vars || !vars->head)
    {
        printf("DEBUG: vars or vars->head is NULL\n");
        return NULL;
    }
    
    // Fast path for simple commands - ONLY if there's just one command token
    if (vars->head->type == TYPE_CMD && !vars->head->next)
    {
        printf("DEBUG: Simple command AST - using head as root\n");
        printf("DEBUG: First node type: %d\n", vars->head->type);
        if (vars->head->args && vars->head->args[0])
            printf("DEBUG: First node args[0]: %s\n", vars->head->args[0]);
        return vars->head;
    }

    // Collect all command and pipe nodes
    current = vars->head;
    
    // First pass - collect all command nodes
    while (current)
    {
        if (current->type == TYPE_CMD || current->type == TYPE_STRING)
        {
            if (current->type == TYPE_STRING)
                current->type = TYPE_CMD;  // Convert string to command
                
            cmd_nodes[cmd_count++] = current;
            printf("DEBUG: Found command node: %s\n", 
                current->args ? current->args[0] : "null");
        }
        
        // Look for pipe tokens to create pipe nodes
        if (current->type == TYPE_PIPE)
        {
            printf("DEBUG: Found pipe node\n");
            // Set up pipe relationships
            if (current->prev && current->next)
            {
                current->left = current->prev;   // Left side of pipe is previous command
                current->right = current->next;  // Right side of pipe is next command
                root = current;  // The pipe node becomes the root
                printf("DEBUG: Connected pipe: %s | %s\n",
                    current->left->args[0], current->right->args[0]);
            }
        }
        current = current->next;
    }
    
    // If we have at least one command node
    if (cmd_count > 0)
    {
        root = cmd_nodes[0];
        printf("DEBUG: Setting root to first command: %s\n", 
            root->args ? root->args[0] : "null");
    }
    
    // If we have at least one pipe and 2+ commands, connect them
    if (pipe_count > 0 && cmd_count >= 2)
    {
        for (int i = 0; i < pipe_count && i < cmd_count - 1; i++)
        {
            pipe_nodes[i]->left = cmd_nodes[i];
            pipe_nodes[i]->right = cmd_nodes[i+1];
            printf("DEBUG: Connected pipe %d: %s | %s\n", i,
                cmd_nodes[i]->args[0], cmd_nodes[i+1]->args[0]);
                
            // Make the first pipe node the root
            if (i == 0)
                root = pipe_nodes[i];
        }
    }
    
    if (!root)
        printf("DEBUG: Failed to build AST - no root node\n");
    else
        printf("DEBUG: Built AST successfully\n");

    return root;
}
*/
t_node *build_ast(t_vars *vars)
{
    t_node *current;
    t_node *root = NULL;
    t_node *cmd_nodes[100];  // Store command nodes
    int cmd_count = 0;
    
    printf("DEBUG: Entering build_ast\n");
    if (!vars || !vars->head)
    {
        printf("DEBUG: vars or vars->head is NULL\n");
        return NULL;
    }
    
    // Fast path for simple commands
    if (vars->head->type == TYPE_CMD && !vars->head->next)
    {
        printf("DEBUG: Simple command AST - using head as root\n");
        return vars->head;
    }

    // First scan: collect command nodes and identify pipes
    current = vars->head;
    while (current)
    {
        if (current->type == TYPE_CMD || current->type == TYPE_STRING)
        {
            if (current->type == TYPE_STRING)
                current->type = TYPE_CMD;
                
            cmd_nodes[cmd_count++] = current;
            printf("DEBUG: Found command node: %s\n", 
                current->args ? current->args[0] : "null");
        }
        
        // Important: if we find a pipe, connect it and make it the root
        if (current->type == TYPE_PIPE)
        {
            printf("DEBUG: Found pipe node\n");
            
            // A pipe needs a left and right command
            if (current->prev && current->next)
            {
                current->left = current->prev;   // Left side is previous command
                current->right = current->next;  // Right side is next command
                root = current;  // The pipe becomes the root of our AST
                printf("DEBUG: Connected pipe: %s | %s\n",
                    current->left->args[0], current->right->args[0]);
                break;  // Important: we set our root and can stop looking
            }
        }
        current = current->next;
    }
    
    // If no pipe was found, use first command as root
    if (!root && cmd_count > 0)
    {
        root = cmd_nodes[0];
        printf("DEBUG: Setting root to first command: %s\n", 
            root->args ? root->args[0] : "null");
    }
    
    if (root)
        printf("DEBUG: Built AST successfully\n");
    else
        printf("DEBUG: Failed to build AST - no root node\n");
        
    return root;
}

/*
Function to initialize a new node.
Uses e_tokentype and s_node struct to create a new node.
Sets default values for all pointers (NULL).
Returns the new node or NULL on allocation failure.
Works with build_ast().
*/
/*
t_node	*initnode(t_tokentype type, char *token)
{
	t_node	*node;
	char	*default_token;
	
	default_token = NULL;
	if (!token)
		return (NULL);
	node = malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->args = malloc(sizeof(char *) * 2);
	if (!node->args)
	{
		ft_safefree((void **)&node);
		return (NULL);
	}
	node->args[0] = ft_strdup(token);
	node->args[1] = NULL;
	node->next = NULL;
	node->prev = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}
*/
/*
Creates a new node with specified type and token.
If token is NULL:
- Uses appropriate default tokens based on type
- For TYPE_HEAD, creates an empty head node
Returns the initialized node or NULL on failure.
*/
/*
t_node *initnode(t_tokentype type, char *token)
{
    t_node *node;
    
    node = malloc(sizeof(t_node));
    if (!node)
        return (NULL);
    node->type = type;
    node->next = NULL;
    node->prev = NULL;
    node->left = NULL;
    node->right = NULL;
    if (type == TYPE_HEAD)
    {
        node->args = NULL;
        return (node);
    }
    if (!token)
    {
        if (type == TYPE_PIPE)
            token = "|";
        else if (type == TYPE_IN_REDIRECT)
            token = "<";
        else if (type == TYPE_OUT_REDIRECT)
            token = ">";
        else if (type == TYPE_APPEND_REDIRECT)
            token = ">>";
        else if (type == TYPE_HEREDOC)
            token = "<<";
        else
            token = "";
    }
    create_args_array(node, token);
    if (!node->args)
    {
        ft_safefree((void **)&node);
        return (NULL);
    }
    return (node);
}
*/

/*
DEBUGGING FUNCTION. REMOVE BEFORE SUBMISSION.
Function to print the token list.
Prints the type and arguments of each token.
Works with lexerlist().
*/
void print_token_list(t_vars *vars)
{
	t_node	*current;
	int		count;
	
	count = 0;
	if (!vars)
	{
		printf("DEBUG: vars is NULL!\n");
		return ;
	}
	printf("DEBUG: head=%p, current=%p\n", 
		   (void*)vars->head, (void*)vars->current);
	if (!vars->head)
	{
		printf("DEBUG: No tokens (vars->head is NULL)!\n");
		return ;
	}
	current = vars->head;
	printf("\n=== TOKEN LIST ===\n");
	while (current)
	{
		printf("Token %d: Type=%d (%s), ", count++, current->type, 
               get_token_str(current->type));
		if (current->args && current->args[0])
		{
			printf("Value='%s', Args=[", current->args[0]);
			int i = 0;
			while (current->args[i])
			{
				printf("'%s'", current->args[i]);
				if (current->args[i+1])
					printf(", ");
				i++;
			}
			printf("]\n");
		}
		else
		{
			printf("No args or empty args\n");
		}
		current = current->next;
	}
	printf("=== END TOKEN LIST ===\n\n");
}
