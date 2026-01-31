#!/bin/bash

SERVER="127.0.0.1"  # Ton serveur ft_irc
PORT="6667"
NICK="testbot$$"    # Nick unique avec PID
PASS="pwd123"      # Si ton serveur a un PASS
CHANNELS="#ft_irc,#test"
CONFIG="/tmp/irc_$$.fifo"  # FIFO pour commandes interactives

# Crée FIFO pour envoyer des commandes pendant l'exécution
mkfifo "$CONFIG"

echo "Connexion IRC à $SERVER:$PORT..."

# Lance la connexion en fond (gère PING/PONG auto)
(
  echo "NICK $NICK"
  echo "USER $NICK 0 * :Test Bot pour ft_irc"
  echo "PASS $PASS"
  echo "JOIN $CHANNELS"
  tail -f "$CONFIG" | nc "$SERVER" $PORT | while read MSG; do
    echo "$MSG"
    case "$MSG" in
      *PING*) echo "PONG ${MSG#PING}" >&3 ;;  # Répond aux PING
      *PRIVMSG*:#*) ;;  # Ignore messages channel pour log clean
      *PRIVMSG* ) echo "[$(date '+%H:%M')] $MSG" ;;  # Log PRIVMSG
    esac
  done
) 3>"$CONFIG" &

sleep 2  # Attends welcome

# Actions automatisées (ajoute tes fonctions ici)
echo "PRIVMSG $CHANNELS :Test auto depuis script Bash" >&3
echo "PRIVMSG $NICK :Message privé à moi-même" >&3
echo "MODE $CHANNELS +nt" >&3  # Test MODE
echo "KICK $CHANNELS $NICK :Test kick auto" >&3  # Se kick lui-même

read -p "Appuie sur Entrée pour quitter..."
echo "QUIT :Fin du test" >&3
rm "$CONFIG"
wait