# Elegram Server

## How does server work?
* Server works in async mode. For every connection it creates 
Session object, which wait requests and send response 
(1 request for every 1 response).

* For every request session create job and push it to job pool using 
boost asio ba::post. Worker threads execute this jobs, extract data 
from database and prepare results for sending response to client 
through network. 

## Database
* Structure of db describes [here](../server/tests/gen_schema.sql)
* In tests we run docker image of postgresql server
* Todo set up pgbouncer for reduce connection resources
* Cause now server have DBConnection factory and create connection 
for every session (lazy connection)


## Protocol
* Server sends all data through TCP protocol
* Messages are serialized and deserialized using [Google Protobuf messages](../protocol/messages.proto)
* Some protocol description
    * Login
        - ––> LoginRequest [email, password]
        - <–– LoginResponse [Contact [user_id, name, email] ]

    * Registration
        - ––> RegisterRequest [name, email, password]
        - <–– StatusResponse [ Accepted / Rejected ]

    * Loading user's chats
        - ––> AllMyChatsRequest
        - <–– ChatsResponse [ List [Chat] ]
    
    * Loading user's contacts
        - ––> AllMyContactsRequest
        - <–– ContactsResponse [ List[Contact] ]
    
    * Loading user's messages
        - ––> MessagesRequest  [ chat_id ]
        - ––> MessagesResponse [ List[Messages] ]
    
    * Sending message to some chat
        - ––> SendMessageRequest [ Message ]
        - <–– StatusResponse [ Accepted / Rejected ]
    
    * Adding contact
        - ––> AddContactRequest [ email]
        - <–– StatusResponse [ Accepted / Rejected ]
    
    * Creating new chat
        - ––> CreateChatRequest [ friend_id ]
        - <–– StatusResponse [ Accepted / Rejected ]

## How to start server
* Add postgresql docker image and start it (look [`docker_start_postgres.sh`](../server/tests/docker_start_postgres.sh))
* run [`gen_data.sh`](../server/tests/gen_data.sh) script for initializing db and crating test data
* Load dependencies and build server (look at [.travis.yml](../.travis.yml) for more details)
* Start server
```bash
./elegram_server --port 8000 --logging true
```

## TODO
-[ ] Send data using secure (SSL) connection
-[ ] ~~Add notification about new incoming message ?~~