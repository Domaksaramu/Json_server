#include <pthread.h>
class Thread
{
	private:
		pthread_t *myThread;
		int retval;
	public:
		Thread ();
		~Thread ();
		virtual int run ();
		static void *run_ (void *);
		void start ();
		int join ();
		void setRetval (int r) { retval = r; }
};
