#Assignment 3 directory

This directory contains source code and other files for Assignment 3.

Use this README document to store notes about design, testing, and
questions you have while developing your assignment.

My design is based off the psuedocode given in class, which was to define
variables in the creation of the queue, as well as the psuedocode for push and pop.
For the other functions, they just initialize variables or delete them.

Queue Struct
The defined queue variables are an array of void pointers, 4 integer variables, 
a lock, and 2 conditional variables.

void pointer array: used to hold the items in the queue
integer variables: to keep track of the array size, count of items, and front and back of the queue
lock: to block other threads from accessing the critical section if a thread is already in it
conditional variables: one for if the array is full, other if empty array

Push 
The function first checks if the inputted queue and element are null. If they are, the 
function will return false. Otherwise, the thread will lock the queue's lock. It then checks if the
queue is at the max size. If so, then it will wait for more space to open up by using the full
conditional variable. This releases the lock for other threads to acquire. The function waits until it
gets a signal from pop, which I will describe after this. Once the thread is awakened, it gets the lock
and enters the critical section of the code, where it will place the inputted element into the array,
and update the count, and position of the front and back. Then it will first unlock the lock, and then
signal to the empty conditional variable that there is an item in the queue now. Finally, the function
returns true since it successfully completed.

Pop
Similarly to push, the function checks if the inputted queue and element are null and returns false if 
they are. Otherwise, the thread will lock the queue's lock. It then checks if the queue is empty. If so, 
then it will wait for items to be placed in the queue using the empty conditional variable and releases 
the lock. The function waits for a signal from push. Once the thread is awakened, it gets the lock
and enters the critical section of the code, where it will grab the element at the front of the queue, 
update the count, and position of the front and back. Then it will unlock the lock, and then
signal to the full conditional variable that there is more space in the queue. Finally, the function
returns true since it successfully completed.

New Queue
This function initializes all the variables in the queue_t struct. It also initializes the queue size to
the size given in the parameter.

Delete
Destroys the lock and conditional variables, as well as freeing the space they took. Also frees the void
pointer array and queue struct. Last, it sets the inputted pointer to null.

The design decisions I made were that I decided to use a lock. This was because locks were more understandable
to me than semaphores. I used an array of void pointers so that anything can go into the queue.