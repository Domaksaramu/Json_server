Thread::Thread () {}
Thread::~Thread () {}

int Thread::run () { return 0; } /* 어짜피 오버라이딩 될 부분 */
void *Thread::run_ (void *pthis_) // 중간에서 run을 다시 호출해줄 녀석
{
	Thread *pthis = (Thread *)pthis_;
	pthis->setRetval (pthis->run ());
	pthread_exit (NULL);
}
void Thread::start ()
{ /* 여기서부터 스레드가 생성됩니다. */
	pthread_create (&myThread, NULL, Thread::run_, (void *)this);
	/* Thread::run_은 앞서 Class내에서 static으로
	 *        선언했으므로 가능합니다. 그리고 this는 엄연한 값이므로 가능. */
}

int Thread::join ()
{
	pthread_join (myThread, NULL);
	return retval;
}
