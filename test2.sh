#!/bin/bash

# --- CONFIGURATION ---
SERVER="localhost"
CMDSERVER="./bin/ircserv" 
PORT=6667
NICK="AdminChatBot"
CHAN="#norminette"
PASSWORD="pwd123" # Optionnel
LOGDIR="test_logs"

# --- FONCTION D'ENVOI ---
# On utilise une fonction pour formater correctement les messages IRC
send_irc() {
    echo "$1"
    echo "$1" >&3
    sleep 1 # Petit délai pour éviter d'être banni pour flood
}


$CMDSERVER $PORT $PASSWORD > $LOGDIR/server.log 2>&1 &

# --- CONNEXION ---
# Ouverture d'un descripteur de fichier (3) vers le serveur IRC
exec 3<>/dev/tcp/$SERVER/$PORT

# Identification
send_irc "NICK $NICK"
send_irc "USER $NICK 8 * :Bot Bash"

# Attendre un peu que la connexion s'établisse avant de rejoindre
sleep 5

# --- AUTOMATISATION DES COMMANDES ---

# 1. Rejoindre le canal
send_irc "JOIN $CHAN"

# 2. Configurer le TOPIC
send_irc "TOPIC $CHAN :Sujet automatisé par script Shell"

# 3. Configurer les MODES (ex: +t pour restreindre le topic, +n pas de msg externe)
send_irc "MODE $CHAN +tn"

# 4. Inviter un utilisateur (si besoin)
# send_irc "INVITE AmiPseudo $CHAN"

# 5. Envoyer un message de confirmation
send_irc "PRIVMSG $CHAN :L'automatisation du canal est terminée ! [OK]"

# Rester à l'écoute pour garder la connexion active (ou 'exit 0' pour quitter)
cat <&3