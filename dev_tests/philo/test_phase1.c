/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_phase1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 00:00:00 by mhaddadi          #+#    #+#             */
/*   Updated: 2025/10/27 00:00:00 by mhaddadi         ###   ########.fr       */
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
/*                         TIME FUNCTION TESTS                                */
/* ************************************************************************** */

void	test_get_time_ms(void)
{
	long	time1;
	long	time2;

	TEST_SECTION("Testing get_time_ms()");
	
	time1 = get_time_ms();
	TEST_ASSERT(time1 > 0, "get_time_ms returns positive value");
	
	usleep(1000);
	time2 = get_time_ms();
	TEST_ASSERT(time2 > time1, "get_time_ms increases with time");
	TEST_ASSERT((time2 - time1) >= 1 && (time2 - time1) <= 10,
		"get_time_ms has millisecond precision");
}

void	test_elapsed_time(void)
{
	long	start;
	long	elapsed;

	TEST_SECTION("Testing elapsed_time()");
	
	start = get_time_ms();
	usleep(5000);
	elapsed = elapsed_time(start);
	
	TEST_ASSERT(elapsed >= 4 && elapsed <= 10,
		"elapsed_time measures ~5ms correctly");
	
	usleep(10000);
	elapsed = elapsed_time(start);
	TEST_ASSERT(elapsed >= 14 && elapsed <= 20,
		"elapsed_time accumulates correctly");
}

void	test_precise_sleep(void)
{
	long	start;
	long	duration;

	TEST_SECTION("Testing precise_sleep()");
	
	/* Test 10ms sleep */
	start = get_time_ms();
	precise_sleep(10);
	duration = elapsed_time(start);
	TEST_ASSERT(duration >= 10 && duration <= 15,
		"precise_sleep(10) sleeps for ~10ms");
	
	/* Test 50ms sleep */
	start = get_time_ms();
	precise_sleep(50);
	duration = elapsed_time(start);
	TEST_ASSERT(duration >= 50 && duration <= 60,
		"precise_sleep(50) sleeps for ~50ms");
	
	/* Test 1ms sleep (edge case) */
	start = get_time_ms();
	precise_sleep(1);
	duration = elapsed_time(start);
	TEST_ASSERT(duration >= 1 && duration <= 5,
		"precise_sleep(1) sleeps for ~1ms");
}

void	test_smart_sleep(void)
{
	long	start;
	long	duration;

	TEST_SECTION("Testing smart_sleep()");
	
	/* Test 10ms sleep */
	start = get_time_ms();
	smart_sleep(10);
	duration = elapsed_time(start);
	TEST_ASSERT(duration >= 10 && duration <= 15,
		"smart_sleep(10) sleeps for ~10ms");
	
	/* Test 100ms sleep */
	start = get_time_ms();
	smart_sleep(100);
	duration = elapsed_time(start);
	TEST_ASSERT(duration >= 100 && duration <= 110,
		"smart_sleep(100) sleeps for ~100ms");
}

/* ************************************************************************** */
/*                        PARSING FUNCTION TESTS                              */
/* ************************************************************************** */

void	test_ft_atoi_positive(void)
{
	TEST_SECTION("Testing ft_atoi_positive()");
	
	/* Valid cases */
	TEST_ASSERT(ft_atoi_positive("42") == 42,
		"ft_atoi_positive('42') returns 42");
	TEST_ASSERT(ft_atoi_positive("0") == 0,
		"ft_atoi_positive('0') returns 0");
	TEST_ASSERT(ft_atoi_positive("2147483647") == 2147483647,
		"ft_atoi_positive('2147483647') returns INT_MAX");
	TEST_ASSERT(ft_atoi_positive("+123") == 123,
		"ft_atoi_positive('+123') returns 123");
	TEST_ASSERT(ft_atoi_positive("  456") == 456,
		"ft_atoi_positive('  456') handles leading spaces");
	
	/* Invalid cases - should return -1 */
	TEST_ASSERT(ft_atoi_positive("-42") == -1,
		"ft_atoi_positive('-42') returns -1 (negative)");
	TEST_ASSERT(ft_atoi_positive("abc") == -1,
		"ft_atoi_positive('abc') returns -1 (non-numeric)");
	TEST_ASSERT(ft_atoi_positive("12abc") == -1,
		"ft_atoi_positive('12abc') returns -1 (trailing chars)");
	TEST_ASSERT(ft_atoi_positive("2147483648") == -1,
		"ft_atoi_positive('2147483648') returns -1 (overflow)");
	TEST_ASSERT(ft_atoi_positive("") == -1,
		"ft_atoi_positive('') returns -1 (empty string)");
	TEST_ASSERT(ft_atoi_positive(NULL) == -1,
		"ft_atoi_positive(NULL) returns -1");
	TEST_ASSERT(ft_atoi_positive("  ") == -1,
		"ft_atoi_positive('  ') returns -1 (only spaces)");
}

void	test_validate_args(void)
{
	char	*valid_args1[] = {"./philo", "5", "800", "200", "200"};
	char	*valid_args2[] = {"./philo", "4", "410", "200", "200", "7"};
	char	*invalid_args1[] = {"./philo", "5", "800", "200"};
	char	*invalid_args2[] = {"./philo", "-5", "800", "200", "200"};
	char	*invalid_args3[] = {"./philo", "5", "0", "200", "200"};
	char	*invalid_args4[] = {"./philo", "abc", "800", "200", "200"};

	TEST_SECTION("Testing validate_args()");
	
	TEST_ASSERT(validate_args(5, valid_args1) == 0,
		"validate_args accepts 5 valid arguments");
	TEST_ASSERT(validate_args(6, valid_args2) == 0,
		"validate_args accepts 6 valid arguments");
	TEST_ASSERT(validate_args(4, invalid_args1) != 0,
		"validate_args rejects wrong argument count");
	TEST_ASSERT(validate_args(5, invalid_args2) != 0,
		"validate_args rejects negative numbers");
	TEST_ASSERT(validate_args(5, invalid_args3) != 0,
		"validate_args rejects zero values");
	TEST_ASSERT(validate_args(5, invalid_args4) != 0,
		"validate_args rejects non-numeric arguments");
}

void	test_parse_arguments(void)
{
	t_table	table;
	char	*args1[] = {"./philo", "5", "800", "200", "200"};
	char	*args2[] = {"./philo", "4", "410", "200", "100", "7"};

	TEST_SECTION("Testing parse_arguments()");
	
	/* Test basic parsing */
	memset(&table, 0, sizeof(t_table));
	TEST_ASSERT(parse_arguments(&table, 5, args1) == 0,
		"parse_arguments succeeds with valid args");
	TEST_ASSERT(table.philo_count == 5,
		"parse_arguments sets philo_count correctly");
	TEST_ASSERT(table.time_to_die == 800,
		"parse_arguments sets time_to_die correctly");
	TEST_ASSERT(table.time_to_eat == 200,
		"parse_arguments sets time_to_eat correctly");
	TEST_ASSERT(table.time_to_sleep == 200,
		"parse_arguments sets time_to_sleep correctly");
	TEST_ASSERT(table.must_eat_count == -1,
		"parse_arguments sets must_eat_count to -1 when not provided");
	
	/* Test with optional parameter */
	memset(&table, 0, sizeof(t_table));
	TEST_ASSERT(parse_arguments(&table, 6, args2) == 0,
		"parse_arguments succeeds with optional parameter");
	TEST_ASSERT(table.must_eat_count == 7,
		"parse_arguments sets must_eat_count correctly when provided");
}

/* ************************************************************************** */
/*                         INTEGRATION TESTS                                  */
/* ************************************************************************** */

void	test_integration_timing(void)
{
	long	start;
	long	elapsed;
	int		i;

	TEST_SECTION("Integration Test: Timing Precision");
	
	/* Test multiple consecutive sleep operations */
	start = get_time_ms();
	for (i = 0; i < 5; i++)
		precise_sleep(10);
	elapsed = elapsed_time(start);
	TEST_ASSERT(elapsed >= 50 && elapsed <= 70,
		"5x precise_sleep(10) totals ~50ms");
	
	/* Test mixed operations */
	start = get_time_ms();
	precise_sleep(20);
	smart_sleep(30);
	elapsed = elapsed_time(start);
	TEST_ASSERT(elapsed >= 50 && elapsed <= 65,
		"precise_sleep(20) + smart_sleep(30) = ~50ms");
}

void	test_integration_parsing_complete(void)
{
	t_table	table;
	char	*test_cases[][7] = {
		{"./philo", "1", "800", "200", "200", NULL, NULL},
		{"./philo", "5", "800", "200", "200", NULL, NULL},
		{"./philo", "200", "800", "200", "200", NULL, NULL},
		{"./philo", "4", "410", "200", "200", "7", NULL},
	};
	int		expected_counts[] = {1, 5, 200, 4};
	int		i;

	TEST_SECTION("Integration Test: Complete Argument Parsing");
	
	for (i = 0; i < 4; i++)
	{
		memset(&table, 0, sizeof(t_table));
		int argc = (test_cases[i][5] == NULL) ? 5 : 6;
		TEST_ASSERT(parse_arguments(&table, argc, test_cases[i]) == 0,
			"parse_arguments succeeds for test case");
		TEST_ASSERT(table.philo_count == expected_counts[i],
			"Correct philo_count parsed");
	}
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
	printf("║   PHILOSOPHERS - PHASE 1 UNIT TESTS      ║\n");
	printf("║   Foundation & Argument Parsing           ║\n");
	printf("╚═══════════════════════════════════════════╝\n");
	printf(RESET);

	/* Time function tests */
	test_get_time_ms();
	test_elapsed_time();
	test_precise_sleep();
	test_smart_sleep();

	/* Parsing function tests */
	test_ft_atoi_positive();
	test_validate_args();
	test_parse_arguments();

	/* Integration tests */
	test_integration_timing();
	test_integration_parsing_complete();

	/* Print summary */
	print_summary();

	return (g_tests_failed == 0 ? 0 : 1);
}
