# Philosophers (42)

*"I never thought philosophy would be so deadly"*

Implementation of the classic Dining Philosophers Problem using C with two approaches: threads+mutexes (mandatory) and processes+semaphores (bonus). Focus on preventing deadlocks, data races, and ensuring precise timing for philosopher death detection.

## 🎯 Evaluation Criteria Checklist

### ✅ **MANDATORY REQUIREMENTS**
- [ ] **NO GLOBAL VARIABLES** - All shared resources managed through structures
- [ ] **NO DATA RACES** - Verified with `valgrind --tool=helgrind` and `--tool=drd`
- [ ] **NO MEMORY LEAKS** - Verified with `valgrind --leak-check=full`
- [ ] **42 NORM COMPLIANT** - All files pass norminette
- [ ] **NO CRASHES/SEGFAULTS** - Robust error handling throughout
- [ ] **DEATH TIMING ≤10ms** - Critical: death messages within 10ms of actual death
- [ ] **NO MESSAGE OVERLAP** - Protected output with mutexes/semaphores

### ✅ **MANDATORY PART (`philo/`) REQUIREMENTS**
- [ ] One thread per philosopher
- [ ] One mutex per fork
- [ ] Mutex protection for fork state changes
- [ ] Death verification with mutex protection
- [ ] All test cases pass (see Testing section)

### ✅ **BONUS PART (`philo_bonus/`) REQUIREMENTS**  
- [ ] One process per philosopher (main ≠ philosopher)
- [ ] Single semaphore for all forks
- [ ] No orphan processes after execution
- [ ] Protected output against multiple access
- [ ] Death verification with semaphore protection

---

## � Quick Start

### **Build & Run**
```bash
# Mandatory part
cd philo/
make
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]

# Bonus part  
cd philo_bonus/
make bonus
./philo_bonus number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

### **Arguments**
- `number_of_philosophers`: Number of philosophers and forks (≥1)
- `time_to_die`: Time in ms before a philosopher dies of starvation (>0)
- `time_to_eat`: Time in ms a philosopher spends eating (>0) 
- `time_to_sleep`: Time in ms a philosopher spends sleeping (>0)
- `number_of_times_each_philosopher_must_eat`: Optional stopping condition (≥1)

**Note**: All times in milliseconds, positive integers only. Invalid arguments should exit with error message.

---

## 📋 Required Testing (Evaluation Sheet)

### **CRITICAL TESTS** ⚠️
```bash
# Test 1: Single philosopher must die
./philo 1 800 200 200
# Expected: Philosopher should die around 800ms

# Test 2: Basic stability  
./philo 5 800 200 200
# Expected: No deaths, continuous simulation

# Test 3: Meal count termination
./philo 5 800 200 200 7  
# Expected: Stops when all eat 7 times, no deaths

# Test 4: Edge case survival
./philo 4 410 200 200
# Expected: No deaths (tight timing)

# Test 5: Edge case death
./philo 4 310 200 100
# Expected: One philosopher should die

# Test 6: Two philosophers timing precision
./philo 2 [various_times]
# Expected: Death within 10ms of actual time
```

### **Stress Testing**
```bash
# Memory leak check
valgrind --leak-check=full ./philo 5 800 200 200 3

# Data race detection  
valgrind --tool=helgrind ./philo 5 800 200 200
valgrind --tool=drd ./philo 5 800 200 200

# High philosopher count (max 200)
./philo 200 800 200 200
```

**⚠️ EVALUATION LIMITS**: 
- Max 200 philosophers
- Min 60ms for time_to_die/eat/sleep
- Death message delay >10ms = FAIL

---

## 🏗️ Data Structures & Headers

### **Mandatory Part (`philo/philo.h`)**
```c
#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/time.h>
# include <pthread.h>

/*
** Allowed functions: memset, printf, malloc, free, write, usleep, gettimeofday
** pthread_create, pthread_detach, pthread_join, pthread_mutex_init,
** pthread_mutex_destroy, pthread_mutex_lock, pthread_mutex_unlock
*/

typedef struct s_philo	t_philo;
typedef struct s_table	t_table;

typedef struct s_philo
{
	int				id;
	int				meals_count;
	long			last_meal_time;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	t_table			*table;
}	t_philo;

typedef struct s_table
{
	int				philo_count;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				must_eat_count;
	long			start_time;
	bool			simulation_end;
	pthread_mutex_t	*forks;
	pthread_mutex_t	write_lock;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	sim_lock;
	pthread_t		monitor;
	t_philo			*philos;
}	t_table;

#endif
```

### **Bonus Part (`philo_bonus/philo_bonus.h`)**
```c
#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/time.h>
# include <pthread.h>
# include <semaphore.h>
# include <signal.h>
# include <sys/wait.h>

/*
** Additional allowed functions: fork, kill, exit, waitpid
** sem_open, sem_close, sem_post, sem_wait, sem_unlink
*/

typedef struct s_philo_bonus	t_philo_bonus;
typedef struct s_table_bonus	t_table_bonus;

typedef struct s_philo_bonus
{
	int				id;
	int				meals_count;
	long			last_meal_time;
	pid_t			pid;
	pthread_t		monitor_thread;
	t_table_bonus	*table;
}	t_philo_bonus;

typedef struct s_table_bonus
{
	int				philo_count;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				must_eat_count;
	long			start_time;
	sem_t			*forks;
	sem_t			*write_sem;
	sem_t			*meal_sem;
	sem_t			*sim_sem;
	t_philo_bonus	*philos;
}	t_table_bonus;

#endif
```

---

## � Implementation Roadmap & Function Documentation

### **PHASE 1: Foundation & Argument Parsing**

#### **1.1 Input Validation (`src/parsing.c`)**
```c
/*
** @brief: Validates command line arguments and converts to integers
** @param: argc - argument count, argv - argument array  
** @return: 0 on success, 1 on error
** TODO:
**   1. Check argc is 5 or 6
**   2. Validate each argument is positive integer
**   3. Check ranges: philo_count >= 1, times > 0
**   4. Handle optional must_eat parameter
**   5. Return error codes for invalid input
*/
int		validate_args(int argc, char **argv);

/*
** @brief: Converts string to positive integer with overflow protection
** @param: str - string to convert
** @return: converted integer or -1 on error
** TODO:
**   1. Skip whitespace
**   2. Check for '+' sign only (no '-')
**   3. Convert digit by digit with overflow check
**   4. Ensure entire string is consumed
**   5. Return -1 for invalid input
*/
int		ft_atoi_positive(const char *str);

/*
** @brief: Parses arguments into table structure
** @param: table - table structure to fill, argc/argv - arguments
** @return: 0 on success, 1 on error
** TODO:
**   1. Call validate_args first
**   2. Convert each argument using ft_atoi_positive
**   3. Fill table structure fields
**   4. Set must_eat_count to -1 if not provided
**   5. Validate special case: 1 philosopher
*/
int		parse_arguments(t_table *table, int argc, char **argv);
```

#### **1.2 Time Management (`src/time.c`)**
```c
/*
** @brief: Gets current timestamp in milliseconds
** @param: void
** @return: timestamp in ms since epoch
** TODO:
**   1. Call gettimeofday() 
**   2. Convert tv_sec to milliseconds
**   3. Add tv_usec converted to milliseconds
**   4. Return total as long
*/
long	get_time_ms(void);

/*
** @brief: Calculates elapsed time since start
** @param: start_time - reference timestamp
** @return: elapsed time in milliseconds
** TODO:
**   1. Get current time with get_time_ms()
**   2. Subtract start_time
**   3. Return difference
*/
long	elapsed_time(long start_time);

/*
** @brief: Precise sleep function using usleep
** @param: duration - sleep time in milliseconds
** @return: void
** TODO:
**   1. Convert ms to microseconds
**   2. Use usleep() for the duration
**   3. Handle potential usleep interruptions
**   4. Ensure minimum precision for timing
*/
void	precise_sleep(long duration);

/*
** @brief: Enhanced sleep with busy-wait for precision
** @param: duration - sleep time in ms
** @return: void  
** TODO:
**   1. Get start time
**   2. Use usleep for 90% of duration
**   3. Busy-wait for remaining 10% for precision
**   4. Check elapsed time continuously
*/
void	smart_sleep(long duration);
```

### **PHASE 2: Memory Management & Initialization**

#### **2.1 Table Initialization (`src/init.c`)**
```c
/*
** @brief: Initializes all mutexes in the table
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Initialize write_lock mutex
**   2. Initialize meal_lock mutex  
**   3. Initialize sim_lock mutex
**   4. Initialize array of fork mutexes
**   5. Handle initialization failures properly
*/
int		init_mutexes(t_table *table);

/*
** @brief: Allocates and initializes philosopher array
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Allocate memory for philos array
**   2. Initialize each philosopher's basic data
**   3. Assign left and right fork pointers
**   4. Set table reference for each philosopher
**   5. Handle memory allocation failures
*/
int		init_philosophers(t_table *table);

/*
** @brief: Main initialization function
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Set simulation_end to false
**   2. Record start_time with get_time_ms()
**   3. Call init_mutexes()
**   4. Call init_philosophers()
**   5. Handle any initialization failures
*/
int		init_table(t_table *table);

/*
** @brief: Cleans up all allocated resources
** @param: table - pointer to table structure
** @return: void
** TODO:
**   1. Destroy all fork mutexes
**   2. Destroy other mutexes (write, meal, sim)
**   3. Free philosophers array
**   4. Free forks array
**   5. Reset all pointers to NULL
*/
void	cleanup_table(t_table *table);
```

### **PHASE 3: Core Philosopher Logic**

#### **3.1 Philosopher Actions (`src/actions.c`)**
```c
/*
** @brief: Philosopher takes both forks (deadlock-safe)
** @param: philo - pointer to philosopher
** @return: void
** TODO:
**   1. Handle single philosopher case (return immediately)
**   2. Determine fork order (even/odd strategy)
**   3. Lock first fork with mutex
**   4. Print "has taken a fork" message
**   5. Lock second fork with mutex
**   6. Print second "has taken a fork" message
*/
void	take_forks(t_philo *philo);

/*
** @brief: Philosopher eating action
** @param: philo - pointer to philosopher
** @return: void
** TODO:
**   1. Print "is eating" message
**   2. Lock meal_lock mutex
**   3. Update last_meal_time to current time
**   4. Increment meals_count
**   5. Unlock meal_lock mutex
**   6. Sleep for time_to_eat duration
*/
void	eat_action(t_philo *philo);

/*
** @brief: Philosopher releases both forks
** @param: philo - pointer to philosopher  
** @return: void
** TODO:
**   1. Unlock left fork mutex
**   2. Unlock right fork mutex
**   3. No printing required for fork release
*/
void	drop_forks(t_philo *philo);

/*
** @brief: Philosopher sleeping action
** @param: philo - pointer to philosopher
** @return: void
** TODO:
**   1. Print "is sleeping" message
**   2. Sleep for time_to_sleep duration
*/
void	sleep_action(t_philo *philo);

/*
** @brief: Philosopher thinking action
** @param: philo - pointer to philosopher
** @return: void
** TODO:
**   1. Print "is thinking" message
**   2. Optional: brief thinking delay for load balancing
*/
void	think_action(t_philo *philo);
```

#### **3.2 Philosopher Routine (`src/routine.c`)**
```c
/*
** @brief: Main philosopher thread routine
** @param: arg - pointer to philosopher (void* cast)
** @return: NULL
** TODO:
**   1. Cast arg to t_philo*
**   2. Handle even philosopher delay (prevent all starting together)
**   3. Main loop: check simulation_end
**   4. Execute: take_forks -> eat -> drop_forks -> sleep -> think
**   5. Break loop if simulation ends or death occurs
*/
void	*philosopher_routine(void *arg);

/*
** @brief: Creates all philosopher threads
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Loop through all philosophers
**   2. Create thread for each with pthread_create
**   3. Pass philosopher pointer as argument
**   4. Handle thread creation failures
**   5. Store thread ID in philosopher structure
*/
int		create_threads(t_table *table);

/*
** @brief: Waits for all philosopher threads to complete
** @param: table - pointer to table structure
** @return: void
** TODO:
**   1. Loop through all philosophers
**   2. Call pthread_join for each thread
**   3. Handle join failures appropriately
*/
void	join_threads(t_table *table);
```

### **PHASE 4: Monitoring & Synchronization**

#### **4.1 Death Monitor (`src/monitor.c`)**
```c
/*
** @brief: Checks if a philosopher has died
** @param: philo - philosopher to check
** @return: 1 if dead, 0 if alive
** TODO:
**   1. Lock meal_lock mutex
**   2. Get philosopher's last_meal_time
**   3. Unlock meal_lock mutex
**   4. Calculate time since last meal
**   5. Compare with time_to_die threshold
*/
int		is_philosopher_dead(t_philo *philo);

/*
** @brief: Checks if all philosophers have eaten enough
** @param: table - pointer to table structure
** @return: 1 if all satisfied, 0 otherwise
** TODO:
**   1. Check if must_eat_count is set (-1 means not set)
**   2. Lock meal_lock mutex
**   3. Count philosophers with meals_count >= must_eat_count
**   4. Unlock meal_lock mutex
**   5. Return 1 if all have eaten enough
*/
int		all_philosophers_satisfied(t_table *table);

/*
** @brief: Main monitoring thread routine
** @param: arg - pointer to table structure (void* cast)
** @return: NULL
** TODO:
**   1. Cast arg to t_table*
**   2. Loop while simulation is running
**   3. Check each philosopher for death
**   4. Check if all philosophers satisfied
**   5. Set simulation_end flag if stopping condition met
**   6. Sleep briefly between checks (1ms)
*/
void	*monitor_routine(void *arg);

/*
** @brief: Starts the monitoring thread
** @param: table - pointer to table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Create monitor thread with pthread_create
**   2. Pass table pointer as argument
**   3. Handle thread creation failure
**   4. Store thread ID in table structure
*/
int		start_monitor(t_table *table);
```

#### **4.2 Synchronization Utilities (`src/sync.c`)**
```c
/*
** @brief: Thread-safe output function
** @param: philo - philosopher performing action, msg - message to print
** @return: void
** TODO:
**   1. Lock write_lock mutex
**   2. Check if simulation has ended (no output after death)
**   3. Get current timestamp with elapsed_time()
**   4. Print formatted message: "timestamp id message"
**   5. Unlock write_lock mutex
*/
void	safe_print(t_philo *philo, char *msg);

/*
** @brief: Thread-safe death announcement
** @param: philo - philosopher who died
** @return: void
** TODO:
**   1. Lock write_lock mutex
**   2. Print death message with timestamp
**   3. Set simulation_end flag to true
**   4. Unlock write_lock mutex
*/
void	announce_death(t_philo *philo);

/*
** @brief: Checks if simulation should end
** @param: table - pointer to table structure
** @return: 1 if should end, 0 if continue
** TODO:
**   1. Lock sim_lock mutex
**   2. Read simulation_end flag
**   3. Unlock sim_lock mutex
**   4. Return flag value
*/
int		should_end_simulation(t_table *table);

/*
** @brief: Sets simulation end flag safely
** @param: table - pointer to table structure
** @return: void
** TODO:
**   1. Lock sim_lock mutex
**   2. Set simulation_end to true
**   3. Unlock sim_lock mutex
*/
void	end_simulation(t_table *table);
```

### **PHASE 5: Main Program Flow**

#### **5.1 Main Function (`src/main.c`)**
```c
/*
** @brief: Entry point of the program
** @param: argc - argument count, argv - argument array
** @return: 0 on success, 1 on error
** TODO:
**   1. Validate argument count (argc == 5 or 6)
**   2. Declare and initialize table structure
**   3. Call parse_arguments()
**   4. Call init_table()
**   5. Call create_threads()
**   6. Call start_monitor()
**   7. Call join_threads()
**   8. Call cleanup_table()
**   9. Return appropriate exit code
*/
int		main(int argc, char **argv);
```

### **BONUS PART: Process Implementation (`philo_bonus/`)**

#### **B.1 Bonus Initialization (`src_bonus/init_bonus.c`)**
```c
/*
** @brief: Initializes semaphores for bonus part
** @param: table - pointer to bonus table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Create named semaphore for forks (value = philo_count)
**   2. Create named semaphore for write protection (value = 1)
**   3. Create named semaphore for meal protection (value = 1)  
**   4. Create named semaphore for simulation control (value = 1)
**   5. Handle semaphore creation failures
**   6. Use unique names to avoid conflicts
*/
int		init_semaphores(t_table_bonus *table);

/*
** @brief: Initializes philosopher processes array
** @param: table - pointer to bonus table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Allocate memory for philos array
**   2. Initialize each philosopher's basic data
**   3. Set table reference for each philosopher
**   4. Initialize PID to -1 (not yet forked)
**   5. Handle memory allocation failures
*/
int		init_philos_bonus(t_table_bonus *table);

/*
** @brief: Main bonus initialization function
** @param: table - pointer to bonus table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Set start_time with get_time_ms()
**   2. Call init_semaphores()
**   3. Call init_philos_bonus()
**   4. Handle any initialization failures
*/
int		init_table_bonus(t_table_bonus *table);

/*
** @brief: Cleanup function for bonus part
** @param: table - pointer to bonus table structure
** @return: void
** TODO:
**   1. Close all semaphores (sem_close)
**   2. Unlink all semaphores (sem_unlink) 
**   3. Free philosophers array
**   4. Reset all pointers to NULL
*/
void	cleanup_table_bonus(t_table_bonus *table);
```

#### **B.2 Process Management (`src_bonus/process.c`)**
```c
/*
** @brief: Individual philosopher process routine
** @param: philo - pointer to philosopher structure
** @return: void (process exits)
** TODO:
**   1. Start monitoring thread for this process
**   2. Initialize last_meal_time to start_time
**   3. Main loop: check if simulation should end
**   4. Execute: take_forks -> eat -> drop_forks -> sleep -> think
**   5. Exit process when simulation ends
*/
void	philosopher_process(t_philo_bonus *philo);

/*
** @brief: Creates all philosopher processes
** @param: table - pointer to bonus table structure
** @return: 0 on success, 1 on error
** TODO:
**   1. Loop through all philosophers
**   2. Fork process for each philosopher
**   3. In child: call philosopher_process()
**   4. In parent: store child PID
**   5. Handle fork failures
*/
int		create_processes(t_table_bonus *table);

/*
** @brief: Waits for all processes to complete
** @param: table - pointer to bonus table structure
** @return: void
** TODO:
**   1. Loop through all philosophers
**   2. Call waitpid for each process
**   3. Handle process termination
**   4. Clean up zombie processes
*/
void	wait_processes(t_table_bonus *table);

/*
** @brief: Terminates all philosopher processes
** @param: table - pointer to bonus table structure
** @return: void
** TODO:
**   1. Loop through all philosophers
**   2. Send SIGTERM to each process (kill)
**   3. Wait for processes to terminate
**   4. Handle stubborn processes with SIGKILL if needed
*/
void	terminate_processes(t_table_bonus *table);
```

#### **B.3 Bonus Actions (`src_bonus/actions_bonus.c`)**
```c
/*
** @brief: Philosopher takes forks using semaphores
** @param: philo - pointer to philosopher
** @return: void
** TODO:
**   1. Wait on forks semaphore (sem_wait) - first fork
**   2. Print "has taken a fork" message
**   3. Wait on forks semaphore (sem_wait) - second fork  
**   4. Print "has taken a fork" message
**   5. Handle semaphore errors
*/
void	take_forks_bonus(t_philo_bonus *philo);

/*
** @brief: Philosopher releases forks using semaphores
** @param: philo - pointer to philosopher
** @return: void
** TODO:
**   1. Post to forks semaphore (sem_post) - release first fork
**   2. Post to forks semaphore (sem_post) - release second fork
**   3. Handle semaphore errors
*/
void	drop_forks_bonus(t_philo_bonus *philo);

/*
** @brief: Bonus eating action with semaphore protection
** @param: philo - pointer to philosopher
** @return: void
** TODO:
**   1. Print "is eating" message with safe_print_bonus
**   2. Wait on meal_sem for protection
**   3. Update last_meal_time to current time
**   4. Increment meals_count
**   5. Post to meal_sem to release protection
**   6. Sleep for time_to_eat duration
*/
void	eat_action_bonus(t_philo_bonus *philo);

/*
** @brief: Thread-safe print for bonus (semaphore protection)
** @param: philo - philosopher, msg - message to print
** @return: void
** TODO:
**   1. Wait on write_sem for exclusive access
**   2. Check if simulation should continue
**   3. Get timestamp and print formatted message
**   4. Post to write_sem to release access
*/
void	safe_print_bonus(t_philo_bonus *philo, char *msg);
```

#### **B.4 Bonus Monitor (`src_bonus/monitor_bonus.c`)**
```c
/*
** @brief: Death monitor thread within each process
** @param: arg - pointer to philosopher (void* cast)
** @return: NULL
** TODO:
**   1. Cast arg to t_philo_bonus*
**   2. Loop continuously checking death condition
**   3. Wait on meal_sem to safely read last_meal_time
**   4. Calculate time since last meal
**   5. If death detected: print death, terminate process
**   6. Post to meal_sem and continue monitoring
**   7. Sleep briefly between checks (1ms)
*/
void	*monitor_thread_bonus(void *arg);

/*
** @brief: Starts monitoring thread for current process
** @param: philo - pointer to philosopher
** @return: 0 on success, 1 on error
** TODO:
**   1. Create monitoring thread with pthread_create
**   2. Pass philosopher pointer as argument
**   3. Detach thread (no need to join)
**   4. Handle thread creation failure
*/
int		start_monitor_bonus(t_philo_bonus *philo);
```

#### **B.5 Bonus Main (`src_bonus/main_bonus.c`)**
```c
/*
** @brief: Entry point for bonus program
** @param: argc - argument count, argv - argument array
** @return: 0 on success, 1 on error
** TODO:
**   1. Validate argument count
**   2. Declare and initialize bonus table structure
**   3. Call parse_arguments() (reuse from mandatory)
**   4. Call init_table_bonus()
**   5. Call create_processes()
**   6. Call wait_processes()
**   7. Call cleanup_table_bonus()
**   8. Return appropriate exit code
*/
int		main(int argc, char **argv);
```

---

## 🎯 Implementation Order & Dependencies

### **Priority 1: Foundation (Build First)**
1. **Time utilities** (`time.c`) - No dependencies
2. **Argument parsing** (`parsing.c`) - Depends on time utils
3. **Safe print utilities** (`sync.c`) - Basic synchronization

### **Priority 2: Core Structure (Build Second)**  
4. **Table initialization** (`init.c`) - Depends on parsing
5. **Philosopher actions** (`actions.c`) - Depends on sync utils
6. **Philosopher routine** (`routine.c`) - Depends on actions

### **Priority 3: Advanced Features (Build Third)**
7. **Death monitor** (`monitor.c`) - Depends on all above
8. **Main program** (`main.c`) - Integrates everything

---

## 📝 Output Format & Testing (EXACT SPECIFICATION)

```
[timestamp_in_ms] [philosopher_id] has taken a fork
[timestamp_in_ms] [philosopher_id] is eating  
[timestamp_in_ms] [philosopher_id] is sleeping
[timestamp_in_ms] [philosopher_id] is thinking
[timestamp_in_ms] [philosopher_id] died
```

**CRITICAL REQUIREMENTS:**
- No message overlap (protect with mutex/semaphore)
- Death message within 10ms of actual death
- No output after death message
- Timestamp = milliseconds since simulation start
- Philosopher IDs from 1 to N

---

## 🛠️ Makefile Requirements

### **Mandatory (`philo/Makefile`)**
```makefile
NAME = philo
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SOURCES = main.c init.c routine.c monitor.c time.c utils.c

all: $(NAME)
$(NAME): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(NAME)
clean:
	# Remove .o files if any
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re
```

### **Bonus (`philo_bonus/Makefile`)**  
```makefile
NAME = philo_bonus  
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SOURCES = main_bonus.c init_bonus.c routine_bonus.c monitor_bonus.c time_bonus.c utils_bonus.c

bonus: $(NAME)
$(NAME): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(NAME)
clean:
	# Remove .o files if any  
fclean: clean
	rm -f $(NAME)
re: fclean bonus
.PHONY: bonus clean fclean re
```

---

## 🔄 Critical Implementation Strategies

### **Deadlock Prevention (MANDATORY)**
```c
// Strategy 1: Ordered fork acquisition (recommended)
void take_forks(t_philosopher *philo)
{
    if (philo->id % 2 == 0)  // Even philosophers
    {
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "has taken a fork");
    }
    else  // Odd philosophers  
    {
        pthread_mutex_lock(philo->left_fork);
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        print_status(philo, "has taken a fork");
    }
}

// Strategy 2: Single philosopher special case
if (data->num_philosophers == 1)
{
    // Only one fork available - philosopher must die
    pthread_mutex_lock(philo->left_fork);
    print_status(philo, "has taken a fork");
    precise_usleep(data->time_to_die + 1);
    return NULL;
}
```

### **Precise Death Detection (10ms requirement)**
```c
void *death_monitor(void *arg)
{
    t_data *data = (t_data *)arg;
    
    while (!simulation_should_stop(data))
    {
        for (int i = 0; i < data->num_philosophers; i++)
        {
            pthread_mutex_lock(&data->meal_mutex);
            long time_since_meal = get_current_time() - data->philosophers[i].last_meal_time;
            pthread_mutex_unlock(&data->meal_mutex);
            
            if (time_since_meal > data->time_to_die)
            {
                stop_simulation(data);
                print_death(&data->philosophers[i]);
                return NULL;
            }
        }
        usleep(100);  // Check every 100 microseconds for precision
    }
    return NULL;
}
```

### **Race Condition Protection**
```c
// Protect all shared data access
void print_status(t_philosopher *philo, char *status)
{
    pthread_mutex_lock(&philo->data->print_mutex);
    if (!simulation_should_stop(philo->data))
    {
        printf("%ld %d %s\n", get_elapsed_time(philo->data->start_time), philo->id, status);
    }
    pthread_mutex_unlock(&philo->data->print_mutex);
}

void eat_action(t_philosopher *philo)
{
    print_status(philo, "is eating");
    
    pthread_mutex_lock(&philo->data->meal_mutex);
    philo->last_meal_time = get_current_time();
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->data->meal_mutex);
    
    precise_usleep(philo->data->time_to_eat);
}
```

---

## ⚠️ Common Pitfalls & Solutions

### **1. Data Races**
❌ **Wrong**: Direct access to shared variables
```c
if (philo->last_meal_time + data->time_to_die < get_current_time())  // RACE!
```
✅ **Correct**: Mutex protection
```c
pthread_mutex_lock(&data->meal_mutex);
long last_meal = philo->last_meal_time;
pthread_mutex_unlock(&data->meal_mutex);
if (last_meal + data->time_to_die < get_current_time())
```

### **2. Memory Leaks**
❌ **Wrong**: Missing cleanup
```c
// Forgot to destroy mutexes and free memory
```
✅ **Correct**: Proper cleanup
```c
void cleanup_data(t_data *data)
{
    for (int i = 0; i < data->num_philosophers; i++)
        pthread_mutex_destroy(&data->forks[i]);
    pthread_mutex_destroy(&data->print_mutex);
    pthread_mutex_destroy(&data->death_mutex);
    pthread_mutex_destroy(&data->meal_mutex);
    free(data->forks);
    free(data->philosophers);
}
```

### **3. Timing Precision Issues**
❌ **Wrong**: Using sleep() or imprecise timing
```c
sleep(time_to_eat / 1000);  // Too imprecise!
```
✅ **Correct**: Microsecond precision
```c
void precise_usleep(long duration_ms)
{
    long start = get_current_time();
    while (get_current_time() - start < duration_ms)
        usleep(100);  // Small sleep to avoid busy waiting
}
```

### **4. Bonus Part Process Management**
❌ **Wrong**: Orphan processes
```c
// Forgot to wait for child processes
```
✅ **Correct**: Proper process cleanup
```c
void wait_for_processes(t_data_bonus *data)
{
    for (int i = 0; i < data->num_philosophers; i++)
    {
        waitpid(data->philosophers[i].process_id, NULL, 0);
    }
}
```

---

## ✅ Evaluation Preparation Checklist

### **Before Defense**
- [ ] Run `norminette` on all files - zero errors
- [ ] Test with `valgrind --leak-check=full` - zero leaks  
- [ ] Test with `valgrind --tool=helgrind` - zero data races
- [ ] Verify all mandatory test cases pass
- [ ] Check death timing precision with 2 philosophers
- [ ] Test edge cases (1 philosopher, high counts)
- [ ] Verify clean compilation with required flags
- [ ] Ensure no global variables in code

### **Defense Strategy**
1. **Explain deadlock prevention**: Describe your fork ordering strategy
2. **Show timing precision**: Demonstrate death detection accuracy  
3. **Walkthrough data protection**: Explain mutex usage for shared data
4. **Process vs Thread differences**: Highlight bonus implementation changes
5. **Handle modifications**: Be ready to adjust timing parameters or add features

### **Key Points to Emphasize**
- **No global variables** - everything in structures
- **Mutex protection** - every shared resource protected  
- **Death timing** - monitor thread checks frequently (100μs intervals)
- **Clean termination** - proper cleanup of all resources
- **Output protection** - no mixed messages via print mutex

---

## 📚 Essential References

- **POSIX Threads**: https://pubs.opengroup.org/onlinepubs/9699919799/
- **POSIX Semaphores**: https://man7.org/linux/man-pages/man7/sem_overview.7.html
- **Dining Philosophers**: Dijkstra (1965) - Classic concurrent programming problem
- **Data Race Detection**: `man valgrind`, `--tool=helgrind`, `--tool=drd`
- **42 Norm**: Mandatory coding standard compliance

**SUCCESS FORMULA**: Precise timing + Deadlock prevention + Race condition protection + Clean resource management = Outstanding evaluation

---

## � Project Files

- **[TODO.md](./TODO.md)** - Detailed implementation roadmap with checkboxes and time estimates
- **README.md** - This documentation with technical specifications and evaluation criteria
