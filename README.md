# ft_irc — Quick usage

## Server

Start the server binary with a listening port and a server password:

```
./ircserv <port> <password>
```

Example:

```
./ircserv 6667 pwd123
```

## Client (irssi)

Use `irssi` (or another IRC client) to connect. Example connection command:

```
irssi -c <hostname> -p <port> -w <password> -n <nickname>
```

Example connecting to the server started above:

```
irssi -c localhost -p 6667 -w pwd123 -n user
```

You can also use the client command `/connect` if supported by your client:

```
/connect localhost 6667 pwd123 user
```

## Quick Commands & Examples

- **Join channel:**

	- Command: ``/join <channel>``
	- Example: ``/join #test``

- **Leave channel:**

	- Command: ``/part <channel>``
	- Example: ``/part #test``

- **Private message:**

	- Command: ``/msg <nickname> <message>``
	- Example: ``/msg user Hello there!``

- **Set topic:**

	- Command: ``/topic <channel> <topic>``
	- Example: ``/topic #test Welcome to the test channel``

- **Channel modes:**

	- Command: ``/mode <channel> <mode> [args]``
	- Examples:
		- Invite-only: ``/mode #test +i`` and unset with ``/mode #test -i``
		- Topic restriction (only ops set topic): ``/mode #test +t`` and unset with ``/mode #test -t``
		- Key/password: set ``/mode #test +k secret123`` and remove with ``/mode #test -k secret123``
		- Operator privilege: give op ``/mode #test +o user`` and remove op ``/mode #test -o user``
		- User limit: set ``/mode #test +l 5`` and remove with ``/mode #test -l``

- **Kick user:**

	- Command: ``/kick <channel> <nickname> [reason]``
	- Example: ``/kick #test user reason``

- **Invite user:**

	- Command: ``/invite <nickname> <channel>``
	- Example: ``/invite user #test``

- **Quit:**

	- Command: ``/quit [reason]``
	- Example: ``/quit Bye everyone!``

## Quick Start (example session)

1. Launch server:

	 ```
	 ./ircserv 6667 pwd123
	 ```

2. Connect with `irssi`:

	 ```
	 irssi -c localhost -p 6667 -w pwd123 -n user
	 ```

3. Join a channel and set a topic:

	 ```
	 /join #test
	 /topic #test Welcome to the test channel
	 ```

4. Set a mode (example: invite-only) and invite a user:

	 ```
	 /mode #test +i
	 /invite otheruser #test
	 ```

5. Send a private message and quit:

	 ```
	 /msg otheruser Hello there!
	 /quit Bye everyone!
	 ```
