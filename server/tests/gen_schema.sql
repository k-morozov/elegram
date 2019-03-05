-------------------------------------------------------------------------------
DROP TABLE IF EXISTS ClientToContact;
DROP TABLE IF EXISTS ClientToChat;
DROP TABLE IF EXISTS Message;
DROP TABLE IF EXISTS Chat;
DROP TABLE IF EXISTS Client;

-------------------------------------------------------------------------------
CREATE TABLE Client (
	id SERIAL PRIMARY KEY,
  name TEXT NOT NULL,
	email TEXT NOT NULL UNIQUE,
	password_hash bytea NOT NULL -- SHA256
);

-- there must be one message, but many chats - for every client it's own chat, and he can be connected
-- to this chat or not

-------------------------------------------------------------------------------
-- one chat for all
CREATE TABLE Chat (
  id SERIAL PRIMARY KEY -- global id, incremental
);

-------------------------------------------------------------------------------
-- many clients can have many chats
CREATE TABLE ClientToChat (
                            client_id INTEGER REFERENCES Client(id) ON DELETE CASCADE ,
                            chat_id   INTEGER REFERENCES Chat (id) ON DELETE CASCADE,
                            title     TEXT NOT NULL
);

--------------------------------------------------------------------------------
CREATE TABLE Message (
  id BIGSERIAL PRIMARY KEY,
  sender_id INTEGER REFERENCES Client(id),
  chat_id INTEGER REFERENCES Chat(id) ON DELETE CASCADE, -- TODO may be remove ON DELETE later
	send_time TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
  mesg TEXT NOT NULL
);

--------------------------------------------------------------------------------
-- many clients can have many contacts
CREATE TABLE ClientToContact (
  client_id INTEGER REFERENCES Client(id) ON DELETE CASCADE, -- delete link if one of them deleted
                                                             -- from Client table
  friend_id INTEGER REFERENCES Client(id) ON DELETE CASCADE,
  UNIQUE (client_id, friend_id)
);
--------------------------------------------------------------------------------
