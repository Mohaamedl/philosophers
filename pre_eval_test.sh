#!/bin/bash

# Pre-Evaluation Test Script for Philosophers
# Run this script right before your evaluation to ensure everything works

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Helper functions
print_header() {
    echo -e "\n${BOLD}${PURPLE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BOLD}${PURPLE}  $1${NC}"
    echo -e "${BOLD}${PURPLE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"
}

print_test() {
    echo -e "${CYAN}[TEST]${NC} $1"
}

print_pass() {
    echo -e "${GREEN}✓ PASS${NC}: $1"
    ((PASSED_TESTS++))
}

print_fail() {
    echo -e "${RED}✗ FAIL${NC}: $1"
    ((FAILED_TESTS++))
}

print_info() {
    echo -e "${BLUE}ℹ INFO${NC}: $1"
}

print_warning() {
    echo -e "${YELLOW}⚠ WARNING${NC}: $1"
}

# Check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Pre-flight checks
print_header "PRE-FLIGHT CHECKS"

# Check required tools
print_test "Checking for required tools..."
TOOLS_OK=true

if ! command_exists gcc; then
    print_fail "gcc not found"
    TOOLS_OK=false
else
    print_pass "gcc found ($(gcc --version | head -n1))"
fi

if ! command_exists make; then
    print_fail "make not found"
    TOOLS_OK=false
else
    print_pass "make found"
fi

if ! command_exists norminette; then
    print_warning "norminette not found (optional but recommended)"
else
    print_pass "norminette found"
fi

if ! command_exists valgrind; then
    print_warning "valgrind not found (optional but recommended)"
else
    print_pass "valgrind found"
fi

if [ "$TOOLS_OK" = false ]; then
    echo -e "\n${RED}${BOLD}Missing required tools! Cannot continue.${NC}"
    exit 1
fi

# Clean previous builds
print_header "CLEANING PREVIOUS BUILDS"
print_test "Running 'make fclean' in mandatory..."
cd philo && make fclean > /dev/null 2>&1 && cd ..
print_pass "Mandatory cleaned"

if [ -d "philo_bonus" ]; then
    print_test "Running 'make fclean' in bonus..."
    cd philo_bonus && make fclean > /dev/null 2>&1 && cd ..
    print_pass "Bonus cleaned"
fi

# Norminette check
if command_exists norminette; then
    print_header "NORMINETTE CHECK"
    
    print_test "Checking mandatory code style..."
    ((TOTAL_TESTS++))
    if norminette philo > /dev/null 2>&1; then
        print_pass "Mandatory norminette: 100%"
    else
        print_fail "Mandatory norminette: ERRORS FOUND"
        norminette philo
    fi
    
    if [ -d "philo_bonus" ]; then
        print_test "Checking bonus code style..."
        ((TOTAL_TESTS++))
        if norminette philo_bonus > /dev/null 2>&1; then
            print_pass "Bonus norminette: 100%"
        else
            print_fail "Bonus norminette: ERRORS FOUND"
            norminette philo_bonus
        fi
    fi
fi

# Compile mandatory
print_header "COMPILING MANDATORY"
print_test "Compiling philo..."
((TOTAL_TESTS++))
cd philo
if make > /dev/null 2>&1; then
    print_pass "Mandatory compilation successful"
    if [ -f "philo" ]; then
        print_pass "Binary 'philo' created"
    else
        print_fail "Binary 'philo' not found"
    fi
else
    print_fail "Mandatory compilation failed"
    make
    cd ..
    exit 1
fi
cd ..

# Compile bonus
if [ -d "philo_bonus" ]; then
    print_header "COMPILING BONUS"
    print_test "Compiling philo_bonus..."
    ((TOTAL_TESTS++))
    cd philo_bonus
    if make > /dev/null 2>&1; then
        print_pass "Bonus compilation successful"
        if [ -f "philo_bonus" ]; then
            print_pass "Binary 'philo_bonus' created"
        else
            print_fail "Binary 'philo_bonus' not found"
        fi
    else
        print_fail "Bonus compilation failed"
        make
        cd ..
        exit 1
    fi
    cd ..
fi

# Mandatory functional tests
print_header "MANDATORY FUNCTIONAL TESTS"

test_mandatory() {
    local test_num=$1
    local args=$2
    local description=$3
    local timeout_duration=$4
    local should_die=$5
    
    print_test "Test $test_num: $description"
    print_info "Command: timeout ${timeout_duration}s ./philo/philo $args"
    ((TOTAL_TESTS++))
    
    if [ "$timeout_duration" = "none" ]; then
        OUTPUT=$(./philo/philo $args 2>&1)
    else
        OUTPUT=$(timeout ${timeout_duration}s ./philo/philo $args 2>&1)
    fi
    EXIT_CODE=$?
    
    if [ "$should_die" = "yes" ]; then
        if echo "$OUTPUT" | grep -q "died"; then
            DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
            print_pass "Philosopher died at ${DEATH_TIME}ms (expected)"
        else
            print_fail "No death detected (one was expected)"
            echo "$OUTPUT"
        fi
    else
        if echo "$OUTPUT" | grep -q "died"; then
            print_fail "Unexpected death detected"
            echo "$OUTPUT" | grep "died"
        else
            print_pass "No deaths (as expected)"
        fi
    fi
}

test_mandatory "1" "1 800 200 200" "Single philosopher (should die)" "none" "yes"
test_mandatory "2" "5 800 200 200" "5 philos, no deaths" "5" "no"
test_mandatory "3" "5 800 200 200 7" "5 philos, 7 meals each" "none" "no"
test_mandatory "4" "4 410 200 200" "Tight timing (no deaths)" "5" "no"
test_mandatory "5" "4 310 200 100" "Very tight (should die)" "none" "yes"
test_mandatory "6" "2 800 200 200" "2 philos (alternation)" "5" "no"

# Bonus functional tests
if [ -d "philo_bonus" ] && [ -f "philo_bonus/philo_bonus" ]; then
    print_header "BONUS FUNCTIONAL TESTS"
    
    test_bonus() {
        local test_num=$1
        local args=$2
        local description=$3
        local timeout_duration=$4
        local should_die=$5
        
        print_test "Test $test_num: $description"
        print_info "Command: timeout ${timeout_duration}s ./philo_bonus/philo_bonus $args"
        ((TOTAL_TESTS++))
        
        if [ "$timeout_duration" = "none" ]; then
            OUTPUT=$(./philo_bonus/philo_bonus $args 2>&1)
        else
            OUTPUT=$(timeout ${timeout_duration}s ./philo_bonus/philo_bonus $args 2>&1)
        fi
        
        # Kill any orphan processes
        pkill -9 philo_bonus 2>/dev/null
        
        if [ "$should_die" = "yes" ]; then
            if echo "$OUTPUT" | grep -q "died"; then
                DEATH_TIME=$(echo "$OUTPUT" | grep "died" | awk '{print $1}')
                print_pass "Philosopher died at ${DEATH_TIME}ms (expected)"
            else
                print_fail "No death detected (one was expected)"
            fi
        else
            if echo "$OUTPUT" | grep -q "died"; then
                print_fail "Unexpected death detected"
                echo "$OUTPUT" | grep "died"
            else
                print_pass "No deaths (as expected)"
            fi
        fi
        
        # Check for orphan processes
        if pgrep philo_bonus > /dev/null; then
            print_fail "Orphan processes detected!"
            pkill -9 philo_bonus
        fi
    }
    
    test_bonus "1" "5 800 200 200" "5 philos, no deaths" "5" "no"
    test_bonus "2" "5 800 200 200 7" "5 philos, 7 meals each" "none" "no"
    test_bonus "3" "4 410 200 200" "Tight timing (no deaths)" "5" "no"
    test_bonus "4" "4 310 200 100" "Very tight (should die)" "none" "yes"
    
    # Check semaphore cleanup
    print_test "Checking semaphore cleanup..."
    ((TOTAL_TESTS++))
    LEAKED_SEMS=$(ls /dev/shm/ 2>/dev/null | grep -E "^(sem\.)?philo" || true)
    if [ -z "$LEAKED_SEMS" ]; then
        print_pass "No leaked semaphores"
    else
        print_fail "Leaked semaphores found: $LEAKED_SEMS"
        # Clean up
        rm -f /dev/shm/sem.philo* /dev/shm/philo* 2>/dev/null
    fi
fi

# Memory leak check (if valgrind available)
if command_exists valgrind; then
    print_header "MEMORY LEAK CHECK"
    
    print_test "Checking mandatory for memory leaks (quick test)..."
    ((TOTAL_TESTS++))
    print_info "Command: valgrind --leak-check=full ./philo/philo 4 410 200 200"
    
    VALGRIND_OUTPUT=$(timeout 6s valgrind --leak-check=full --error-exitcode=1 \
        ./philo/philo 4 410 200 200 2>&1)
    
    if [ $? -eq 0 ]; then
        if echo "$VALGRIND_OUTPUT" | grep -q "All heap blocks were freed"; then
            print_pass "No memory leaks detected"
        else
            print_warning "Valgrind completed but check output for details"
        fi
    else
        print_fail "Memory leaks detected!"
        echo "$VALGRIND_OUTPUT" | grep -A 5 "LEAK SUMMARY"
    fi
fi

# Final summary
print_header "TEST SUMMARY"

echo -e "${BOLD}Total Tests:${NC} $TOTAL_TESTS"
echo -e "${GREEN}${BOLD}Passed:${NC} $PASSED_TESTS"
echo -e "${RED}${BOLD}Failed:${NC} $FAILED_TESTS"

PASS_RATE=$((PASSED_TESTS * 100 / TOTAL_TESTS))
echo -e "\n${BOLD}Pass Rate:${NC} ${PASS_RATE}%"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "\n${GREEN}${BOLD}🎉 ALL TESTS PASSED! You're ready for evaluation!${NC}\n"
    echo -e "${CYAN}Next steps:${NC}"
    echo -e "  1. Review EVALUATION_GUIDE.md for common questions"
    echo -e "  2. Practice explaining your algorithm"
    echo -e "  3. Make sure you can explain thread vs process model"
    echo -e "  4. Be ready to show your code and walk through it"
    exit 0
else
    echo -e "\n${RED}${BOLD}⚠️  SOME TESTS FAILED! Fix issues before evaluation.${NC}\n"
    exit 1
fi
