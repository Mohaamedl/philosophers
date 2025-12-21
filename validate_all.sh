#!/bin/bash

# Philosophers - Complete Validation Script
# Tests both mandatory and bonus implementations

# Don't exit on error - we want to run all tests

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
PASSED=0
FAILED=0

# Helper function to print status
print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✅ PASS${NC}: $2"
        ((PASSED++))
    else
        echo -e "${RED}❌ FAIL${NC}: $2"
        ((FAILED++))
    fi
}

# Helper function to check for orphan processes
check_orphans() {
    local program=$1
    sleep 1
    local count=$(ps aux | grep -i "$program" | grep -v grep | wc -l)
    if [ $count -eq 0 ]; then
        return 0
    else
        echo "Found $count orphan processes"
        ps aux | grep -i "$program" | grep -v grep
        killall -9 "$program" 2>/dev/null || true
        return 1
    fi
}

echo -e "${BLUE}╔════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║     PHILOSOPHERS - COMPLETE VALIDATION SUITE      ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════╝${NC}"
echo ""

#------------------------------------------------------------------------------
# MANDATORY PART
#------------------------------------------------------------------------------

echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  MANDATORY PART - Thread-based Implementation${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

echo "Building mandatory..."
cd philo
make re > /dev/null 2>&1
print_status $? "Compilation (mandatory)"

echo ""
echo "Running evaluation tests..."

# Test 1: Single philosopher dies at 800ms
echo -n "  Test 1: Single philosopher... "
output=$(timeout 2s ./philo 1 800 200 200 2>/dev/null | tail -1)
if echo "$output" | grep -q "died"; then
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC}"
    ((FAILED++))
fi
check_orphans "philo" > /dev/null

# Test 2: 5 philosophers no deaths
echo -n "  Test 2: 5 philosophers (5s)... "
timeout 5s ./philo 5 800 200 200 > /dev/null 2>&1
if [ $? -eq 124 ]; then  # timeout exit code
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC}"
    ((FAILED++))
fi
check_orphans "philo" > /dev/null

# Test 3: Meal count
echo -n "  Test 3: Meal count (2 philos, 5 meals)... "
timeout 5s ./philo 2 800 200 200 5 > /dev/null 2>&1
result=$?
if [ $result -eq 0 ]; then
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC} (exit code: $result)"
    ((FAILED++))
fi
check_orphans "philo" > /dev/null

# Test 4: No deaths with 4 philosophers
echo -n "  Test 4: 4 philosophers no deaths (5s)... "
timeout 5s ./philo 4 410 200 200 > /dev/null 2>&1
if [ $? -eq 124 ]; then
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC}"
    ((FAILED++))
fi
check_orphans "philo" > /dev/null

echo ""
echo "Running quality checks..."

# Norminette
echo -n "  Norminette... "
norminette include/ src/ > /dev/null 2>&1
print_status $? "Norminette compliance"

# Memory leaks
echo -n "  Memory leaks (valgrind)... "
valgrind --leak-check=full --error-exitcode=1 ./philo 4 410 200 200 5 > /dev/null 2>&1
print_status $? "No memory leaks"

echo ""

cd ..

#------------------------------------------------------------------------------
# BONUS PART
#------------------------------------------------------------------------------

echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  BONUS PART - Process-based Implementation${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

echo "Building bonus..."
cd philo_bonus
make re > /dev/null 2>&1
print_status $? "Compilation (bonus)"

echo ""
echo "Running evaluation tests..."

# Test 1: Single philosopher dies at 800ms
echo -n "  Test 1: Single philosopher... "
output=$(timeout 2s ./philo_bonus 1 800 200 200 2>/dev/null | tail -1)
if echo "$output" | grep -q "died"; then
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC}"
    ((FAILED++))
fi
check_orphans "philo_bonus" > /dev/null

# Test 2: 5 philosophers no deaths
echo -n "  Test 2: 5 philosophers (5s)... "
timeout 5s ./philo_bonus 5 800 200 200 > /dev/null 2>&1
if [ $? -eq 124 ]; then
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC}"
    ((FAILED++))
fi
check_orphans "philo_bonus" > /dev/null

# Test 3: Meal count
echo -n "  Test 3: Meal count (2 philos, 5 meals)... "
timeout 5s ./philo_bonus 2 800 200 200 5 > /dev/null 2>&1
result=$?
if [ $result -eq 0 ]; then
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC} (exit code: $result)"
    ((FAILED++))
fi
check_orphans "philo_bonus" > /dev/null

# Test 4: No deaths with 4 philosophers
echo -n "  Test 4: 4 philosophers no deaths (5s)... "
timeout 5s ./philo_bonus 4 410 200 200 > /dev/null 2>&1
if [ $? -eq 124 ]; then
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC}"
    ((FAILED++))
fi
check_orphans "philo_bonus" > /dev/null

echo ""
echo "Running quality checks..."

# Norminette
echo -n "  Norminette... "
norminette include/ src/ > /dev/null 2>&1
print_status $? "Norminette compliance"

# Memory leaks
echo -n "  Memory leaks (valgrind)... "
valgrind --leak-check=full --trace-children=yes --error-exitcode=1 ./philo_bonus 4 410 200 200 5 > /dev/null 2>&1
print_status $? "No memory leaks"

# Check for leaked semaphores
echo -n "  Semaphore cleanup... "
leaked=$(ls /dev/shm/sem.* 2>/dev/null | grep -E "(forks|write|dead)" | wc -l)
if [ $leaked -eq 0 ]; then
    echo -e "${GREEN}✅${NC}"
    ((PASSED++))
else
    echo -e "${RED}❌${NC} ($leaked leaked semaphores)"
    ((FAILED++))
fi

echo ""

cd ..

#------------------------------------------------------------------------------
# SUMMARY
#------------------------------------------------------------------------------

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}  VALIDATION SUMMARY${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

TOTAL=$((PASSED + FAILED))
echo "Total Tests: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}╔════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║   🎉 ALL TESTS PASSED! 🎉             ║${NC}"
    echo -e "${GREEN}║   Ready for evaluation!               ║${NC}"
    echo -e "${GREEN}╚════════════════════════════════════════╝${NC}"
    exit 0
else
    echo -e "${RED}╔════════════════════════════════════════╗${NC}"
    echo -e "${RED}║   ⚠️  SOME TESTS FAILED ⚠️             ║${NC}"
    echo -e "${RED}║   Please review and fix issues        ║${NC}"
    echo -e "${RED}╚════════════════════════════════════════╝${NC}"
    exit 1
fi
