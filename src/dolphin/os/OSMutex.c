/* Dolphin SDK OS: mutexes. The game never locks an SDK mutex, so only the hook the thread code
 * calls when a thread exits remains: it releases every mutex the thread still holds and wakes
 * the threads waiting on each one. fn_80378568 is OSWakeupThread. */
#include <dolphin/os.h>

void fn_80378568(OSThreadQueue *queue);

void __OSUnlockAllMutex(OSThread *thread) {
    OSMutex *mutex;
    OSMutex *next;

    while (thread->queueMutex.head != NULL) {
        mutex = thread->queueMutex.head;
        next = mutex->link.next;
        if (next == NULL) {
            thread->queueMutex.tail = NULL;
        } else {
            next->link.prev = NULL;
        }
        thread->queueMutex.head = next;

        mutex->count = 0;
        mutex->thread = NULL;
        fn_80378568(&mutex->queue);
    }
}
