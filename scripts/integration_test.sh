#!/bin/bash

set -eu

ANTIBOT_SO=build/libantibot.so
# Can be overriden by user given arg
SERVER_SRC_DIR=../ddnet
CLIENT_BIN=ddnet-headless
# TODO: do not hardcode this and find free port instead
SERVER_PORT=8309
TEST_MARKER=antibob-test-8z712

RESET='\033[0m'
BOLD='\033[1m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'

parse_args() {
	local arg
	local num_pos_args=0
	while true; do
		[ "$#" -gt 0 ] || break

		arg="$1"
		shift

		if [ "${arg::1}" == "-" ]; then
			if [ "$arg" == "--help" ] || [ "$arg" == "-h" ]; then
				cat <<-EOF
					usage: ./scripts/integration_test.sh [OPTION..] [SERVER SRC DIR] [OPTION..]
					description:
					  runs a simple integration test of the antibob antibot module
					  by spinning up a ddnet server and connecting a headless
					  ddnet client to it that will authenticate as admin
					  and then run one antibot command
					options:
					  -h | --help        shows this help page
					server src dir:
					  absolute or relative path to a ddnet source code
					  of the server that will be used in the test
					  the server binary is expected to exist in the build/ directory
					  and it has to be compiled with -DANTIBOT=ON
				EOF
				exit 0
			else
				echo "Error: unexpected option '$arg', see --help"
				exit 1
			fi
		else
			if [ "$num_pos_args" == "0" ]; then
				SERVER_SRC_DIR="$arg"
			else
				echo "Error: unexepected positional argument '$arg'"
				exit 1
			fi
			num_pos_args=$((num_pos_args + 1))
		fi
	done
}

parse_args "$@"

find_one_file() {
	local pattern="$1"
	local f
	local first=1
	local match=""
	while read -r f; do
		if [ "$first" = 0 ]; then
			echo "Error: found more than one file!" 1>&2
			echo "       dir: $(pwd)" 1>&2
			echo "       pattern: $pattern" 1>&2
			exit 1
		fi
		first=0
		match="$f"
	done < <(find . -wholename "$pattern")
	if [ "$match" = "" ] || [ ! -f "$match" ]; then
		echo "Error: file not found!" 1>&2
		echo "       dir: $(pwd)" 1>&2
		echo "       pattern: $pattern" 1>&2
		exit 1
	fi
	printf '%s\n' "$match"
}

download_headless_client() {
	PATH="$HOME/.local/bin:$PATH"
	[ -x "$(command -v "$CLIENT_BIN")" ] && return

	if [ -f ~/.local/bin/$CLIENT_BIN ]; then
		echo "[-] Error: file exits but not in PATH ~/.local/bin/$CLIENT_BIN"
		exit 1
	fi

	echo "[*] $CLIENT_BIN not found, downloading ..."
	mkdir -p "$HOME/.local/bin"
	wget \
		https://github.com/ChillerDragon/ddnet/releases/download/v20.1-headless/DDNet \
		-O ~/.local/bin/$CLIENT_BIN \
		--quiet
	chmod +x ~/.local/bin/$CLIENT_BIN
}

download_headless_client

echo -n "[*] test launching headless client ... "
if ! "$CLIENT_BIN" quit &>/tmp/antibob-clienterr.txt; then
	echo ERROR
	cat /tmp/antibob-clienterr.txt
	exit 1
fi
echo OK

if [ ! -d "$SERVER_SRC_DIR" ]; then
	echo "Error: missing ddnet server src directory $SERVER_SRC_DIR"
	exit 1
fi
if [ ! -d "$SERVER_SRC_DIR"/build ]; then
	echo "Error: missing ddnet server build directory $SERVER_SRC_DIR/build"
	exit 1
fi

cp "$ANTIBOT_SO" "$SERVER_SRC_DIR/build"
cd "$SERVER_SRC_DIR/build"

# TODO: when to clear out the previous run?
mkdir -p antibob_integration_test
cd antibob_integration_test

client_fifo() {
	local cmd="$1"
	printf -- "[*][client][fifo] %b%s%b\n" "$BOLD" "$cmd" "$RESET"
	printf -- '%s\n' "$cmd" >client.fifo
}

server_fifo() {
	local cmd="$1"
	printf -- "[*][server][fifo] %b%s%b\n" "$BOLD" "$cmd" "$RESET"
	printf -- '%s\n' "$cmd" >server.fifo
}

dump_srv_log() {
	local line
	line="$(tail -n1 "server.stdout.txt")"
	echo "[*][server][log] $line"
}

shutdown_all() {
	echo "[*] shutting down server and client .."
	echo "shutdown" >server.fifo
	echo "quit" >client.fifo
	pkill -f "$TEST_MARKER"
}
trap shutdown_all EXIT

rm -f dumps/remote_console_*

cat <<'EOF' >storage.cfg
add_path $CURRENTDIR
add_path $DATADIR
EOF

cat <<EOF >autoexec_server.cfg
sv_name "antibob test server"
sv_register 0
sv_map dm1
sv_rcon_password x
sv_port $SERVER_PORT
EOF

mkdir -p data/maps
cp ../../data/maps/dm1.map data/maps

../DDNet-Server "sv_input_fifo server.fifo;#$TEST_MARKER" >server.stdout.txt 2>server.stderr.txt &
sleep 1

cat <<EOF >client.cfg
cl_input_fifo client.fifo
player_name bob
EOF

"$CLIENT_BIN" "exec client.cfg;connect localhost:$SERVER_PORT;#$TEST_MARKER" >client.stdout.txt 2>client.stderr.txt &
sleep 1

client_fifo "rcon_auth x"
sleep 1
dump_srv_log

client_fifo "rcon antibot dump"
sleep 1
dump_srv_log

client_fifo "dump_remote_console"
sleep 1
dump_srv_log

rcon_logfile="$(find_one_file "./dumps/remote_console_*.txt")"

antibot_lines="$(grep " I antibot: " "$rcon_logfile")"
num_lines="$(echo "$antibot_lines" | wc -l)"

if [ "$num_lines" != 1 ]; then
	printf -- '[*][test] %bantibot dump%b output %bERROR%b\n' "$BOLD" "$RESET" "$RED" "$RESET"
	printf -- '[*][test]  expected 1 antibot output line but instead got %d\n' "$num_lines"
	printf -- '[*][test]  lines:\n'
	cat "$antibot_lines"
	printf -- '[*][test]  full rcon log:\n'
	cat "$rcon_logfile"
	exit 1
fi

# expected output looks like this
# 2026-09-19 10:06:35 I antibot: cid=0 name='bob'

if ! echo "$antibot_lines" | grep -q "I antibot: cid=0 name='bob'"; then
	printf -- '[*][test] %bantibot dump%b output %bERROR%b\n' "$BOLD" "$RESET" "$RED" "$RESET"
	printf -- "[*][test]  expected: I antibot: cid=0 name='bob'\n"
	printf -- "[*][test]       got: %s\n" "$antibot_lines"
	exit 1
fi

printf -- '[*][test] %bantibot dump%b output %bOK%b\n' "$BOLD" "$RESET" "$GREEN" "$RESET"
echo "[*][client][log] $antibot_lines"
