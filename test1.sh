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
CHANNEL="#toto"
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
        echo "JOIN $CHANNEL"
        sleep 1
        echo "WHO *"  # Vérifie membres
        echo "TOPIC $CHANNEL :Premier topic de test"
        sleep 1
     echo "TOPIC $CHANNEL"  # Get
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
sleep 5





