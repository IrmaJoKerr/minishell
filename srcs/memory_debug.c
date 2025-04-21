/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_debug.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 01:31:11 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 01:17:47 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

#ifdef DEBUG_MEMORY
static int g_malloc_count = 0;
static int g_free_count = 0;

void ensure_token_cleanup(t_node *node_chain)
{
    t_node *current;
    t_node *next;
    
    if (!node_chain)
        return;
        
    fprintf(stderr, "[DEBUG] ensure_token_cleanup: Cleaning orphaned token chain\n");
    current = node_chain;
    
    while (current)
    {
        next = current->next;
        free_token_node(current);
        current = next;
    }
}

// Track memory allocation
// Track memory allocation
void *tracked_malloc(size_t size, const char *func, int line)
{
    void *ptr = malloc(size);
    if (ptr)
    {
        g_malloc_count++;
        fprintf(stderr, "[MEMTRACK:%s:%d] Malloc #%d: %p (%zu bytes)\n", 
                func, line, g_malloc_count, ptr, size);
    }
    return ptr;
}

void register_external_free(void *ptr, const char *func, int line)
{
    if (!ptr)
        return;
        
    // Increment the free count in your tracking system
    g_free_count++;
    fprintf(stderr, "[MEMTRACK:%s:%d] External Free #%d: %p\n", 
            func, line, g_free_count, ptr);
}

// Track memory freeing
void tracked_free(void *ptr, const char *func, int line)
{
    if (ptr)
    {
        g_free_count++;
        fprintf(stderr, "[MEMTRACK:%s:%d] Free #%d: %p\n", 
                func, line, g_free_count, ptr);
        free(ptr);
    }
}

// Print memory statistics
void print_memory_stats(void)
{
    fprintf(stderr, "\n[MEMTRACK] === Memory Statistics ===\n");
    fprintf(stderr, "[MEMTRACK] Allocations: %d\n", g_malloc_count);
    fprintf(stderr, "[MEMTRACK] Frees: %d\n", g_free_count);
    fprintf(stderr, "[MEMTRACK] Difference: %d\n", g_malloc_count - g_free_count);
    fprintf(stderr, "[MEMTRACK] =========================\n\n");
}
#endif

// Global array to track all open file descriptors
#define MAX_TRACKED_FDS 100
static int g_open_fds[MAX_TRACKED_FDS];
static int g_fd_count = 0;

// Register an opened file descriptor
int register_fd(int fd)
{
    if (fd < 3 || g_fd_count >= MAX_TRACKED_FDS)
        return fd;
        
    g_open_fds[g_fd_count++] = fd;
    fprintf(stderr, "[DEBUG] register_fd: Added fd %d to tracking (total: %d)\n", 
            fd, g_fd_count);
    return fd;
}

// Safe open wrapper that registers the fd
int safe_open(const char *path, int flags, mode_t mode)
{
    int fd = open(path, flags, mode);
    if (fd >= 3)  // Don't track stdin/stdout/stderr
        register_fd(fd);
    return fd;
}

// Close a tracked file descriptor
// Close a tracked file descriptor
int safe_close(int fd)
{
    int i, result;
    
    if (fd < 3)
        return 0;  // Don't close stdin/stdout/stderr
    
    result = close(fd);
    
    // Remove from tracked array
    for (i = 0; i < g_fd_count; i++)
    {
        if (g_open_fds[i] == fd)
        {
            // Shift remaining elements
            while (i < g_fd_count - 1)
            {
                g_open_fds[i] = g_open_fds[i + 1];
                i++;
            }
            g_fd_count--;
            fprintf(stderr, "[DEBUG] safe_close: Removed fd %d from tracking (total: %d)\n", 
                    fd, g_fd_count);
            break;
        }
    }
    
    return result;
}

// Close all tracked file descriptors (call during cleanup)
void close_all_tracked_fds(void)
{
    int i;
    
    fprintf(stderr, "[DEBUG] close_all_tracked_fds: Closing %d file descriptors\n", g_fd_count);
    
    for (i = 0; i < g_fd_count; i++)
    {
        fprintf(stderr, "[DEBUG] close_all_tracked_fds: Closing fd %d\n", g_open_fds[i]);
        close(g_open_fds[i]);
    }
    
    g_fd_count = 0;
}

#include "../includes/minishell.h"

// Safely allocate pipe file descriptors array
int *alloc_pipe_fds(int pipe_count)
{
    int *pipe_fds = NULL;
    int i, created = 0;
    
    // Allocate memory for pipe file descriptors
    pipe_fds = malloc(sizeof(int) * pipe_count * 2);
    if (!pipe_fds)
        return NULL;
        
    // Create the pipes
    for (i = 0; i < pipe_count; i++)
    {
        if (pipe(&pipe_fds[i * 2]) == -1)
        {
            // Close already created pipes on error
            while (created > 0)
            {
                created--;
                close(pipe_fds[created * 2]);
                close(pipe_fds[created * 2 + 1]);
            }
            free(pipe_fds);
            return NULL;
        }
        created++;
    }
    
    return pipe_fds;
}

// Safely close all pipe file descriptors
void close_all_pipe_fds(int *pipe_fds, int pipe_count)
{
    int i;
    
    if (!pipe_fds)
        return;
        
    for (i = 0; i < pipe_count * 2; i++)
    {
        if (pipe_fds[i] > 2)
        {
            close(pipe_fds[i]);
        }
    }
    
    free(pipe_fds);
}

void final_cleanup(t_vars *vars)
{
    // Clean standard resources
    cleanup_exit(vars);
    
    // Close any tracked file descriptors that might have been leaked
    close_all_tracked_fds();
    
    // Print memory statistics if enabled
#ifdef DEBUG_MEMORY
    print_memory_stats();
#endif
}

// Add to check if a pointer is still valid
void verify_memory(void *ptr, const char *name, const char *func, int line)
{
    fprintf(stderr, "[MEMVERIFY:%s:%d] Checking pointer %s=%p\n", 
            func, line, name, ptr);
    
    // Try to read from the pointer (risky but can help detect if memory is invalid)
    if (ptr)
    {
        char tmp;
        memcpy(&tmp, ptr, 1);  // Just read 1 byte to check if accessible
        fprintf(stderr, "[MEMVERIFY:%s:%d] Memory at %s=%p is readable\n", 
                func, line, name, ptr);
    }
    else
    {
        fprintf(stderr, "[MEMVERIFY:%s:%d] %s is NULL\n", 
                func, line, name);
    }
}



// Add this enhanced verification function
void verify_memory_range(void *ptr, size_t size, const char *name, 
	const char *func, int line)
{
	if (!ptr)
	{
		fprintf(stderr, "[MEMVERIFY:%s:%d] %s is NULL\n", 
			func, line, name);
		return;
	}

	fprintf(stderr, "[MEMVERIFY:%s:%d] Checking memory range %s=%p (size=%zu)\n", 
			func, line, name, ptr, size);

	// Try to validate the memory by reading the first and last byte
	char *start = (char *)ptr;
	char *end = start + size - 1;

	// Read first byte
	char tmp;
	memcpy(&tmp, start, 1);

	// Read last byte if size > 1
	if (size > 1)
	{
		memcpy(&tmp, end, 1);
	}

	fprintf(stderr, "[MEMVERIFY:%s:%d] Memory range %s=%p to %p is valid\n", 
		func, line, name, start, end);
}

// Add this after the verify_memory_range function

// Register memory allocations with type information
void register_allocation(void *ptr, const char *type, const char *caller_func)
{
    if (!ptr)
        return ;
    fprintf(stderr, "[MEMTRACK:%s] Allocation: %p (%s)\n", 
            caller_func, ptr, type);
    // Optionally increment allocation counter if you want to track it in statistics
    #ifdef DEBUG_MEMORY
    g_malloc_count++;  // Comment this out if you don't want to count these as new allocations
    #endif
}

// Use this when handling critical memory operations:
// verify_memory_range(new_quote_types[0], sizeof(int) * total_chars, 
//                    "quote_types buffer", __func__, __LINE__);
void verify_token_list_integrity(t_vars *vars)
{
    t_node *forward;
    t_node *backward;
    int forward_count = 0;
    int backward_count = 0;
    
    if (!vars || !vars->head) // Changed from token_list to head
    {
        fprintf(stderr, "[INTEGRITY] No token list to verify\n");
        return ;
    }
    fprintf(stderr, "[INTEGRITY] Verifying token list integrity\n");
    // Forward traversal
    forward = vars->head; // Changed from token_list to head
    while (forward)
    {
        forward_count++;
        // Check for broken links
        if (forward->next && forward->next->prev != forward)
        {
            fprintf(stderr, "[INTEGRITY ERROR] Broken link: node %p -> next %p, but next->prev = %p\n", 
                    forward, forward->next, forward->next->prev);
        }
        forward = forward->next;
    }
    // Backward traversal
    backward = vars->current; // Changed from token_list_end to current
    while (backward)
    {
        backward_count++;
        // Check for broken links
        if (backward->prev && backward->prev->next != backward)
        {
            fprintf(stderr, "[INTEGRITY ERROR] Broken link: node %p -> prev %p, but prev->next = %p\n", 
                    backward, backward->prev, backward->prev->next);
        }
        backward = backward->prev;
    }
    fprintf(stderr, "[INTEGRITY] Forward count: %d, Backward count: %d\n", 
            forward_count, backward_count);
    if (forward_count != backward_count)
    {
        fprintf(stderr, "[INTEGRITY ERROR] List count mismatch: forward=%d, backward=%d\n", 
                forward_count, backward_count);
    }
    else
    {
        fprintf(stderr, "[INTEGRITY] Token list is intact with %d nodes\n", forward_count);
    }
}

void track_alloc(void *ptr, const char *func, int line)
{
    fprintf(stderr, "[MEM_TRACK] Allocated %p at %s:%d\n", ptr, func, line);
}

void track_free(void *ptr, const char *func, int line)
{
    fprintf(stderr, "[MEM_TRACK] Freed %p at %s:%d\n", ptr, func, line);
}

void cleanup_expansion_mem(char *token, char *expanded_val) 
{
    if (token)
	{
        fprintf(stderr, "[MEM_DEBUG] cleanup_expansion_mem: Freeing token=%p\n", (void*)token);
        free(token);
    }
    if (expanded_val)
	{
        fprintf(stderr, "[MEM_DEBUG] cleanup_expansion_mem: Freeing expanded_val=%p\n", 
                (void*)expanded_val);
        free(expanded_val);
    }
}
