# Debug System Documentation

## Overview
The debug system has been refactored to allow direct function calls without requiring DEBUG flags or the deprecated `setup_debug_flags()` system.

## Available Debug Functions

### Core Debug Functions (in debug.c)
- `print_node_content(FILE *fp, t_node *node)` - Prints node content to file
- `print_ast_node(FILE *fp, t_node *node, int indent_level)` - Recursive AST printing
- `print_ast(t_node *root, const char *filename)` - Print complete AST (NULL for stdout)
- `print_token_list(t_node *head, const char *filename)` - Print token list (NULL for stdout)
- `print_node_debug(t_node *node, const char *prefix, const char *location)` - Detailed node info to stderr
- `print_node_args(t_node *node, const char *prefix)` - Print node arguments to stderr
- `print_node_linked_list(t_node *head, const char *prefix)` - Print linked list to stderr
- `print_ast_detailed(t_node *root, const char *prefix)` - Detailed AST analysis to stderr

### Convenience Functions (in debug.c)
- `debug_print_ast(t_node *root)` - Quick AST print to stdout
- `debug_print_tokens(t_node *head)` - Quick token list print to stdout
- `debug_analyze_ast(t_node *root)` - Detailed AST analysis to stderr
- `debug_analyze_list(t_node *head)` - Linked list analysis to stderr
- `debug_print_node(t_node *node, const char *context)` - Single node debug to stderr
- `debug_save_ast(t_node *root, const char *filename)` - Save AST to file
- `debug_save_tokens(t_node *head, const char *filename)` - Save tokens to file

## Usage Examples

### Basic Usage
```c
// Include the header
#include "includes/minishell.h"

// In your function
void some_function(t_vars *vars) {
    // Print AST to stdout
    debug_print_ast(vars->astroot);
    
    // Print tokens to stdout  
    debug_print_tokens(vars->head);
    
    // Debug a single node
    debug_print_node(current_node, "parsing_stage");
}
```

### Advanced Analysis
```c
// Detailed analysis (output to stderr)
debug_analyze_ast(vars->astroot);
debug_analyze_list(vars->head);

// Save to files for later inspection
debug_save_ast(vars->astroot, "debug_ast.txt");
debug_save_tokens(vars->head, "debug_tokens.txt");
```

### Integration Points
You can add debug calls at key points in your minishell:

1. **After tokenization:**
   ```c
   debug_print_tokens(vars->head);
   ```

2. **After AST construction:**
   ```c
   debug_print_ast(vars->astroot);
   ```

3. **During node processing:**
   ```c
   debug_print_node(current_node, "processing");
   ```

4. **For detailed analysis:**
   ```c
   debug_analyze_ast(vars->astroot);
   ```

## Migration from Old System

### Before (with DEBUG flags):
```c
if (setup_debug_flags("DEBUG-AST")) {
    // Debug code that was conditional
}
```

### After (direct calls):
```c
// Direct call - no flags needed
debug_print_ast(vars->astroot);
```

## Files Modified
- `srcs/debug.c` - Added convenience wrapper functions
- `includes/minishell.h` - Added function declarations
- All other source files - Removed DEBUG print statements

## Benefits
1. **Simpler usage** - No need to manage DEBUG flags
2. **Always available** - Debug functions can be called anytime
3. **Flexible output** - Can output to stdout, stderr, or files
4. **Better organization** - Clear separation between quick debug and detailed analysis
5. **Easy integration** - Just include header and call functions

## Example Integration
See `debug_example.c` for a complete demonstration of how to use all debug functions.
