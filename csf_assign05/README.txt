Contributions
--------------
MS1
Arijit and Taran paircoded for the entire assignment.

MS2
Arijit and Taran continued to paircode for MS2.

Our Approach To Thread Synchronization:

We start by using pthread_create to create our threads and then immediately detatched them
with pthread_detach. We do this because detatching threads allows us to let them run independently,
then the system will automatically reclaim the resources upon terminaion. Out communication_data struct
encapsulates the data for each thread, giving them a Server and Connection instance which facilitates
passing thread specific data. We also have a mutex m_lock in our Server class to help protect shared
resources such as the m_rooms map. Here we use a Guard object in our find_or_create_room to ensure 
mutual exclusion between threads accessing/modifying the shared m_rooms map. We also handle connections
through our threads, with each thread handling its own Connection object. The threads also play a role in
Room and User Queue management. Rooms are managed by the Server through find_or_create_room which ensures 
that rooms are thread safe. In User Queue, each thread/user has its own message queue as shown in 
chat_with_receiver, so that it can view/access. Finally, the destructor in communication_data allows for
the Connection to be deleted when threads finish, stopping memory leakage.