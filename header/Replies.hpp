/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:00:00 by witong            #+#    #+#             */
/*   Updated: 2026/02/02 11:39:43 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string>
#include <sstream>

// Welcome Replies
// 001 - Sent upon successful registration
#define RPL_WELCOME(nick, user, host, servername) (":" + servername + " 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!" + user + "@" + host)

// 002 - Part of typical registration burst
#define RPL_YOURHOST(nick, servername) (":" + servername + " 002 " + nick + " :Your host is " + servername + ", running version 1.0")

// 003 - Part of typical registration burst
#define RPL_CREATED(nick, servername) (":" + servername + " 003 " + nick + " :This server was created Jan 2026")

// 004 - Part of typical registration burst (servername version user_modes channel_modes)
#define RPL_MYINFO(nick, servername) (":" + servername + " 004 " + nick + " " + servername + " 1.0  itklo")

// Commands Replies
// 324 - Response to MODE command when no arguments are given (viewing mode)
#define RPL_CHANNELMODEIS(nick, channel, modes) (":localhost 324 " + nick + " " + channel + " " + modes)

// 331 - Response to TOPIC command when no topic is set
#define RPL_NOTOPIC(nick, channel) (":localhost 331 " + nick + " " + channel + " :No topic is set")

// 332 - Response to TOPIC command when a topic is set
#define RPL_TOPIC(nick, channel, topic) (":localhost 332 " + nick + " " + channel + " :" + topic)

// 341 - Returned when a client is successfully invited
#define RPL_INVITING(nick, channel, invited_nick) (":localhost 341 " + nick + " " + invited_nick + " " + channel)

// 353 - Reply to NAMES (also sent after JOIN). Usernames should be space-separated.
// Start of the list with "=" for public, "@" for secret, "*" for private channels
#define RPL_NAMREPLY(nick, channel, users) (":localhost 353 " + nick + " = " + channel + " :" + users)

// 366 - Termination of RPL_NAMREPLY
#define RPL_ENDOFNAMES(nick, channel) (":localhost 366 " + nick + " " + channel + " :End of /NAMES list")

// Error Replies
// 401 - Used when a nickname parameter doesn't exist
#define ERR_NOSUCHNICK(nick, target) (":localhost 401 " + nick + " " + target + " :No such nick/channel")

// 403 - Used when a channel parameter doesn't exist
#define ERR_NOSUCHCHANNEL(nick, channel) (":localhost 403 " + nick + " " + channel + " :No such channel")

// 411 - No recipient given (PRIVMSG/NOTICE)
#define ERR_NORECIPIENT(nick, command) (":localhost 411 " + nick + " :No recipient given (" + command + ")")

// 412 - No text to send (PRIVMSG/NOTICE)
#define ERR_NOTEXTTOSEND(nick) (":localhost 412 " + nick + " :No text to send")

// 421 - Command unknown
#define ERR_UNKNOWNCOMMAND(nick, command) (":localhost 421 " + nick + " " + command + " :Unknown command")

// 431 - No nickname given (NICK command)
#define ERR_NONICKNAMEGIVEN(nick) (":localhost 431 " + nick + " :No nickname given")

// 432 - Erroneous nickname (invalid characters)
#define ERR_ERRONEUSNICKNAME(nick, new_nick) (":localhost 432 " + nick + " " + new_nick + " :Erroneous nickname")

// 433 - Nickname already in use
#define ERR_NICKNAMEINUSE(nick, new_nick) (":localhost 433 " + nick + " " + new_nick + " :Nickname is already in use")

// 441 - User not in channel (KICK, PART, etc)
#define ERR_USERNOTINCHANNEL(nick, target, channel) (":localhost 441 " + nick + " " + target + " " + channel + " :They aren't on that channel")

// 442 - Client not on channel (TOPIC, INVITE, KICK, etc)
#define ERR_NOTONCHANNEL(nick, channel) (":localhost 442 " + nick + " " + channel + " :You're not on that channel")

// 443 - User already on channel (INVITE)
#define ERR_USERONCHANNEL(nick, target, channel) (":localhost 443 " + nick + " " + target + " " + channel + " :is already on channel")

// 451 - You have not registered
#define ERR_NOTREGISTERED(nick) (":localhost 451 " + nick + " :You have not registered")

// 461 - Not enough parameters
#define ERR_NEEDMOREPARAMS(nick, command) (":localhost 461 " + nick + " " + command + " :Not enough parameters")

// 462 - Already registered (USER/PASS command)
#define ERR_ALREADYREGISTERED(nick) (":localhost 462 " + nick + " :Unauthorized command (already registered)")

// 464 - Password incorrect
#define ERR_PASSWDMISMATCH(nick) (":localhost 464 " + nick + " :Password incorrect")

// 471 - Channel is full (limit +l)
#define ERR_CHANNELISFULL(nick, channel) (":localhost 471 " + nick + " " + channel + " :Cannot join channel (+l)")

// 472 - Unknown mode char
#define ERR_UNKNOWNMODE(nick, char_mode, channel) (":localhost 472 " + nick + " " + char_mode + " :is unknown mode char to me for " + channel)

// 473 - Invite only channel (+i)
#define ERR_INVITEONLYCHAN(nick, channel) (":localhost 473 " + nick + " " + channel + " :Cannot join channel (+i)")

// 475 - Bad channel key (+k)
#define ERR_BADCHANNELKEY(nick, channel) (":localhost 475 " + nick + " " + channel + " :Cannot join channel (+k)")

// 482 - Chanop privileges needed (KICK, MODE, TOPIC, INVITE)
#define ERR_CHANOPRIVSNEEDED(nick, channel) (":localhost 482 " + nick + " " + channel + " :You're not channel operator")

// 501 - Unknown MODE flag (for user modes)
#define ERR_UMODEUNKNOWNFLAG(nick) (":localhost 501 " + nick + " :Unknown MODE flag")

// 502 - Cannot change mode for other users
#define ERR_USERSDONTMATCH(nick) (":localhost 502 " + nick + " :Cannot change mode for other users")

// 315 - End of WHO list
#define RPL_ENDOFWHO(nick, target) (":localhost 315 " + nick + " " + target + " :End of /WHO list")

// 352 - WHO
// #define RPL_WHOREPLY(nick, user, host, server, nick_target, status, hopcount) (":localhost 352 " + nick + " " + user + " " + host + " " + server + " " + nick_target + " " + status + " " + hopcount + " :0 " + user)

// 352 - WHO with different parameter name
#define RPL_WHOREPLY(nick,target, user, host, server, nick_target, status, hopcount) (":localhost 352 " + nick + " "+target+ " " + user + " " + host + " " + server + " "+ nick_target + " " + status + " " + hopcount + " :0 "+ target +" "+ user)



#endif
