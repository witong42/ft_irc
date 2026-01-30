#!/bin/bash

# Usage: ./test_ft_irc.sh <port> <password>
# Suppose que ft_irc est compilé et dans le PATH

if [ $# -ne 2 ]; then
    echo "Usage: $0 <port> <password>"
    exit 1
fi

PORT="6667"
PASS="pwd123"
SERVER="./bin/ircserv"  # Change si ton binaire s'appelle autrement
CHANNEL="#test"
LOGDIR="test_logs"
mkdir -p $LOGDIR

echo "=== Test ft_irc : KICK, INVITE, TOPIC, MODE ==="
echo "Port: $PORT, Password: $PASS"
echo "Logs dans $LOGDIR/"

# Tue les anciens serveurs au cas où
pkill -f ./bin/ircserv || true
sleep 1

# Lance le serveur en arrière-plan et capture stdout/stderr
$SERVER $PORT $PASS > $LOGDIR/server.log 2>&1 &
SERVER_PID=$!
sleep 3  # Attendre connexion

if ! nc -z localhost $PORT 2>/dev/null; then
    echo "ERREUR: Serveur non accessible sur localhost:$PORT"
    kill $SERVER_PID 2>/dev/null
    exit 1
fi

function connect_client {
    local nick=$1
    local log=$LOGDIR/client_${nick}.log
    {
        echo "NICK $nick"
        echo "USER $nick 0 * :$nick@localhost"
        sleep 1
        echo "PASS $PASS"
        sleep 1
    } | nc -C localhost $PORT | tee $log
}

function expect_log {
    local log=$1
    local pattern=$2
    if grep -q "$pattern" $log; then
        echo "✓ OK: $pattern"
    else
        echo "✗ KO: '$pattern' non trouvé dans $log"
        cat $log
    fi
}

# Test 1: Connexions de base
echo -e "\n## Test 1: Connexions base"
connect_client alice &
connect_client bob &
connect_client charlie &
sleep 5

# Test 2: JOIN #test
echo -e "\n## Test 2: JOIN $CHANNEL"
{
    echo "JOIN $CHANNEL"
    sleep 1
    echo "NAMES $CHANNEL"  # Vérifie membres
} | nc -C localhost $PORT | tee $LOGDIR/join.log
sleep 2

expect_log $LOGDIR/client_alice.log "JOIN.*$CHANNEL"
expect_log $LOGDIR/client_bob.log "JOIN.*$CHANNEL"

# Test 3: TOPIC (set et get)
echo -e "\n## Test 3: TOPIC"
{
    echo "JOIN $CHANNEL"
    sleep 1
    echo "TOPIC $CHANNEL :Premier topic de test"
    sleep 1
    echo "TOPIC $CHANNEL"  # Get
} | nc -C localhost $PORT | tee $LOGDIR/topic.log
sleep 2

