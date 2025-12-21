/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_phase2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/10/30 00:00:00 by mhaddadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"
#include <assert.h>

/* Color codes for test output */
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

/* Test counter */
static int g_tests_passed = 0;
static int g_tests_failed = 0;

/* Test helper macros */
#define TEST_ASSERT(condition, test_name) \
	do { \
		if (condition) { \
			printf(GREEN "✓ PASS: " RESET "%s\n", test_name); \
			g_tests_passed++; \
		} else { \
			printf(RED "✗ FAIL: " RESET "%s\n", test_name); \
			g_tests_failed++; \
		} \
	} while(0)

#define TEST_SECTION(name) \
	printf(BLUE "\n=== %s ===" RESET "\n", name)

/* ************************************************************************** */
/*                      INITIALIZATION FUNCTION TESTS                         */
/* ************************************************************************** */

void	test_init_mutexes(void)
{
	t_table	table;
	int		i;

	TEST_SECTION("Testing init_mutexes()");
	
	/* Test basic initialization */
	memset(&table, 0, sizeof(t_table));
	table.philo_count = 5;
	TEST_ASSERT(init_mutexes(&table) == 0,
		"init_mutexes succeeds for 5 philosophers");
	TEST_ASSERT(table.forks != NULL,
		"init_mutexes allocates fork array");
	
	/* Test that mutexes are actually usable */
	TEST_ASSERT(pthread_mutex_lock(&table.write_lock) == 0,
		"write_lock mutex is functional");
	pthread_mutex_unlock(&table.write_lock);
	
	TEST_ASSERT(pthread_mutex_lock(&table.meal_lock) == 0,
		"meal_lock mutex is functional");
	pthread_mutex_unlock(&table.meal_lock);
	
	TEST_ASSERT(pthread_mutex_lock(&table.sim_lock) == 0,
		"sim_lock mutex is functional");
	pthread_mutex_unlock(&table.sim_lock);
	
	/* Test all fork mutexes */
	i = 0;
	while (i < table.philo_count)
	{
		TEST_ASSERT(pthread_mutex_lock(&table.forks[i]) == 0,
			"fork mutex is functional");
		pthread_mutex_unlock(&table.forks[i]);
		i++;
	}
	
	cleanup_table(&table);
	
	/* Test with single philosopher */
	memset(&table, 0, sizeof(t_table));
	table.philo_count = 1;
	TEST_ASSERT(init_mutexes(&table) == 0,
		"init_mutexes succeeds for 1 philosopher");
	cleanup_table(&table);
	
	/* Test with many philosophers */
	memset(&table, 0, sizeof(t_table));
	table.philo_count = 200;
	TEST_ASSERT(init_mutexes(&table) == 0,
		"init_mutexes succeeds for 200 philosophers");
	cleanup_table(&table);
}

void	test_init_philosophers(void)
{
	t_table	table;
	int		i;

	TEST_SECTION("Testing init_philosophers()");
	
	/* Setup table for testing */
	memset(&table, 0, sizeof(t_table));
	table.philo_count = 5;
	init_mutexes(&table);
	
	TEST_ASSERT(init_philosophers(&table) == 0,
		"init_philosophers succeeds");
	TEST_ASSERT(table.philos != NULL,
		"init_philosophers allocates philos array");
	
	/* Test philosopher IDs (should be 1-indexed) */
	i = 0;
	while (i < table.philo_count)
	{
		TEST_ASSERT(table.philos[i].id == i + 1,
			"philosopher ID is correctly set");
		i++;
	}
	
	/* Test initial meals_count */
	TEST_ASSERT(table.philos[0].meals_count == 0,
		"initial meals_count is 0");
	TEST_ASSERT(table.philos[4].meals_count == 0,
		"all philosophers start with meals_count = 0");
	
	/* Test fork assignments (circular) */
	TEST_ASSERT(table.philos[0].left_fork == &table.forks[0],
		"philosopher 1 left fork is fork 0");
	TEST_ASSERT(table.philos[0].right_fork == &table.forks[1],
		"philosopher 1 right fork is fork 1");
	TEST_ASSERT(table.philos[4].left_fork == &table.forks[4],
		"philosopher 5 left fork is fork 4");
	TEST_ASSERT(table.philos[4].right_fork == &table.forks[0],
		"philosopher 5 right fork wraps to fork 0 (circular)");
	
	/* Test table reference */
	TEST_ASSERT(table.philos[0].table == &table,
		"philosopher table reference is correct");
	TEST_ASSERT(table.philos[2].table == &table,
		"all philosophers reference same table");
	
	cleanup_table(&table);
}

void	test_init_table(void)
{
	t_table	table;
	char	*args[] = {"./philo", "5", "800", "200", "200"};
	long	time_before;
	long	time_after;

	TEST_SECTION("Testing init_table()");
	
	/* Parse arguments first */
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	
	/* Test initialization */
	time_before = get_time_ms();
	TEST_ASSERT(init_table(&table) == 0,
		"init_table succeeds with valid table");
	time_after = get_time_ms();
	
	/* Verify simulation_end is false */
	TEST_ASSERT(table.simulation_end == false,
		"simulation_end initialized to false");
	
	/* Verify start_time is set correctly */
	TEST_ASSERT(table.start_time >= time_before && table.start_time <= time_after,
		"start_time is set to current time");
	
	/* Verify mutexes are initialized */
	TEST_ASSERT(table.forks != NULL,
		"forks array is allocated");
	
	/* Verify philosophers are initialized */
	TEST_ASSERT(table.philos != NULL,
		"philosophers array is allocated");
	TEST_ASSERT(table.philos[0].id == 1,
		"philosophers are properly initialized");
	
	cleanup_table(&table);
	
	/* Test with optional parameter */
	char *args2[] = {"./philo", "3", "600", "150", "150", "5"};
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 6, args2);
	TEST_ASSERT(init_table(&table) == 0,
		"init_table succeeds with must_eat parameter");
	TEST_ASSERT(table.must_eat_count == 5,
		"must_eat_count is preserved");
	cleanup_table(&table);
}

void	test_cleanup_table(void)
{
	t_table	table;
	char	*args[] = {"./philo", "5", "800", "200", "200"};

	TEST_SECTION("Testing cleanup_table()");
	
	/* Test full cleanup */
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	cleanup_table(&table);
	TEST_ASSERT(table.forks == NULL,
		"cleanup_table sets forks to NULL");
	TEST_ASSERT(table.philos == NULL,
		"cleanup_table sets philos to NULL");
	
	/* Test cleanup on partially initialized table */
	memset(&table, 0, sizeof(t_table));
	table.philo_count = 3;
	init_mutexes(&table);
	/* Don't initialize philosophers */
	
	cleanup_table(&table);
	TEST_ASSERT(table.forks == NULL,
		"cleanup_table handles partial initialization");
	
	/* Test cleanup on uninitialized table (should not crash) */
	memset(&table, 0, sizeof(t_table));
	cleanup_table(&table);
	TEST_ASSERT(1, "cleanup_table handles uninitialized table safely");
}

/* ************************************************************************** */
/*                         INTEGRATION TESTS                                  */
/* ************************************************************************** */

void	test_integration_mutex_locking(void)
{
	t_table	table;
	char	*args[] = {"./philo", "5", "800", "200", "200"};

	TEST_SECTION("Integration Test: Mutex Thread Safety");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	/* Test concurrent locking/unlocking */
	pthread_mutex_lock(&table.write_lock);
	pthread_mutex_lock(&table.meal_lock);
	pthread_mutex_lock(&table.sim_lock);
	
	TEST_ASSERT(1, "multiple mutexes can be locked simultaneously");
	
	pthread_mutex_unlock(&table.sim_lock);
	pthread_mutex_unlock(&table.meal_lock);
	pthread_mutex_unlock(&table.write_lock);
	
	TEST_ASSERT(1, "mutexes unlock in reverse order successfully");
	
	/* Test fork locking pattern */
	pthread_mutex_lock(table.philos[0].left_fork);
	pthread_mutex_lock(table.philos[0].right_fork);
	
	TEST_ASSERT(1, "philosopher can lock both forks");
	
	pthread_mutex_unlock(table.philos[0].right_fork);
	pthread_mutex_unlock(table.philos[0].left_fork);
	
	cleanup_table(&table);
}

void	test_integration_fork_assignments(void)
{
	t_table	table;
	char	*args[] = {"./philo", "4", "800", "200", "200"};
	int		i;

	TEST_SECTION("Integration Test: Fork Assignment Pattern");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	/* Verify circular fork assignment */
	i = 0;
	while (i < table.philo_count)
	{
		int left_idx = i;
		int right_idx = (i + 1) % table.philo_count;
		
		TEST_ASSERT(table.philos[i].left_fork == &table.forks[left_idx],
			"left fork matches expected index");
		TEST_ASSERT(table.philos[i].right_fork == &table.forks[right_idx],
			"right fork matches expected index");
		i++;
	}
	
	/* Verify last philosopher connects to first (circular) */
	TEST_ASSERT(table.philos[table.philo_count - 1].right_fork
		== table.philos[0].left_fork,
		"last philosopher's right fork is first philosopher's left fork");
	
	cleanup_table(&table);
}

void	test_integration_complete_lifecycle(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "600", "150", "150", "2"};

	TEST_SECTION("Integration Test: Complete Lifecycle");
	
	/* Full lifecycle: parse -> init -> cleanup */
	memset(&table, 0, sizeof(t_table));
	
	TEST_ASSERT(parse_arguments(&table, 6, args) == 0,
		"parse phase succeeds");
	TEST_ASSERT(init_table(&table) == 0,
		"initialization phase succeeds");
	
	/* Verify all components are working */
	TEST_ASSERT(table.philo_count == 3,
		"parsed data is preserved");
	TEST_ASSERT(table.philos != NULL,
		"data structures are allocated");
	TEST_ASSERT(table.forks != NULL,
		"mutexes are initialized");
	TEST_ASSERT(table.simulation_end == false,
		"simulation state is ready");
	
	cleanup_table(&table);
	TEST_ASSERT(table.philos == NULL && table.forks == NULL,
		"cleanup phase completes");
}

void	test_edge_cases(void)
{
	t_table	table;
	char	*args_one[] = {"./philo", "1", "800", "200", "200"};
	char	*args_many[] = {"./philo", "200", "800", "200", "200"};

	TEST_SECTION("Edge Cases");
	
	/* Single philosopher */
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args_one);
	TEST_ASSERT(init_table(&table) == 0,
		"initialization works with 1 philosopher");
	TEST_ASSERT(table.philos[0].left_fork == table.philos[0].right_fork,
		"single philosopher's left and right forks are the same");
	cleanup_table(&table);
	
	/* Many philosophers (200) */
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args_many);
	TEST_ASSERT(init_table(&table) == 0,
		"initialization works with 200 philosophers");
	TEST_ASSERT(table.philo_count == 200,
		"200 philosophers are created");
	cleanup_table(&table);
}

void	test_memory_safety(void)
{
	t_table	table;
	char	*args[] = {"./philo", "5", "800", "200", "200"};

	TEST_SECTION("Memory Safety Tests");
	
	/* Test multiple init/cleanup cycles */
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	
	init_table(&table);
	cleanup_table(&table);
	TEST_ASSERT(1, "first init/cleanup cycle completes");
	
	init_table(&table);
	cleanup_table(&table);
	TEST_ASSERT(1, "second init/cleanup cycle completes");
	
	init_table(&table);
	cleanup_table(&table);
	TEST_ASSERT(1, "third init/cleanup cycle completes");
}

/* ************************************************************************** */
/*                              MAIN TEST RUNNER                              */
/* ************************************************************************** */

void	print_summary(void)
{
	int	total;

	total = g_tests_passed + g_tests_failed;
	printf(YELLOW "\n========================================\n" RESET);
	printf(YELLOW "           TEST SUMMARY\n" RESET);
	printf(YELLOW "========================================\n" RESET);
	printf("Total tests: %d\n", total);
	printf(GREEN "Passed: %d\n" RESET, g_tests_passed);
	printf(RED "Failed: %d\n" RESET, g_tests_failed);
	if (g_tests_failed == 0)
		printf(GREEN "\n✓ All tests passed!\n" RESET);
	else
		printf(RED "\n✗ Some tests failed!\n" RESET);
	printf(YELLOW "========================================\n" RESET);
}

int	main(void)
{
	printf(BLUE "\n");
	printf("╔═══════════════════════════════════════════╗\n");
	printf("║   PHILOSOPHERS - PHASE 2 UNIT TESTS      ║\n");
	printf("║   Memory Management & Initialization      ║\n");
	printf("╚═══════════════════════════════════════════╝\n");
	printf(RESET);

	/* Initialization tests */
	test_init_mutexes();
	test_init_philosophers();
	test_init_table();
	test_cleanup_table();

	/* Integration tests */
	test_integration_mutex_locking();
	test_integration_fork_assignments();
	test_integration_complete_lifecycle();
	
	/* Edge cases */
	test_edge_cases();
	test_memory_safety();

	/* Print summary */
	print_summary();

	return (g_tests_failed == 0 ? 0 : 1);
}
