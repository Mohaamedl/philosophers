# Philosophers - Bonus Part

## Overview

This is the **bonus implementation** of the Dining Philosophers problem using **processes** and **semaphores** instead of threads and mutexes.

### Key Differences from Mandatory Part

| Feature | Mandatory | Bonus |
|---------|-----------|-------|
| **Concurrency Model** | Threads (`pthread_create`) | Processes (`fork()`) |
| **Synchronization** | Mutexes (`pthread_mutex_t`) | Semaphores (`sem_t`) |
| **Fork Representation** | Array of mutexes | Named semaphore with count |
| **Process Architecture** | Single process, multiple threads | Multiple processes, one philosopher each |
| **Death Detection** | Central monitor thread | Monitor thread in each process |
| **Communication** | Shared memory (direct) | Semaphores (system-wide) |

## Architecture

### Process Model

```
Main Process
├── Philosopher 1 (child process)
│   └── Monitor Thread (pthread)
├── Philosopher 2 (child process)
│   └── Monitor Thread (pthread)
├── Philosopher 3 (child process)
│   └── Monitor Thread (pthread)
└── ...
```

Each philosopher runs in its own process with:
- Independent memory space
- Local monitor thread for death detection
- Access to shared semaphores

### Semaphores

Three named semaphores coordinate the dining:

1. **`/forks`** - Binary semaphore array (count = number of philosophers)
   - Each `sem_wait()` takes one fork
   - Each `sem_post()` releases one fork
   - Prevents deadlock through resource availability

2. **`/write`** - Mutex semaphore (count = 1)
   - Protects stdout from interleaved output
   - Ensures atomic message printing

3. **`/dead`** - Death notification (count = 0)
   - Posted by first dying philosopher
   - Main process waits on this to detect death
   - (Not currently used - using exit codes instead)

### Death Detection

Each process contains:
- **Main loop**: take_forks → eat → drop_forks → sleep → think
- **Monitor thread**: Checks every 2ms if philosopher has died
  - If dead: `announce_death()` and `exit(1)`
  - Process exits with status 1 on death

Main process:
- Uses `waitpid(-1, &status, 0)` to wait for any child
- If `WEXITSTATUS(status) == 1`: philosopher died → kill all others
- If all exit with status 0: all philosophers completed their meals

### Meal Completion

Unlike the mandatory part where a central monitor checks all philosophers:

1. Each philosopher checks its own meal count after eating
2. After `eat_action()` and `drop_forks()`:
   ```c
   if (meals_count >= must_eat_count)
       exit(0);
   ```
3. Process exits cleanly with status 0
4. Main process counts exits - when all exit, simulation ends

## Implementation Details

### Files Structure

```
philo_bonus/
├── include/
│   └── philo_bonus.h         # Structures with sem_t* and pid_t
├── src/
│   ├── main_bonus.c          # Entry point, process orchestration
│   ├── parsing.c             # Argument validation (shared with mandatory)
│   ├── time.c                # Time utilities (shared with mandatory)
│   ├── init_bonus.c          # Semaphore initialization (sem_open)
│   ├── cleanup_bonus.c       # Semaphore cleanup (sem_close/unlink)
│   ├── sync_bonus.c          # safe_print, announce_death
│   ├── actions_bonus.c       # take/drop forks, eat, sleep, think
│   ├── process_bonus.c       # fork processes, wait, kill
│   └── monitor_bonus.c       # Death detection thread
└── Makefile
```

### Key Functions

#### Process Management (`process_bonus.c`)
- **`philosopher_process()`** - Main routine for each philosopher process
- **`create_processes()`** - Forks all philosopher processes
- **`wait_processes()`** - Waits for death or completion
- **`kill_all_processes()`** - Terminates all children on death

#### Semaphore Operations (`actions_bonus.c`)
- **`take_forks()`** - Calls `sem_wait()` twice on `/forks`
- **`drop_forks()`** - Calls `sem_post()` twice on `/forks`
- **`eat_action()`** - Updates `last_meal_time` and `meals_count`

#### Monitor (`monitor_bonus.c`)
- **`monitor_routine()`** - Thread checking for death every 2ms
- **`is_philosopher_dead()`** - Compares time since last meal
- **`start_monitor()`** - Creates detached monitor thread

### Race Condition Fix

**Problem**: When a philosopher reaches the meal count limit:
1. `eat_action()` increments `meals_count` to target value
2. Monitor thread checks `meals_count >= must_eat_count` every 2ms
3. Monitor could call `exit(0)` BEFORE `drop_forks()` is called
4. Result: forks remain locked, other philosophers starve

**Solution**: Check meal count in main loop, not in monitor:
```c
while (1) {
    take_forks(philo);
    eat_action(philo);
    drop_forks(philo);         // Always drop before exiting
    if (meals_count >= must_eat_count)
        exit(0);               // Safe to exit now
    sleep_action(philo);
    think_action(philo);
}
```

Monitor thread now only checks for death, not meal completion.

## Testing

### Evaluation Tests

All tests from mandatory part, expecting same behavior:

```bash
# Test 1: Single philosopher (should die at ~800ms)
./philo_bonus 1 800 200 200

# Test 2: 5 philosophers no deaths (run for 5+ seconds)
./philo_bonus 5 800 200 200

# Test 3: Meal count termination
./philo_bonus 2 800 200 200 5

# Test 4: No philosopher should die
./philo_bonus 4 410 200 200

# Test 5: Tight timing
./philo_bonus 4 310 200 100

# Test 6: Two philosophers alternating
./philo_bonus 2 800 200 200
```

### Verification Checklist

- ✅ **No philosopher dies** (unless required by timing)
- ✅ **Death detected within 10ms** (monitor checks every 2ms)
- ✅ **Clean process termination** (all children reaped)
- ✅ **No orphan processes** (`ps aux | grep philo_bonus` should be empty)
- ✅ **No leaked semaphores** (`ls /dev/shm/sem.* | grep philo` should be empty)
- ✅ **Norminette compliant** (100% on all files)

### Memory Leak Check

```bash
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --trace-children=yes \
         ./philo_bonus 4 410 200 200 5
```

Expected: `definitely lost: 0 bytes in 0 blocks`

### Semaphore Cleanup Verification

```bash
# Before run
ls /dev/shm/sem.* 2>/dev/null

# Run program
./philo_bonus 5 800 200 200

# After run - should see no philo semaphores
ls /dev/shm/sem.* 2>/dev/null | grep -E "(forks|write|dead)"
```

### Stress Test

```bash
# 200 philosophers
timeout 10s ./philo_bonus 200 410 200 200

# Check for orphans
ps aux | grep philo_bonus | grep -v grep
# Should output: (empty) or count: 0
```

## Compilation

```bash
make            # Compile bonus
make clean      # Remove object files
make fclean     # Remove objects and executable
make re         # Recompile from scratch
```

### Compilation Flags

```
-Wall -Wextra -Werror
-pthread        # Required for monitor threads
-g              # Debug symbols (for valgrind)
```

## Known Limitations

### Semaphore Portability

Named semaphores (`sem_open`) may have platform-specific behavior:
- **Linux**: Stored in `/dev/shm/sem.*`
- **macOS**: Different implementation
- Maximum name length varies by system

### Process Limit

Creating many processes (>200) may hit system limits:
```bash
ulimit -u    # Check max user processes
```

Adjust if needed:
```bash
ulimit -u 4096
```

## Debugging Tips

### Tracing Process Creation

```bash
strace -f -e fork,clone,execve ./philo_bonus 3 800 200 200
```

### Monitoring Semaphores

```bash
# List all semaphores
ipcs -s

# Watch semaphore files in real-time
watch -n 0.5 'ls -la /dev/shm/sem.*'
```

### Finding Orphan Processes

```bash
ps aux | grep philo_bonus | awk '{print $2}' | xargs kill -9
```

## Comparison with Mandatory

### Advantages of Bonus (Processes)
- ✅ Complete memory isolation between philosophers
- ✅ More realistic simulation (separate entities)
- ✅ Easier debugging (each process in separate debugger)
- ✅ Better crash isolation (one crash doesn't affect others)

### Advantages of Mandatory (Threads)
- ✅ Lower memory overhead (shared address space)
- ✅ Faster context switching
- ✅ Easier shared state management
- ✅ More straightforward death detection (central monitor)

## Performance Notes

- Process creation overhead: ~1-2ms per philosopher
- Monitor thread checks: every 2ms
- Death detection accuracy: <10ms guaranteed
- Maximum philosophers tested: 200 (successful)

## Author

**mhaddadi** - 42 Porto

## Norminette

All files pass norminette 100%:
- ✅ Maximum 25 lines per function
- ✅ Maximum 5 functions per file
- ✅ No forbidden functions
- ✅ Proper header and formatting
