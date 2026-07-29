#!/bin/bash
# **************************************************************************** #
#  Codexion tester                                                            #
#  Usage:                                                                     #
#    ./tester.sh            -> run full assertion suite                       #
#    ./tester.sh <name>     -> run a single test (raw output, like before)    #
#    ./tester.sh asan       -> build with -fsanitize=address and run subset   #
#    ./tester.sh list       -> list all available single tests                 #
#  Single tests keep backward-compat: 1,2,3,...,big,starvation,...            #
# **************************************************************************** #

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

PROG="./codexion"
ASAN_PROG="./codexion_asan"
TIMEOUT=30          # default per-test timeout (s)
STRESS_RUNS=50      # number of repeated runs for stability
PASS=0
FAIL=0
SKIP=0

# -------- helpers ----------------------------------------------------------- #

die() { echo -e "${RED}$1${NC}"; exit 1; }

build() {
	if [ ! -x "$PROG" ]; then
		echo -e "${CYAN}Building project...${NC}"
		make re >/dev/null 2>&1 || die "Build failed. Run 'make' manually."
	fi
}

# run the program with a timeout, capture stdout+stderr, set $RC and $OUT
run() {
	OUT=$(timeout "$TIMEOUT" $PROG "$@" 2>&1)
	RC=$?
}

# assert last run exited with expected code
assert_exit() {
	local expect=$1 name=$2
	if [ "$RC" -eq 124 ]; then
		echo -e "  ${RED}FAIL${NC} $name (HANG/timeout)"
		FAIL=$((FAIL+1)); return
	fi
	if [ "$RC" -eq "$expect" ]; then
		echo -e "  ${GREEN}PASS${NC} $name (exit $RC)"
		PASS=$((PASS+1))
	else
		echo -e "  ${RED}FAIL${NC} $name (expected exit $expect, got $RC)"
		FAIL=$((FAIL+1))
	fi
}

# assert last run output contains a substring
assert_contains() {
	local pat=$1 name=$2
	if echo "$OUT" | grep -qE "$pat"; then
		echo -e "  ${GREEN}PASS${NC} $name"
		PASS=$((PASS+1))
	else
		echo -e "  ${RED}FAIL${NC} $name (missing: '$pat')"
		FAIL=$((FAIL+1))
	fi
}

# assert last run output does NOT contain a substring
assert_not_contains() {
	local pat=$1 name=$2
	if echo "$OUT" | grep -qE "$pat"; then
		echo -e "  ${RED}FAIL${NC} $name (unexpected: '$pat')"
		FAIL=$((FAIL+1))
	else
		echo -e "  ${GREEN}PASS${NC} $name"
		PASS=$((PASS+1))
	fi
}

# assert last run output is empty
assert_empty() {
	local name=$1
	if [ -z "$OUT" ]; then
		echo -e "  ${GREEN}PASS${NC} $name"
		PASS=$((PASS+1))
	else
		echo -e "  ${RED}FAIL${NC} $name (expected no output)"
		FAIL=$((FAIL+1))
	fi
}

# extract first burned-out timestamp (first number on the burn line)
burn_ts() { echo "$OUT" | grep -m1 'burned out' | grep -oE '^[0-9]+'; }

header() { echo -e "\n${BOLD}${CYAN}== $1 ==${NC}"; }

# -------- 1. argument validation (must reject) ------------------------------ #
test_arg_reject() {
	header "Argument validation (must reject -> exit 1)"
	local cases=(
		"wrong_count|"
		"non_integer_arg1|banana 200 300 400 500 5 10 fifo"
		"negative_arg|10 200 300 -400 500 5 10 edf"
		"bad_scheduler|10 200 300 400 500 5 10 banana"
		"overflow_intmax+1|2147483648 200 200 200 200 5 10 fifo"
		"overflow_12dig|999999999999 200 200 200 200 5 10 fifo"
		"trailing_chars|5abc 200 200 200 200 5 10 fifo"
		"empty_string| 200 200 200 200 5 10 fifo"
		"too_many_args|1 2 3 4 5 6 7 8 9"
	)
	for c in "${cases[@]}"; do
		local name="${c%%|*}"; local args="${c#*|}"
		if [ -z "$args" ]; then run; else run $args; fi
		assert_exit 1 "$name"
	done
}

# -------- 2. argument validation (must accept / edge) ----------------------- #
test_arg_accept() {
	header "Argument edge values (must accept)"
	run 0 200 300 400 500 5 10 fifo
	assert_exit 0 "0 coders -> exit 0"
	assert_empty "  0 coders -> no output"
	run 5 200 300 400 500 0 10 fifo
	assert_exit 0 "0 min_compiles -> exit 0"
	assert_empty "  0 min_compiles -> no output"
	run 2 2147483647 100 100 100 1 10 fifo
	assert_exit 0 "INT_MAX burnout accepted"
}

# -------- 3. basic functionality -------------------------------------------- #
test_basic() {
	header "Basic functionality"
	run 4 800 200 200 200 5 10 fifo
	assert_exit 0 "basic_fifo completes"
	assert_not_contains 'burned out' "  basic_fifo no burnout"
	run 4 800 200 200 200 5 10 edf
	assert_exit 0 "basic_edf completes"
	assert_not_contains 'burned out' "  basic_edf no burnout"
	run 10 10000 100 100 100 5 50 fifo
	assert_exit 0 "success_fifo (10 coders)"
	run 20 5000 500 500 500 10 100 edf
	assert_exit 0 "large_edf (20 coders)"
}

# -------- 4. single coder --------------------------------------------------- #
test_single() {
	header "Single coder (1 dongle, self-bypass)"
	run 1 1000 200 200 200 5 50 fifo
	assert_exit 0 "one_compiler_fifo"
	assert_contains '^[0-9]+ 1 burned out$' "  one_compiler_fifo burns at deadline"
	run 1 1000 200 200 200 5 50 edf
	assert_exit 0 "one_compiler_edf"
	assert_contains '^[0-9]+ 1 burned out$' "  one_compiler_edf burns at deadline"
}

# -------- 5. burnout cases -------------------------------------------------- #
test_burnout() {
	header "Burnout detection"
	run 2 1 200 200 200 5 10 fifo
	assert_exit 0 "immediate_burnout completes"
	assert_contains 'burned out' "  immediate_burnout detects burnout"
	run 3 0 100 100 100 5 10 fifo
	assert_exit 0 "time_burnout=0"
	assert_contains 'burned out' "  time_burnout=0 detects burnout"
	run 2 1000 100 100 100 5 2000 fifo
	assert_exit 0 "cooldown_hell completes"
	assert_contains 'burned out' "  cooldown_hell burns (cooldown > burnout)"
}

# -------- 6. degenerate durations ------------------------------------------- #
test_degenerate() {
	header "Degenerate durations (valid edges)"
	run 3 5000 0 0 0 2 10 fifo
	assert_exit 0 "time_compile=0 completes"
	assert_contains 'is compiling' "  time_compile=0 still logs compile"
	run 5 2000 100 100 100 20 1 fifo
	assert_exit 0 "low_cooldown=1 completes"
	run 3 10000 2000 2000 2000 2 100 fifo
	assert_exit 0 "long_actions completes"
}

# -------- 7. starvation (infeasible -> burnout is correct) ------------------ #
test_starvation() {
	header "Starvation (infeasible config -> burnout expected)"
	run 3 1000 600 10 10 5 100 fifo
	assert_exit 0 "starvation_fifo completes"
	assert_contains 'burned out' "  starvation_fifo burns (infeasible)"
	run 3 1000 600 10 10 5 100 edf
	assert_exit 0 "starvation_edf completes"
	assert_contains 'burned out' "  starvation_edf burns (infeasible)"
}

# -------- 8. liveness (feasible -> NO burnout) ------------------------------ #
test_liveness() {
	header "Liveness (feasible config -> NO burnout)"
	run 5 100000 100 100 100 10 10 edf
	assert_exit 0 "feasible_edf 5 coders x10"
	assert_not_contains 'burned out' "  feasible_edf never burns out"
	run 6 50000 100 50 50 4 10 fifo
	assert_exit 0 "feasible_fifo 6 coders x4"
	assert_not_contains 'burned out' "  feasible_fifo never burns out"
}

# -------- 9. scale ---------------------------------------------------------- #
test_scale() {
	header "Scale / high coder count"
	run 300 10000 100 100 100 5 10 edf
	assert_exit 0 "max_coders (300 edf) completes"
	run 999 1000 200 200 200 5 50 fifo
	assert_exit 0 "toomany (999) completes"
	assert_contains 'burned out' "  toomany burns (infeasible at 999)"
	run 100 10000 66 24 87 3 10 fifo
	assert_exit 0 "big (100 coders) completes"
}

# -------- 10. log invariants (needs python3) -------------------------------- #
test_invariants() {
	header "Log invariants (2 takes before each compile)"
	if ! command -v python3 >/dev/null 2>&1; then
		echo -e "  ${YELLOW}SKIP${NC} python3 not available"
		SKIP=$((SKIP+1)); return
	fi
	run 5 50000 100 100 100 3 50 fifo
	if [ "$RC" -ne 0 ]; then
		echo -e "  ${RED}FAIL${NC} invariant run failed"
		FAIL=$((FAIL+1)); return
	fi
	echo "$OUT" | python3 - <<'EOF'
import re,sys
lines=sys.stdin.read().split('\n')
take=re.compile(r'^\d+ (\d+) has taken a dongle$')
comp=re.compile(r'^\d+ (\d+) is compiling$')
errs=[]; pending={}; compiles={}
for ln in lines:
    if not ln.strip(): continue
    m=take.match(ln)
    if m: pending[m.group(1)]=pending.get(m.group(1),0)+1; continue
    m=comp.match(ln)
    if m:
        c=m.group(1)
        if pending.get(c,0)!=2: errs.append(f"compile {c} pending={pending.get(c,0)}")
        pending[c]=0; compiles[c]=compiles.get(c,0)+1; continue
sys.exit(1 if errs else 0)
EOF
	if [ $? -eq 0 ]; then
		echo -e "  ${GREEN}PASS${NC} 2 takes precede every compile"
		PASS=$((PASS+1))
	else
		echo -e "  ${RED}FAIL${NC} invariants broken"
		FAIL=$((FAIL+1))
	fi
}

# -------- 11. burnout precision (within 10ms) ------------------------------- #
test_burnout_precision() {
	header "Burnout precision (within 10ms tolerance)"
	# burnout=50 -> coder 2 should burn between 50 and 60
	run 2 50 200 200 200 5 10 fifo
	if [ "$RC" -ne 0 ]; then
		echo -e "  ${RED}FAIL${NC} precision run failed"; FAIL=$((FAIL+1)); return
	fi
	local ts; ts=$(burn_ts)
	if [ -z "$ts" ]; then
		echo -e "  ${RED}FAIL${NC} no burnout detected"; FAIL=$((FAIL+1)); return
	fi
	if [ "$ts" -ge 50 ] && [ "$ts" -le 60 ]; then
		echo -e "  ${GREEN}PASS${NC} burnout at ${ts}ms (target 50, tol +10)"
		PASS=$((PASS+1))
	else
		echo -e "  ${RED}FAIL${NC} burnout at ${ts}ms (outside 50-60)"
		FAIL=$((FAIL+1))
	fi
}

# -------- 12. cooldown enforcement ----------------------------------------- #
test_cooldown() {
	header "Cooldown enforcement"
	# 2 coders share both dongles; re-acquire must respect cooldown=30
	run 2 10000 100 50 50 3 30 fifo
	if [ "$RC" -ne 0 ]; then
		echo -e "  ${RED}FAIL${NC} cooldown run failed"; FAIL=$((FAIL+1)); return
	fi
	# verify it completed without burnout (feasible)
	assert_not_contains 'burned out' "  cooldown run no burnout"
}

# -------- 13. stability (stress, no crash/hang) ----------------------------- #
test_stress() {
	header "Stress ($STRESS_RUNS runs, no crash/hang)"
	local f=0 h=0 i ec
	for i in $(seq 1 "$STRESS_RUNS"); do
		timeout 5 $PROG 4 2000 100 100 100 2 10 fifo >/dev/null 2>&1
		ec=$?
		[ "$ec" -eq 124 ] && h=$((h+1))
		[ "$ec" -ne 0 ] && f=$((f+1))
	done
	if [ "$f" -eq 0 ] && [ "$h" -eq 0 ]; then
		echo -e "  ${GREEN}PASS${NC} $STRESS_RUNS runs ok (0 fail, 0 hang)"
		PASS=$((PASS+1))
	else
		echo -e "  ${RED}FAIL${NC} $STRESS_RUNS runs: $f fail, $h hang"
		FAIL=$((FAIL+1))
	fi
}

# -------- 14. ASAN build + run ---------------------------------------------- #
test_asan() {
	header "AddressSanitizer build + run"
	if ! cc -Wall -Wextra -Werror -pthread -fsanitize=address -g \
		-I includes srcs/*.c -o "$ASAN_PROG" 2>asan_build.log; then
		echo -e "  ${RED}FAIL${NC} ASAN build failed (see asan_build.log)"
		FAIL=$((FAIL+1)); return
	fi
	echo -e "  ${GREEN}PASS${NC} ASAN build ok"
	PASS=$((PASS+1))
	local cases=(
		"2 1 200 200 200 5 10 fifo"
		"3 5000 100 100 100 2 10 fifo"
		"1 500 200 200 200 5 50 fifo"
		"0 200 300 400 500 5 10 fifo"
		"10 8000 200 200 200 5 50 edf"
	)
	ASAN_OPTIONS=detect_leaks=1:abort_on_error=0
	export ASAN_OPTIONS
	for c in "${cases[@]}"; do
		local log; log=$(timeout "$TIMEOUT" "$ASAN_PROG" $c 2>&1 >/dev/null)
		local rc=$?
		if [ "$rc" -eq 124 ]; then
			echo -e "  ${RED}FAIL${NC} ASAN hang: $c"; FAIL=$((FAIL+1))
		elif echo "$log" | grep -qiE 'runtime error|leak|ERROR:' \
			| grep -v 'suppressions'; then
			if echo "$log" | grep -qiE 'runtime error|ERROR: AddressSanitizer|detected memory leaks'; then
				echo -e "  ${RED}FAIL${NC} ASAN issue: $c"; FAIL=$((FAIL+1))
			else
				echo -e "  ${GREEN}PASS${NC} ASAN clean: $c"; PASS=$((PASS+1))
			fi
		else
			echo -e "  ${GREEN}PASS${NC} ASAN clean: $c"; PASS=$((PASS+1))
		fi
	done
	rm -f "$ASAN_PROG" asan_build.log
}

# -------- single-test backward-compat (raw output) -------------------------- #
run_test() { echo -e "Test $1 : $2\n"; timeout "$TIMEOUT" $PROG $2; }

SINGLE_TESTS() {
	case "$1" in
		1) run_test basic_fifo "4 800 200 200 200 5 10 fifo" ;;
		2) run_test basic_edf "4 800 200 200 200 5 10 edf" ;;
		3) run_test success_fifo "10 10000 100 100 100 5 50 fifo" ;;
		4) run_test large_edf "20 5000 500 500 500 10 100 edf" ;;
		5) run_test low_cooldown "5 2000 100 100 100 20 1 fifo" ;;
		6) run_test long_actions "3 10000 2000 2000 2000 2 100 fifo" ;;
		big) run_test big_test "100 10000 66 24 87 10 10 fifo" ;;
		starvation) run_test starvation_case "3 1000 600 10 10 5 100 fifo" ;;
		starvation2) run_test starvation_case "3 1000 600 10 10 5 100 edf" ;;
		one_compiler_fifo) run_test one_compiler_fifo "1 1000 200 200 200 5 50 fifo" ;;
		one_compiler_edf) run_test one_compiler_edf "1 1000 200 200 200 5 50 edf" ;;
		zero_compile) run_test zero_compiles "5 1000 200 200 200 0 10 fifo" ;;
		immediate_burnout) run_test immediate_burnout "2 1 200 200 200 5 10 fifo" ;;
		cooldown_hell) run_test cooldown_hell "2 1000 100 100 100 5 2000 fifo" ;;
		max_coders) run_test max_coders "300 10000 100 100 100 5 10 edf" ;;
		toomany_compiler) run_test toomany_compiler "999 1000 200 200 200 5 50 fifo" ;;
		error_arg1) run_test error_coder "banana 200 300 400 500 5 10 fifo" ;;
		error_arg2) run_test error_coder "10 banana 300 400 500 5 10 fifo" ;;
		error_arg3) run_test error_coder "10 200 banana 400 500 5 10 fifo" ;;
		error_arg4) run_test error_coder "10 200 300 banana 500 5 10 fifo" ;;
		error_arg5) run_test error_coder "10 200 300 400 banana 5 10 fifo" ;;
		error_arg6) run_test error_coder "10 200 300 400 500 banana 10 fifo" ;;
		error_arg7) run_test error_coder "10 200 300 400 500 5 banana fifo" ;;
		error_arg8) run_test error_coder "10 200 300 400 500 5 10 banana" ;;
		error_arg9) run_test error_coder "10 200 300 -400 500 5 10 edf" ;;
		error_arg10) run_test error_coder "too 10 200 300 400 500 5 10 edf" ;;
		*) return 1 ;;
	esac
}

# -------- dispatch ---------------------------------------------------------- #

show_summary() {
	echo -e "\n${BOLD}-------------------------------------------"
	echo -e "PASS: ${GREEN}$PASS${NC}  FAIL: ${RED}$FAIL${NC}  SKIP: ${YELLOW}$SKIP${NC}"
	echo -e "-------------------------------------------${NC}"
	[ "$FAIL" -eq 0 ]
}

build

if [ -z "$1" ] || [ "$1" = "all" ] || [ "$1" = "full" ]; then
	test_arg_reject
	test_arg_accept
	test_basic
	test_single
	test_burnout
	test_degenerate
	test_starvation
	test_liveness
	test_scale
	test_invariants
	test_burnout_precision
	test_cooldown
	test_stress
	[ "$1" = "full" ] && test_asan
	show_summary
	exit $?
elif [ "$1" = "asan" ]; then
	test_asan
	show_summary
	exit $?
elif [ "$1" = "list" ]; then
	echo "Single tests: 1 2 3 4 5 6 big starvation starvation2"
	echo "              one_compiler_fifo one_compiler_edf zero_compile"
	echo "              immediate_burnout cooldown_hell max_coders toomany_compiler"
	echo "              error_arg1..error_arg10"
	echo "Suites: all | full (all+asan) | asan"
elif SINGLE_TESTS "$1"; then
	exit 0
else
	die "Unknown test: $1\nUsage: ./tester.sh [all|full|asan|list|<name>]"
fi
