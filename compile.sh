#!/bin/sh

name='scb'

# not safe but meh
files=$(ls src/*.c)
progameFiles=$(ls src/program/*.c)


safety=" -D=NAME_CHECK "
rule=" -g -D PROG_NAME=\"$name\" $safety "

compileLine="cc $rule -D=SETUP_EXTERN -Wall -Werror -Wextra  $files $progameFiles -o $name"

#

echo '|' $compileLine '|'

#

$compileLine