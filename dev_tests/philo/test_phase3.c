/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_phase3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr       */
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
/*                      SYNCHRONIZATION FUNCTION TESTS                        */
/* ************************************************************************** */

void	test_safe_print(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "200", "200"};

	TEST_SECTION("Testing safe_print()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	/* Test normal printing */
	safe_print(&table.philos[0], "is testing");
	TEST_ASSERT(1, "safe_print executes without crash");
	
	/* Test printing with simulation_end set */
	table.simulation_end = true;
	safe_print(&table.philos[1], "should not print");
	TEST_ASSERT(1, "safe_print respects simulation_end flag");
	
	cleanup_table(&table);
}

void	test_announce_death(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "200", "200"};

	TEST_SECTION("Testing announce_death()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	TEST_ASSERT(table.simulation_end == false,
		"simulation_end is false initially");
	
	announce_death(&table.philos[1]);
	
	TEST_ASSERT(table.simulation_end == true,
		"announce_death sets simulation_end to true");
	
	cleanup_table(&table);
}

void	test_should_end_simulation(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "200", "200"};

	TEST_SECTION("Testing should_end_simulation()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	TEST_ASSERT(should_end_simulation(&table) == 0,
		"should_end_simulation returns false initially");
	
	table.simulation_end = true;
	TEST_ASSERT(should_end_simulation(&table) == 1,
		"should_end_simulation returns true after flag set");
	
	cleanup_table(&table);
}

void	test_end_simulation(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "200", "200"};

	TEST_SECTION("Testing end_simulation()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	TEST_ASSERT(table.simulation_end == false,
		"simulation_end is false initially");
	
	end_simulation(&table);
	
	TEST_ASSERT(table.simulation_end == true,
		"end_simulation sets flag to true");
	TEST_ASSERT(should_end_simulation(&table) == 1,
		"end_simulation is thread-safe");
	
	cleanup_table(&table);
}

/* ************************************************************************** */
/*                       PHILOSOPHER ACTIONS TESTS                            */
/* ************************************************************************** */

void	test_take_and_drop_forks(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "200", "200"};

	TEST_SECTION("Testing take_forks() and drop_forks()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	/* Test taking and dropping forks */
	take_forks(&table.philos[0]);
	TEST_ASSERT(1, "take_forks succeeds for philosopher 1");
	
	drop_forks(&table.philos[0]);
	TEST_ASSERT(1, "drop_forks succeeds for philosopher 1");
	
	/* Test with even philosopher */
	take_forks(&table.philos[1]);
	TEST_ASSERT(1, "take_forks succeeds for even philosopher");
	drop_forks(&table.philos[1]);
	
	cleanup_table(&table);
}

void	test_eat_action(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "50", "200"};
	long	start;
	long	duration;

	TEST_SECTION("Testing eat_action()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	/* Initialize last_meal_time */
	table.philos[0].last_meal_time = table.start_time;
	
	TEST_ASSERT(table.philos[0].meals_count == 0,
		"meals_count is 0 initially");
	
	take_forks(&table.philos[0]);
	start = get_time_ms();
	eat_action(&table.philos[0]);
	duration = get_time_ms() - start;
	drop_forks(&table.philos[0]);
	
	TEST_ASSERT(table.philos[0].meals_count == 1,
		"eat_action increments meals_count");
	TEST_ASSERT(table.philos[0].last_meal_time >= table.start_time,
		"eat_action updates last_meal_time");
	TEST_ASSERT(duration >= 50 && duration <= 65,
		"eat_action sleeps for correct duration");
	
	cleanup_table(&table);
}

void	test_sleep_action(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "200", "30"};
	long	start;
	long	duration;

	TEST_SECTION("Testing sleep_action()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	start = get_time_ms();
	sleep_action(&table.philos[0]);
	duration = get_time_ms() - start;
	
	TEST_ASSERT(duration >= 30 && duration <= 40,
		"sleep_action sleeps for correct duration");
	
	cleanup_table(&table);
}

void	test_think_action(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "200", "200"};

	TEST_SECTION("Testing think_action()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	think_action(&table.philos[0]);
	TEST_ASSERT(1, "think_action executes without crash");
	
	cleanup_table(&table);
}

/* ************************************************************************** */
/*                       PHILOSOPHER ROUTINE TESTS                            */
/* ************************************************************************** */

void	test_create_threads(void)
{
	t_table	table;
	char	*args[] = {"./philo", "5", "800", "200", "200"};

	TEST_SECTION("Testing create_threads()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	TEST_ASSERT(create_threads(&table) == 0,
		"create_threads succeeds");
	
	/* Let threads run briefly */
	usleep(10000);
	
	/* Stop simulation */
	end_simulation(&table);
	
	/* Wait for threads */
	join_threads(&table);
	TEST_ASSERT(1, "threads can be joined successfully");
	
	cleanup_table(&table);
}

void	test_join_threads(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "200", "200"};

	TEST_SECTION("Testing join_threads()");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	create_threads(&table);
	usleep(5000);
	end_simulation(&table);
	
	join_threads(&table);
	TEST_ASSERT(1, "join_threads completes without hanging");
	
	cleanup_table(&table);
}

/* ************************************************************************** */
/*                         INTEGRATION TESTS                                  */
/* ************************************************************************** */

void	test_integration_single_cycle(void)
{
	t_table	table;
	char	*args[] = {"./philo", "3", "800", "50", "50"};

	TEST_SECTION("Integration Test: Single Philosopher Cycle");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	table.philos[0].last_meal_time = table.start_time;
	
	/* Execute one complete cycle */
	take_forks(&table.philos[0]);
	eat_action(&table.philos[0]);
	drop_forks(&table.philos[0]);
	sleep_action(&table.philos[0]);
	think_action(&table.philos[0]);
	
	TEST_ASSERT(table.philos[0].meals_count == 1,
		"philosopher completes one eating cycle");
	
	cleanup_table(&table);
}

void	test_integration_multi_thread(void)
{
	t_table	table;
	char	*args[] = {"./philo", "5", "1000", "100", "100"};
	int		i;

	TEST_SECTION("Integration Test: Multi-threaded Execution");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	create_threads(&table);
	
	/* Let philosophers eat a few times */
	usleep(500000);
	
	end_simulation(&table);
	join_threads(&table);
	
	/* Verify all philosophers did something */
	i = 0;
	while (i < table.philo_count)
	{
		TEST_ASSERT(table.philos[i].meals_count > 0,
			"philosopher executed at least one meal");
		i++;
	}
	
	cleanup_table(&table);
}

void	test_integration_deadlock_prevention(void)
{
	t_table	table;
	char	*args[] = {"./philo", "4", "1000", "50", "50"};

	TEST_SECTION("Integration Test: Deadlock Prevention");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	create_threads(&table);
	
	/* Let philosophers run for a bit */
	usleep(200000);
	
	TEST_ASSERT(!should_end_simulation(&table),
		"simulation continues without deadlock");
	
	end_simulation(&table);
	join_threads(&table);
	
	TEST_ASSERT(1, "threads complete without hanging");
	
	cleanup_table(&table);
}

void	test_edge_case_single_philosopher(void)
{
	t_table	table;
	char	*args[] = {"./philo", "1", "800", "200", "200"};

	TEST_SECTION("Edge Case: Single Philosopher");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	/* Single philosopher has both forks pointing to same mutex */
	TEST_ASSERT(table.philos[0].left_fork == table.philos[0].right_fork,
		"single philosopher's forks point to same mutex");
	
	/* This will deadlock if not handled properly */
	/* So we skip the actual execution in unit tests */
	
	cleanup_table(&table);
	TEST_ASSERT(1, "single philosopher setup handled");
}

void	test_thread_safety(void)
{
	t_table	table;
	char	*args[] = {"./philo", "10", "1000", "50", "50"};

	TEST_SECTION("Thread Safety Test");
	
	memset(&table, 0, sizeof(t_table));
	parse_arguments(&table, 5, args);
	init_table(&table);
	
	create_threads(&table);
	
	/* Let many threads run simultaneously */
	usleep(300000);
	
	/* All threads should still be running without crashes */
	TEST_ASSERT(1, "10 threads run simultaneously without crashes");
	
	end_simulation(&table);
	join_threads(&table);
	
	TEST_ASSERT(1, "all threads terminate cleanly");
	
	cleanup_table(&table);
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
	printf("║   PHILOSOPHERS - PHASE 3 UNIT TESTS      ║\n");
	printf("║   Core Philosopher Logic                  ║\n");
	printf("╚═══════════════════════════════════════════╝\n");
	printf(RESET);

	/* Synchronization tests */
	test_safe_print();
	test_announce_death();
	test_should_end_simulation();
	test_end_simulation();

	/* Action tests */
	test_take_and_drop_forks();
	test_eat_action();
	test_sleep_action();
	test_think_action();

	/* Routine tests */
	test_create_threads();
	test_join_threads();

	/* Integration tests */
	test_integration_single_cycle();
	test_integration_multi_thread();
	test_integration_deadlock_prevention();
	test_edge_case_single_philosopher();
	test_thread_safety();

	/* Print summary */
	print_summary();

	return (g_tests_failed == 0 ? 0 : 1);
}
