# TODO List - Implementation Roadmap

### **SETUP: Project Structure**
```bash
# Create directory structure
mkdir -p philo/{src,include}
mkdir -p philo_bonus/{src,include}

# Create header files
touch philo/include/philo.h
touch philo_bonus/include/philo_bonus.h

# Create Makefiles
touch philo/Makefile
touch philo_bonus/Makefile
```

### **PHASE 1: Foundation & Core Utils** ⏱️ *~2-3 hours*

#### **1.1 Header Files**
- [ ] **`philo/include/philo.h`**
  - [ ] Add all includes (`stdio.h`, `stdlib.h`, `pthread.h`, etc.)
  - [ ] Define `t_philo` and `t_table` structures
  - [ ] Add function prototypes
  - [ ] Add header guards

- [ ] **`philo_bonus/include/philo_bonus.h`**
  - [ ] Add semaphore includes (`semaphore.h`, `signal.h`, `sys/wait.h`)
  - [ ] Define `t_philo_bonus` and `t_table_bonus` structures
  - [ ] Add function prototypes

#### **1.2 Time Management - `philo/src/time.c`**
- [ ] **`long get_time_ms(void)`**
  - [ ] Call `gettimeofday()`
  - [ ] Convert to milliseconds
  - [ ] Return timestamp
  
- [ ] **`long elapsed_time(long start_time)`**
  - [ ] Get current time
  - [ ] Subtract start_time
  - [ ] Return difference
  
- [ ] **`void precise_sleep(long duration)`**
  - [ ] Convert ms to microseconds
  - [ ] Use `usleep()` for duration
  - [ ] Handle interruptions

#### **1.3 Argument Parsing - `philo/src/parsing.c`**
- [ ] **`int ft_atoi_positive(const char *str)`**
  - [ ] Skip whitespace
  - [ ] Check for positive numbers only
  - [ ] Convert with overflow protection
  - [ ] Return -1 on error

- [ ] **`int validate_args(int argc, char **argv)`**
  - [ ] Check argc is 5 or 6
  - [ ] Validate each argument is positive
  - [ ] Check ranges (≥1 for philos, ≥60 for times)
  - [ ] Return 0 on success, 1 on error

- [ ] **`int parse_arguments(t_table *table, int argc, char **argv)`**
  - [ ] Call validate_args first
  - [ ] Fill table structure
  - [ ] Set must_eat_count to -1 if not provided
  - [ ] Handle special case: 1 philosopher

### **PHASE 2: Synchronization & Init** ⏱️ *~3-4 hours*

#### **2.1 Synchronization Utils - `philo/src/sync.c`**
- [ ] **`void safe_print(t_philo *philo, char *msg)`**
  - [ ] Lock write_lock mutex
  - [ ] Check if simulation ended
  - [ ] Print with timestamp and format
  - [ ] Unlock write_lock mutex

- [ ] **`void announce_death(t_philo *philo)`**
  - [ ] Lock write_lock mutex
  - [ ] Print death message
  - [ ] Set simulation_end flag
  - [ ] Unlock write_lock mutex

- [ ] **`bool should_end_simulation(t_table *table)`**
  - [ ] Lock sim_lock mutex
  - [ ] Read simulation_end flag
  - [ ] Unlock sim_lock mutex
  - [ ] Return flag value

- [ ] **`void end_simulation(t_table *table)`**
  - [ ] Lock sim_lock mutex
  - [ ] Set simulation_end to true
  - [ ] Unlock sim_lock mutex

#### **2.2 Table Initialization - `philo/src/init.c`**
- [ ] **`int init_mutexes(t_table *table)`**
  - [ ] Initialize write_lock, meal_lock, sim_lock
  - [ ] Allocate and initialize fork mutexes array
  - [ ] Handle initialization failures
  - [ ] Return 0 on success

- [ ] **`int init_philosophers(t_table *table)`**
  - [ ] Allocate memory for philos array
  - [ ] Initialize each philosopher's data
  - [ ] Assign left/right fork pointers (circular)
  - [ ] Set table reference
  - [ ] Handle allocation failures

- [ ] **`int init_table(t_table *table)`**
  - [ ] Set simulation_end to false
  - [ ] Record start_time
  - [ ] Call init_mutexes()
  - [ ] Call init_philosophers()
  - [ ] Return 0 on success

- [ ] **`void cleanup_table(t_table *table)`**
  - [ ] Destroy all mutexes
  - [ ] Free all allocated memory
  - [ ] Reset pointers to NULL

### **PHASE 3: Core Logic** ⏱️ *~4-5 hours*

#### **3.1 Philosopher Actions - `philo/src/actions.c`**
- [ ] **`void take_forks(t_philo *philo)`**
  - [ ] Handle single philosopher case (return immediately)
  - [ ] Implement even/odd fork ordering strategy
  - [ ] Lock forks with proper order
  - [ ] Print "has taken a fork" for each

- [ ] **`void eat_action(t_philo *philo)`**
  - [ ] Print "is eating"
  - [ ] Lock meal_lock
  - [ ] Update last_meal_time
  - [ ] Increment meals_count
  - [ ] Unlock meal_lock
  - [ ] Sleep for time_to_eat

- [ ] **`void drop_forks(t_philo *philo)`**
  - [ ] Unlock left fork
  - [ ] Unlock right fork

- [ ] **`void sleep_action(t_philo *philo)`**
  - [ ] Print "is sleeping"
  - [ ] Sleep for time_to_sleep

- [ ] **`void think_action(t_philo *philo)`**
  - [ ] Print "is thinking"
  - [ ] Optional: brief delay

#### **3.2 Thread Management - `philo/src/routine.c`**
- [ ] **`void *philosopher_routine(void *arg)`**
  - [ ] Cast arg to t_philo*
  - [ ] Even philosopher delay (prevent sync start)
  - [ ] Main loop: check simulation_end
  - [ ] Execute cycle: take_forks → eat → drop_forks → sleep → think
  - [ ] Return NULL

- [ ] **`int create_threads(t_table *table)`**
  - [ ] Loop through philosophers
  - [ ] Create thread for each with pthread_create
  - [ ] Handle creation failures
  - [ ] Return 0 on success

- [ ] **`void join_threads(t_table *table)`**
  - [ ] Loop through philosophers
  - [ ] Join each thread with pthread_join

### **PHASE 4: Monitoring** ⏱️ *~2-3 hours*

#### **4.1 Death Monitor - `philo/src/monitor.c`**
- [ ] **`bool is_philosopher_dead(t_philo *philo)`**
  - [ ] Lock meal_lock
  - [ ] Get last_meal_time
  - [ ] Unlock meal_lock
  - [ ] Calculate time since last meal
  - [ ] Compare with time_to_die

- [ ] **`bool all_philosophers_satisfied(t_table *table)`**
  - [ ] Check if must_eat_count is set
  - [ ] Lock meal_lock
  - [ ] Count satisfied philosophers
  - [ ] Unlock meal_lock
  - [ ] Return true if all satisfied

- [ ] **`void *death_monitor(void *arg)`**
  - [ ] Cast arg to t_table*
  - [ ] Loop while simulation running
  - [ ] Check each philosopher for death
  - [ ] Check meal completion
  - [ ] Set end flag if needed
  - [ ] Sleep 100µs between checks

- [ ] **`int start_monitor(t_table *table)`**
  - [ ] Create monitor thread
  - [ ] Handle creation failure
  - [ ] Return 0 on success

### **PHASE 5: Main Program** ⏱️ *~1 hour*

#### **5.1 Main Function - `philo/src/main.c`**
- [ ] **`int main(int argc, char **argv)`**
  - [ ] Validate argument count
  - [ ] Declare and initialize table
  - [ ] Call parse_arguments()
  - [ ] Call init_table()
  - [ ] Call create_threads()
  - [ ] Call start_monitor()
  - [ ] Call join_threads()
  - [ ] Call cleanup_table()
  - [ ] Return exit code

### **PHASE 6: Makefiles** ⏱️ *~30 minutes*

#### **6.1 Mandatory Makefile - `philo/Makefile`**
- [ ] Set `NAME = philo`
- [ ] Set compiler and flags (`-Wall -Wextra -Werror -pthread`)
- [ ] List source files
- [ ] Add rules: `all`, `clean`, `fclean`, `re`
- [ ] Ensure no unnecessary relinking

### **PHASE 7: Testing & Validation** ⏱️ *~2 hours*

#### **7.1 Basic Testing**
- [ ] Test: `./philo 1 800 200 200` (should die)
- [ ] Test: `./philo 5 800 200 200` (should run indefinitely)
- [ ] Test: `./philo 5 800 200 200 7` (should stop after 7 meals)
- [ ] Test: `./philo 4 410 200 200` (should survive)
- [ ] Test: `./philo 4 310 200 100` (should die)

#### **7.2 Quality Assurance**
- [ ] Run `norminette` on all files
- [ ] Run `valgrind --leak-check=full`
- [ ] Run `valgrind --tool=helgrind`
- [ ] Run `valgrind --tool=drd`
- [ ] Test death timing precision
- [ ] Test with high philosopher counts

### **BONUS PHASE: Process Implementation** ⏱️ *~6-8 hours*

#### **B.1 Bonus Structure**
- [ ] Copy and adapt mandatory functions for bonus
- [ ] Replace mutexes with semaphores
- [ ] Replace threads with processes
- [ ] Implement process monitoring
- [ ] Create bonus Makefile

#### **B.2 Key Bonus Functions**
- [ ] **`init_semaphores()`** - Create named semaphores
- [ ] **`philosopher_process()`** - Process routine
- [ ] **`create_processes()`** - Fork philosopher processes
- [ ] **`monitor_thread_bonus()`** - Death monitor per process
- [ ] **`wait_processes()`** - Wait for all processes
- [ ] **`cleanup_table_bonus()`** - Close/unlink semaphores

### **FINAL PHASE: Polish & Defense Prep** ⏱️ *~1-2 hours*

#### **F.1 Code Review**
- [ ] Check all error handling
- [ ] Verify no global variables
- [ ] Ensure clean compilation
- [ ] Test edge cases thoroughly

#### **F.2 Defense Preparation**
- [ ] Practice explaining deadlock prevention
- [ ] Understand timing precision implementation
- [ ] Know difference between threads and processes
- [ ] Be ready to modify parameters

---

**ESTIMATED TOTAL TIME: 20-25 hours**
**TARGET: Complete mandatory in 15 hours, bonus in 8 hours, polish in 2 hours**
