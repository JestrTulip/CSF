In our implementation of the chat room, we had each of the connections the server
made to other devices as individual detatched threads. Naturally, this could lead
to hazards such as races due to the existance, access, and alteration of certain
objects shared between all the threads. In order to properly handle these hazards
we identified any shared resources that might be altered by the either the sender 
or receivers (critical regions). This included the list of rooms in the server, the 
list of users in each room, and the message queue that each user in the room has. Then, 
we identified all the functions were these shared objects were accessed:
 - find_or_create_room
 - add_member
 - remove_member
 - bradcast_message
 - enqueue
 - dequeue
 Which each had the potential to be accessed by multiple threads, causing irregularities
 in the execution of our program. 

 To ensure that only one thread was accessing these objects at time, we added mutexes
 each of the releant classes (server, room, message_queue) and locked and locked these
 mutexes whenever one of the above functions is called. By doing this, if one of the
 objects is being accessed by a thread, it's associated mutex is locked, and no other 
 thread can lock it/access it until the previously mentioned thread finishes its goal
 and unlocks the mutex. The associated mutexes are initialized and destroyed within the 
 classes constructor and destructor. To prevent any of these functions being deadlocked, 
 we utilized the Guard class, which automatically unlocks the mutex when it is destroyed.
 This is displayed in our implementation of each of the above functions, which make use of  
 these guards to stop a given thread from entering a critical section before the oher has 
 completed. Locks and mutexes were necessary for all sections, as they allowed only one thread to access a 
 shared resource at a given time.

 Additionally to manage the access of the limited resource of messages in the message queue
 for each user, we utilized a semaphore in the message_queue class. This was due to the fact that 
 we needed to utilize multiple threads. This semaphore was associated with the number of messages in the queue. 
 Therefore, when there are no messages in the queue, the semaphore is zero and the reciever thread containing the 
 user of that message_queue is momentarally put to sleep until a message is added, the semaphore is incremented, 
 and the reciever thread is alerted and can access and remove a message from the queue (after using the mutex to lock the
 message queue naturally).
 
