/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/04/06 13:13:01 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Initializes the first pipe node in a pipes structure.
- Sets left child to previous command node.
- Sets right child to next command node.
- Both commands must be valid for pipe to be properly configured.
Works with proc_pipes_pt1().
*/
void	setup_first_pipe(t_node *pipe_node, t_node *prev_cmd, t_node *next_cmd)
{
	if (!pipe_node || !prev_cmd || !next_cmd)
		return ;
	pipe_node->left = prev_cmd;
	pipe_node->right = next_cmd;
}

/*
Connects additional pipe nodes in a pipes chain.
- Sets pipe_node's left child to last_cmd.
- Sets pipe_node's right child to next_cmd.
- Links the current pipe to the previous pipe chain.
- Updates the pipes structure for continued processing.
Works with proc_pipes_pt2().
*/
void	setup_next_pipes(t_node *pipe_node, t_node *last_pipe,
	t_node *last_cmd, t_node *next_cmd)
{
	if (!pipe_node || !last_pipe || !last_cmd || !next_cmd)
		return ;
	pipe_node->left = last_cmd;
	pipe_node->right = next_cmd;
	last_pipe->right = pipe_node;
}

/*
Processes the first pipe node in the token list.
- Identifies the first pipe token and its surrounding commands.
- Initializes the pipe node with left and right command references.
- Updates tracking pointers for subsequent pipe processing.
Returns:
- Pointer to root pipe node if found and initialized.
- NULL if no valid pipe configuration found.
Works with proc_token_list().
*/
// t_node	*proc_pipes_pt1(t_vars *vars, t_node **last_pipe, t_node **last_cmd)
// {
// 	t_node	*current;
// 	t_node	*pipe_root;
// 	t_node	*next_cmd;

// 	current = vars->head;
// 	pipe_root = NULL;
// 	*last_pipe = NULL;
// 	*last_cmd = NULL;
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 			*last_cmd = current;
// 		else if (current->type == TYPE_PIPE && !pipe_root
// 			&& current->prev && current->prev->type == TYPE_CMD)
// 		{
// 			pipe_root = current;
// 			next_cmd = find_cmd(current->next, NULL, FIND_NEXT, NULL);
// 			if (next_cmd)
// 			{
// 				setup_first_pipe(pipe_root, current->prev, next_cmd);
// 			}
// 			*last_pipe = pipe_root;
// 			*last_cmd = next_cmd;
// 		}
// 		current = current->next;
// 	}
// 	return (pipe_root);
// }
/*
Processes the first pipe node in the token list.
- Identifies the first pipe token and its surrounding commands.
- Initializes the pipe node with left and right command references.
- Updates tracking in pipes structure for subsequent pipe processing.
Returns:
- Pointer to root pipe node if found and initialized.
- NULL if no valid pipe configuration found.
Works with proc_token_list().
*/
t_node *proc_pipes_pt1(t_vars *vars)
{
    t_node *current;
    t_node *next_cmd;

    if (!vars || !vars->head || !vars->pipes)
        return (NULL);
    // Initialize pipe tracking
    current = vars->head;
    vars->pipes->last_pipe = NULL;
    vars->pipes->last_cmd = NULL;
    vars->pipes->pipe_root = NULL;
    while (current)
    {
        if (current->type == TYPE_CMD)
        {
            vars->pipes->last_cmd = current;
        }
        else if (current->type == TYPE_PIPE)
        {
            // Find the command after the pipe
            next_cmd = find_cmd(current->next, NULL, FIND_NEXT, vars);
            
            // If we have commands before and after the pipe, set up the pipe
            if (vars->pipes->last_cmd && next_cmd && !vars->pipes->pipe_root)
            {
                vars->pipes->pipe_root = current;
                vars->pipes->last_pipe = current;
                setup_first_pipe(current, vars->pipes->last_cmd, next_cmd);
                break ; // Found first pipe, done
            }
        }
        current = current->next;
    }
    return (vars->pipes->pipe_root);
}

/*
Processes additional pipe nodes after the first one.
- Traverses token list to find remaining pipe tokens.
- Connects each pipe to the growing pipes structure.
- Updates tracking pointers for each new pipe addition.
- Maintains the pipes chain for AST construction.
Works with proc_token_list().
*/
// void	proc_pipes_pt2(t_vars *vars, t_node *pipe_root,
// 	t_node **last_pipe, t_node **last_cmd)
// {
// 	t_node	*current;
// 	t_node	*next_cmd;

// 	current = vars->head;
// 	while (current)
// 	{
// 		if (current->type == TYPE_PIPE && pipe_root
// 			&& current != pipe_root)
// 		{
// 			next_cmd = find_cmd(current->next, NULL, FIND_NEXT, NULL);
// 			if (*last_cmd && next_cmd)
// 			{
// 				setup_next_pipes(current, *last_pipe,
// 					*last_cmd, next_cmd);
// 				*last_pipe = current;
// 				*last_cmd = next_cmd;
// 			}
// 		}
// 		current = current->next;
// 	}
// }
/*
Processes additional pipe nodes after the first one.
- Traverses token list to find remaining pipe tokens.
- Connects each pipe to the growing pipes structure.
- Updates tracking in pipes structure for each new pipe addition.
Works with proc_token_list().
*/
void proc_pipes_pt2(t_vars *vars)
{
    t_node *current;
    t_node *next_cmd;

    if (!vars || !vars->head || !vars->pipes || !vars->pipes->pipe_root
		|| !vars->pipes->last_pipe)
        return ;
    current = vars->head;
    while (current && current != vars->pipes->last_pipe)
        current = current->next;
    if (!current || !current->next)
        return ;
    current = current->next;
    while (current)
    {
        if (current->type == TYPE_CMD)
        {
            vars->pipes->last_cmd = current;
        }
        else if (current->type == TYPE_PIPE)
        {
            next_cmd = find_cmd(current->next, NULL, FIND_NEXT, vars);
            if (vars->pipes->last_cmd && next_cmd)
            {
                setup_next_pipes(current, vars->pipes->last_pipe, 
                                vars->pipes->last_cmd, next_cmd);
                vars->pipes->last_pipe = current;
            }
        }
        current = current->next;
    }
}

/*
Configures a redirection node with source and target commands.
- Sets left child to source command node.
- Sets right child to target command/filename node.
- Establishes the redirection relationship in the AST.
Works with proc_redir_pt1() and proc_redir_pt2().
*/
void	setup_redir_ast(t_node *redir, t_node *cmd, t_node *target)
{
	if (!redir || !cmd || !target)
		return ;
	redir->left = cmd;
	redir->right = target;
}

/*
Updates pipe structure when commands are redirected.
- Traverses pipe chain looking for references to the command.
- Replaces command references with redirection node references.
- Preserves pipe structure while incorporating redirections.
- Handles both left and right side command replacements.
Works with proc_redir_pt2().
*/
void	upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir)
{
	t_node	*pipe_node;

	if (!pipe_root || !cmd || !redir)
		return ;
	pipe_node = pipe_root;
	while (pipe_node)
	{
		if (pipe_node->left == cmd)
			pipe_node->left = redir;
		else if (pipe_node->right == cmd)
			pipe_node->right = redir;
		pipe_node = pipe_node->right;
		if (pipe_node && pipe_node->type != TYPE_PIPE)
			break ;
	}
}

/*
Determines if a redirection node has valid adjacent commands.
- Checks if next node exists and is a command.
Returns:
- 1 if redirection has valid syntax.
- 0 otherwise.
Works with proc_redir_pt1().
*/
int	is_valid_redir_node(t_node *current)
{
	if (!current)
		return (0);
	if (!is_redirection(current->type))
		return (0);
	 // Modified to accept ARGS type for redirection targets
	if (!current->next || (current->next->type != TYPE_CMD && current->next->type != TYPE_ARGS))
		return (0);
	return (1);
}

/*
Gets target command for redirection.
- Uses previous command if it exists and is a command node.
- Otherwise uses last tracked command.
Returns:
- Pointer to target command node.
- NULL if no suitable command found.
Works with proc_redir_pt1().
*/
// t_node	*get_redir_target(t_node *current, t_node *last_cmd)
// {
// 	t_node	*target;

// 	target = NULL;
// 	if (current->prev && current->prev->type == TYPE_CMD)
// 		target = current->prev;
// 	else
// 		target = last_cmd;
// 	return (target);
// }
t_node *get_redir_target(t_node *current, t_node *last_cmd)
{
    t_node *target;

    target = NULL;
    if (current->prev && current->prev->type == TYPE_CMD)
        target = current->prev;
    else
        target = last_cmd;
        
    // Make sure we have a valid target
    if (!target)
        DBG_PRINTF(DEBUG_EXEC, "No valid redirection target found\n");
    
    return (target);
}

/*
Processes the first part of redirection nodes identification.
- Traverses token list to find redirection operators.
- Records last command seen for reference.
- Identifies redirection targets.
Returns:
- First valid redirection node if no pipe root exists.
- NULL otherwise or if no valid redirections found.
Works with proc_token_list().
*/
// t_node	*proc_redir_pt1(t_vars *vars, t_node *pipe_root)
// {
// 	t_node	*current;
// 	t_node	*last_cmd;
// 	t_node	*redir_root;
// 	t_node	*target_cmd;

// 	current = vars->head;
// 	last_cmd = NULL;
// 	redir_root = NULL;
// 	(void)pipe_root;
	
// 	DBG_PRINTF(DEBUG_EXEC, "Starting redirection processing (proc_redir_pt1)\n");
	
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 		{
// 			last_cmd = current;
// 			DBG_PRINTF(DEBUG_EXEC, "Found command node: %s\n", 
// 					  current->args ? current->args[0] : "NULL");
// 		}
// 		else if (is_valid_redir_node(current))
// 		{
// 			DBG_PRINTF(DEBUG_EXEC, "Found valid redirection node type=%d (%s)\n", 
// 					  current->type, get_token_str(current->type));
			
// 			target_cmd = get_redir_target(current, last_cmd);
			
// 			DBG_PRINTF(DEBUG_EXEC, "Target command for redirection: %p\n", (void*)target_cmd);
// 			if (target_cmd && target_cmd->args)
// 				DBG_PRINTF(DEBUG_EXEC, "Target command content: %s\n", target_cmd->args[0]);
			
// 			if (target_cmd)
// 			{
// 				DBG_PRINTF(DEBUG_EXEC, "Setting up redirection AST: %s %s %s\n",
// 						  target_cmd->args ? target_cmd->args[0] : "NULL",
// 						  get_token_str(current->type),
// 						  current->next && current->next->args ? current->next->args[0] : "NULL");
				
// 				setup_redir_ast(current, target_cmd, current->next);
// 				if (!redir_root)
// 				{
// 					redir_root = current;
// 					DBG_PRINTF(DEBUG_EXEC, "Set redir_root to node %p\n", (void*)redir_root);
// 				}
// 			}
// 		}
// 		current = current->next;
// 	}
	
// 	DBG_PRINTF(DEBUG_EXEC, "Completed redirection processing, returning root: %p\n", (void*)redir_root);
	
// 	return (redir_root);
// }
/*
Processes the first part of redirection nodes identification.
- Traverses token list to find redirection operators.
- Records last command seen in vars->pipes structure.
- Identifies redirection targets.
Returns:
- First valid redirection node if found.
- NULL if no valid redirections found or on error.
Works with proc_token_list().
*/
t_node *proc_redir_pt1(t_vars *vars)
{
    t_node *current;

    if (!vars || !vars->head || !vars->pipes)
        return (NULL);
    current = vars->head;
    vars->pipes->last_cmd = NULL;
    vars->pipes->redir_root = NULL;
    DBG_PRINTF(DEBUG_EXEC, "Starting redirection processing (proc_redir_pt1)\n");
    while (current)
    {
        if (current->type == TYPE_CMD)
        {
            vars->pipes->last_cmd = current;
            DBG_PRINTF(DEBUG_EXEC, "Found command node: %s\n", 
                      current->args ? current->args[0] : "NULL");
        }
        else if (is_valid_redir_node(current))
        {
            DBG_PRINTF(DEBUG_EXEC, "Found valid redirection node type=%d (%s)\n", 
                      current->type, get_token_str(current->type));
                      
            vars->pipes->cmd_redir = get_redir_target(current, vars->pipes->last_cmd);
            DBG_PRINTF(DEBUG_EXEC, "Target command for redirection: %p\n", (void*)vars->pipes->cmd_redir);
            if (vars->pipes->cmd_redir && vars->pipes->cmd_redir->args)
                DBG_PRINTF(DEBUG_EXEC, "Target command content: %s\n", vars->pipes->cmd_redir->args[0]);
            if (vars->pipes->cmd_redir)
            {
                DBG_PRINTF(DEBUG_EXEC, "Setting up redirection AST: %s %s %s\n",
                          vars->pipes->cmd_redir->args ? vars->pipes->cmd_redir->args[0] : "NULL",
                          get_token_str(current->type),
                          current->next && current->next->args ? current->next->args[0] : "NULL");
                
                setup_redir_ast(current, vars->pipes->cmd_redir, current->next);
                if (!vars->pipes->redir_root)
                {
                    vars->pipes->redir_root = current;
                    DBG_PRINTF(DEBUG_EXEC, "Set redir_root to node %p\n", (void*)vars->pipes->redir_root);
                }
            }
        }
        current = current->next;
    }
    DBG_PRINTF(DEBUG_EXEC, "Completed redirection processing, returning root: %p\n", 
              (void*)vars->pipes->redir_root);
    
    return (vars->pipes->redir_root);
}

/*
Processes the second part of redirection nodes handling.
- Updates pipe structure with redirection references.
- Links redirections into existing pipe hierarchy.
- Only modifies pipe structure if redirection affects piped commands.
Works with proc_token_list().
*/
// void	proc_redir_pt2(t_vars *vars, t_node *pipe_root)
// {
// 	t_node	*current;
// 	t_node	*target_cmd;
// 	t_node	*last_cmd;

// 	current = vars->head;
// 	last_cmd = NULL;
// 	while (current)
// 	{
// 		if (current->type == TYPE_CMD)
// 			last_cmd = current;
// 		else if (is_valid_redir_node(current))
// 		{
// 			target_cmd = get_redir_target(current, last_cmd);
// 			if (target_cmd && pipe_root)
// 				upd_pipe_redir(pipe_root, target_cmd, current);
// 		}
// 		current = current->next;
// 	}
// }
/*
Processes the second part of redirection nodes handling.
- Updates pipe structure with redirection references.
- Links redirections into existing pipe hierarchy.
- Only modifies pipe structure if redirection affects piped commands.
Works with proc_token_list().
*/
void proc_redir_pt2(t_vars *vars)
{
    t_node *current;
    t_node *target_cmd;  // Need to keep this as local temporary variable

    if (!vars || !vars->head || !vars->pipes || !vars->pipes->pipe_root)
        return;
        
    current = vars->head;
    vars->pipes->last_cmd = NULL;
    
    while (current)
    {
        if (current->type == TYPE_CMD)
            vars->pipes->last_cmd = current;
        else if (is_valid_redir_node(current))
        {
            // Find target command for this redirection
            target_cmd = get_redir_target(current, vars->pipes->last_cmd);
            
            // Update pipe structure to include this redirection
            if (target_cmd && vars->pipes->pipe_root)
                upd_pipe_redir(vars->pipes->pipe_root, target_cmd, current);
        }
        current = current->next;
    }
}

/*
Processes the entire token list to build the AST structure.
- Identifies command nodes for reference.
- Processes pipe tokens to build pipes structure.
- Processes redirection tokens and integrates with pipe structure.
- Determines appropriate root node for the final AST.
Returns:
- Pointer to root node of the constructed AST.
- NULL if invalid syntax or no commands found.

Example: For input "ls -l | grep a > output.txt":
- First identifies command nodes "ls" and "grep"
- Processes pipe to connect them
- Processes redirection to output.txt
- Returns pipe node as root, with redirection integrated
*/
t_node *proc_token_list(t_vars *vars)
{
    // Base case validation
    if (!vars || !vars->head || !vars->pipes)
        return (NULL);
    // Find all command nodes
    find_cmd(NULL, NULL, FIND_ALL, vars);
    // Safety check: make sure we have at least one valid command
    if (vars->cmd_count == 0 || !vars->cmd_nodes[0] || !vars->cmd_nodes[0]->args)
        return (NULL);
    // Initialize for pipe processing
    vars->pipes->pipe_root = NULL;
    vars->pipes->redir_root = NULL;
    // Process pipes - first pass identifies the first pipe
    vars->pipes->pipe_root = proc_pipes_pt1(vars);
    // Process pipes - second pass handles remaining pipes
    if (vars->pipes->pipe_root)
        proc_pipes_pt2(vars);
    // Process redirections - first phase
    DBG_PRINTF(DEBUG_EXEC, "Processing redirections: pipe_root=%p\n", 
              (void*)vars->pipes->pipe_root);  
    proc_redir_pt1(vars);
    DBG_PRINTF(DEBUG_EXEC, "After proc_redir_pt1: redir_root=%p\n", 
              (void*)vars->pipes->redir_root);
    // Process redirections - second phase if pipes exist
    if (vars->pipes->pipe_root)
        proc_redir_pt2(vars);
    DBG_PRINTF(DEBUG_EXEC, "After proc_redir_pt2: redir_root=%p\n", 
              (void*)vars->pipes->redir_root);
    // Return the appropriate root node
    DBG_PRINTF(DEBUG_EXEC, "Final AST root selection: pipe_root=%p, redir_root=%p\n", 
              (void*)vars->pipes->pipe_root, (void*)vars->pipes->redir_root);
    if (vars->pipes->pipe_root)
        return vars->pipes->pipe_root;
    else if (vars->pipes->redir_root)
        return vars->pipes->redir_root;
    else if (vars->cmd_count > 0)
        return vars->cmd_nodes[0];
    return NULL;
}

/*
Converts string tokens after pipes to command tokens.
*/
void	convert_strs_to_cmds(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current && current->next)
	{
		if (current->type == TYPE_PIPE && current->next->type == TYPE_ARGS)
			current->next->type = TYPE_CMD;
		current = current->next;
	}
}

/*
Attaches string tokens as arguments to their commands.
*/
void	link_strargs_to_cmds(t_vars *vars)
{
	t_node	*current;
	t_node	*cmd_node;

	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_CMD)
			cmd_node = current;
		else if (cmd_node && current->type == TYPE_ARGS)
		{
			// Append the argument to the command node
			if (cmd_node->args && current->args && current->args[0])
			{
				cmd_node->args[0] = ft_strdup(current->args[0]);
			}
		}
		current = current->next;
	}
}

/*
Connects an additional pipe in the pipe chain.
*/
void	link_addon_pipe(t_node *last_pipe, t_node *new_pipe, t_node *right_cmd)
{
	t_node	*prev_right;

	prev_right = last_pipe->right;
	last_pipe->right = new_pipe;
	new_pipe->left = prev_right;
	new_pipe->right = right_cmd;
}

/*
Creates a tree structure for pipe commands.
*/
void	build_pipe_ast(t_vars *vars)
{
	t_node	*current;
	t_node	*last_pipe;
	t_node	*cmd_before;
	t_node	*cmd_after;

	last_pipe = NULL;
	cmd_before = NULL;
	current = vars->head;
	vars->astroot = NULL;
	while (current)
	{
		if (current->type == TYPE_CMD)
			cmd_before = current;
		else if (current->type == TYPE_PIPE)
		{
			cmd_after = find_cmd(current->next, NULL, FIND_NEXT, NULL);
			if (!vars->astroot)
			{
				vars->astroot = current;
				if (cmd_before)
					vars->astroot->left = cmd_before;
				if (cmd_after)
					vars->astroot->right = cmd_after;
				last_pipe = vars->astroot;
			}
			else if (last_pipe)
			{
				link_addon_pipe(last_pipe, current, cmd_after);
				last_pipe = current;
			}
		}
		current = current->next;
	}
	if (!vars->astroot)
	{
		if (vars->head && vars->head->type == TYPE_CMD)
			vars->astroot = vars->head;
	}
}


/*
Detects if a pipe token appears at the beginning of input.
- Checks if the first token is a pipe (syntax error).
- Sets error code and outputs error message if detected.
Returns:
- 1 if pipe found at beginning (error condition).
- 0 if no error detected or no tokens exist.
Works with chk_pipe_syntax_err().
*/
int	chk_start_pipe(t_vars *vars)
{
	if (!vars || !vars->head)
		return (0);
	if (vars->head->type == TYPE_PIPE)
	{
		ft_putstr_fd("bleshell: unexpected syntax error at '|'\n", 2);
		vars->error_code = 258;
		return (1);
	}
	return (0);
}

/*
Detects multiple pipe tokens in sequence.
- Tracks count of consecutive pipe tokens.
- Reports syntax error if multiple pipes are detected.
- Sets error code and outputs error message.
Returns:
- 1 if consecutive pipes detected (error).
- 0 if no consecutive pipes.
Works with chk_next_pipes().
*/
int	chk_multi_pipes(t_vars *vars, int pipes_count)
{
	if (pipes_count > 1)
	{
		ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
		vars->error_code = 258;
		return (1);
	}
	return (0);
}

/*
Detects adjacent pipe tokens in the list.
- Checks if current and next token are both pipes.
- Reports syntax error if adjacent pipes are detected.
- Sets error code and outputs error message.
Returns:
- 1 if adjacent pipes detected (error).
- 0 if no adjacent pipes.
Works with chk_next_pipes().
*/
int	chk_adj_pipes(t_vars *vars, t_node *current)
{
	if (current->next && current->next->type == TYPE_PIPE)
	{
		ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
		vars->error_code = 258;
		return (1);
	}
	return (0);
}

/*
Checks for consecutive pipe tokens in the token list.
- Tracks sequence of pipe tokens.
- Reports syntax error if multiple pipes found consecutively.
Returns:
- 1 if pipe syntax error found.
- 0 if no errors detected.
Works with chk_pipe_syntax_err().
*/
int	chk_next_pipes(t_vars *vars)
{
	t_node	*current;
	int		multi_pipes;
	int		error;

	current = vars->head;
	multi_pipes = 0;
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			multi_pipes++;
			error = chk_multi_pipes(vars, multi_pipes);
			if (error)
				return (error);
			error = chk_adj_pipes(vars, current);
			if (error)
				return (error);
		}
		else
			multi_pipes = 0;
		current = current->next;
	}
	return (0);
}

/*
Detects if a pipe token appears at the end of input.
- Checks if the last token is a pipe.
- Indicates more input is needed to complete command.
Returns:
- 2 if pipe found at end (requires more input).
- 0 if no pipe at end or no tokens exist.
Works with chk_pipe_syntax_err().
*/
int	chk_end_pipe(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current && current->next)
		current = current->next;
	if (current && current->type == TYPE_PIPE)
		return (2);
	return (0);
}

/*
Performs comprehensive check of pipe syntax in token list.
- Detects pipes at beginning (error).
- Detects consecutive pipes (error).
- Detects pipes at end (needs more input).
Returns:
- 0 if pipe syntax is valid.
- 1 if syntax error detected.
- 2 if more input needed (pipe at end).

Example: For input "| ls":
- Returns 1 (error: pipe at beginning)
For input "ls | grep a |":
- Returns 2 (needs more input)
*/
int	chk_pipe_syntax_err(t_vars *vars)
{
	int	result;

	if (!vars->head)
		return (0);
	result = chk_start_pipe(vars);
	if (result != 0)
		return (result);
	result = chk_next_pipes(vars);
	if (result != 0)
		return (result);
	result = chk_end_pipe(vars);
	if (result != 0)
		return (result);
	return (0);
}
