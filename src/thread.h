/*--------------------------------------------------------------*
 | Engine name - thread.h - thread symbolic declarations        |
 | License ++++++++++++++++++++++++                             |
 | posibly GPL or just put the code in the public domain        |
 | written by: Alfredo Luzon                                    |
 | Contact info: @hotmail.com, @gmail.com, website.com          |
 | last modifications by <guy #n> : <modifications>             |
 *--------------------------------------------------------------*/
#ifndef CHESS_THREADS_H
#define CHESS_THREADS_H
#endif

#if defined(_WIN32) || defined(_WIN64)
  #  include <windows.h>
  #  include <process.h>
  #  include <time.h>
  #define THREAD_RETURN_VAL void
  typedef HANDLE THREAD_t;
  typedef HANDLE MUTEX_t;
  #define INIT_MUTEX(m) \
   ((m) = CreateMutex (NULL, FALSE, NULL))
  #define MUTEX_LOCK(m) \
   (WaitForSingleObject((m),INFINITE))
  #define MUTEX_UNLOCK(m) \
   (ReleaseMutex((m)))
  #define THREAD_CREATE(id,attr,fn,data) \
   ((id) = (HANDLE)_beginthread((fn),0,(data)))
  #define THREAD_EXIT(id) \
   (_endthread())
  #define WAIT(id) \
   (WaitForSingleObject((id),INFINITE))

#else

  #  include <pthread.h>
  #define THREAD_RETURN_VAL void *
  typedef pthread_t THREAD_t;
  typedef pthread_mutex_t MUTEX_t;
  #define INIT_MUTEX(m) \
   (pthread_mutex_init(&(m), NULL))
  #define MUTEX_LOCK(m) \
   (pthread_mutex_lock(&(m)))
  #define MUTEX_UNLOCK(m) \
   (pthread_mutex_unlock(&(m)))
  #define THREAD_CREATE(id,attr,fn,data) \
   (pthread_create(&(id),(attr),(fn),(data)))
  #define THREAD_EXIT(id) \
   (pthread_exit(&(id)))
  #define WAIT(id) \
   (pthread_join((id),NULL))
#endif

enum {
	INTERRUPT = 0x01,
	TIMEOUT   = 0x02,
	FINISH    = 0x4
}IterateFlags;

/*Iterative deepening routines*/

int Think(void *);
int Ponder(void *);
THREAD_RETURN_VAL Timer(void *);
THREAD_RETURN_VAL _Timer(void *);
THREAD_RETURN_VAL Iterate(void *);
THREAD_RETURN_VAL InputReader(void *);
extern MUTEX_t StopIterate; /* Global Mutex for stoping iterative deepening */
extern THREAD_t IterativeDeepening;
extern THREAD_t PonderThread;
extern THREAD_t TimerThread;
extern THREAD_t _TimerThread ;
extern THREAD_t InputThread;
extern int flags;
extern int exitXboard;
