#!/bin/bash

set -euo pipefail

ERROR=0
DRY_RUN=0

parse_args() {
	local arg
	while [ "$#" -gt 0 ]
	do
		arg="$1"
		shift
		if [ "$arg" = "--dry-run" ]
		then
			DRY_RUN=1
		elif [ "$arg" = "--help" ] || [ "$arg" = "-h" ]
		then
			cat <<-EOF
			usage: ./scripts/update_abi.sh [OPTIONS]
			options:
			  --dry-run     only show diff that would be applied"
			EOF
		else
			echo "Error: unknown argument $arg" 1>&2
			exit 1
		fi
	done
}

parse_args "$@"

print_ddnet_dir_or_die() {
	if [ -d ../ddnet ]
	then
		echo "../ddnet"
		return 0
	fi
	echo "Error: no ddnet source code found" 1>&2
	exit 1
}

DDNET_DIR="$(print_ddnet_dir_or_die)" || exit 1
TMP_DIR="$(mktemp -d /tmp/antibob_XXXXX)" || exit 1

cleanup() {
	rm -rf "$TMP_DIR"
}

trap cleanup EXIT

move_file() {
	local filepath="$1"
	local src="$DDNET_DIR/src/$filepath"
	local dst="src/ddnet/polybob/$filepath"
	if [ ! -f "$src" ]
	then
		echo "Error: move_file missing file $src" 1>&2
		exit 1
	fi

	local tmpfile="$TMP_DIR/want.txt"
	sed 's/^#include <base/#include <polybob\/base/' "$src" > "$tmpfile"

	if [ "$DRY_RUN" = 0 ]
	then
		if ! mv "$tmpfile" "$dst"
		then
			echo "Error: move_file $src -> $dst failed" 1>&2
			exit 1
		fi
	else
		if diff -u --color=auto "$tmpfile" "$dst"
		then
			ERROR=1
		fi
	fi
}

move_file antibot/antibot_interface.h
move_file antibot/antibot_data.h

exit "$ERROR"

