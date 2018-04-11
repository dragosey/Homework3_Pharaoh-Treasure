#!/bin/bash

#------------------------------------------------------------------------------
# 
#   CHECKER Tema 3 SDA-AB, 2017
#
#   Author: Iulian-Razvan Matesica <iulian.matesica@gmail.com>
#
#------------------------------------------------------------------------------

# Exec name
EXEC_NAME="comoara"

# Teste Normale
FILE_TEST_DAT="date/date%d.in"
FILE_TEST_REF="rezultate/rezultate%d.ref"
FILE_TEST_OUT="out/out%d.out"
NUM_TESTS=10

# Format: "x=a,y=b,z=c..."
# x, y, z are test indexes
# Each test with an index less or equal to x gets a points.
# Each test with an index less or equal to y gets b points.
POINTS_TEST="0=0,10=10"

# Teste Bonus
FILE_BONUS_DAT="date/datebonus%d.in"
FILE_BONUS_REF="rezultate/rezultatebonus%d.ref"
FILE_BONUS_OUT="out/outbonus%d.out"
NUM_BONUS=4
POINTS_BONUS="2=5,4=10"

# Total score
SCORE_TESTS=0
SCORE_BONUS=0

# Delimitor
DELIM="-------------------------------------------------------------"
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# Points:
# Test 	0 		=> exemplul din descrierea temei (0pt)
# Test  1		=> doar C1 			(10pt)
# Teste 2 - 3	=> doar C1 si C3 	(20pt)
# Teste 4 - 5	=> doar C1 si C2	(20pt)
# Teste 6 - 8	=> matrice mai mare	(30pt)
# Teste 9 - 10	=> numere mari		(20pt)
# Bonus  1 - 2	=> matrice mai mica	(10pt)
# Bonus  3 - 4	=> matrice mai mare	(20pt)

points=0

function run_test {
	points=0
	msg=""
	color=$RED

	# Convert from Points string to array
	p_text=`echo $5 | tr ',' ' '`
	p_arr=($p_text)

	for x in "${p_arr[@]}";
	do
	        tidx=`echo $x | cut -d'=' -f1`
	        tpts=`echo $x | cut -d'=' -f2`
	
        	if (( $1 <= $tidx ));
        	then
			points=$tpts
        	        break
        	fi
	done

	# Okay, so this test will get $points if passed.
	timeout 2 ./$EXEC_NAME $2 $3 > /dev/null 2>&1 | cat
	
	diff -Z -q $3 $4 > /dev/null 2>&1
	if [ $? -eq 0 ];
	then
		msg="PASSED"
		color=$GREEN
	else
		msg="FAILED"
		color=$RED
		points=0
	fi

	printf "Test %2d ................................. ${color}%6s${NC} (+%2dpt)\n" $i $msg $points
}

function run_tests {
	for i in `seq 0 1 $1`
	do
		if [ $i -eq 0 ] && [ $6 -eq 1 ];
		then #exista test0 numai la NORMALE, nu si BONUS
			continue;
		fi
		printf -v file_date $2 $i
		printf -v file_out $3 $i
		printf -v file_ref $4 $i
		run_test $i $file_date $file_out $file_ref $5
		
		if [ $6 -eq 0 ];
		then
			# Normal test
			SCORE_TESTS=$(($SCORE_TESTS+$points))
		else
			# Bonus test
			SCORE_BONUS=$(($SCORE_BONUS+$points))
		fi
	done
}

function run_normal_tests {
	echo $DELIM
	echo "[TESTE NORMALE]"
	run_tests $NUM_TESTS $FILE_TEST_DAT $FILE_TEST_OUT $FILE_TEST_REF "$POINTS_TEST" 0

	echo
	printf "TOTAL ........................................... %3spt\n" $SCORE_TESTS 
	echo
}

function run_bonus_tests {
	echo $DELIM
	echo "[TESTE BONUS]"
	run_tests $NUM_BONUS $FILE_BONUS_DAT $FILE_BONUS_OUT $FILE_BONUS_REF "$POINTS_BONUS" 1

	echo
	printf "TOTAL BONUS ..................................... %3spt\n" $SCORE_BONUS
	echo
}

function show_total {
	echo $DELIM
	echo "[NOTA FINALA]"
	
	echo
	printf "NOTA FINALA ..................................... %3spt\n" $(($SCORE_BONUS+$SCORE_TESTS))
	echo	
}

# Prepare output directory
rm -rf out/
mkdir out

# Build 
make clean
make

# Run normal tests
echo $DELIM
echo "Executabil = '$EXEC_NAME'"

run_normal_tests
run_bonus_tests
show_total

