
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <mqueue.h>

#include "../../../srv/log/skyfend_log.h"
#include "../../../common/tools/skyfend_tools.h"
#include "../sysport.h"

#define SYSPORT_TASK_MSG_SIZE		(10)

typedef struct sysport_task_func
{
	void 		(*pv_func)(void* pvArg, void* pvInfo, uint32_t uSize);
	uint32_t	uSize;
	char		acInfo[0];
}sysport_task_func_t;

typedef struct sysport_task_module
{
	char 		*strName;
	void		*pvArg;
	mqd_t 		mq;
	pthread_t 	pid;
}sysport_task_module_t;

static void *sysport_task_pthread( void* pvModule )
{
	struct sysport_task_module *psModule = (struct sysport_task_module*)pvModule;
	
	for( ; ; )
	{
		struct sysport_task_func* psFunc;
		struct mq_attr attr;
		mq_getattr( psModule->mq, &attr );
		int count = mq_receive( psModule->mq, (char*)(&psFunc), attr.mq_msgsize, NULL);

		// if( sizeof(struct sysport_task_func*) == count )
		if( sizeof(struct sysport_task_func*) == count && psFunc)//cdx
		{			
			if(psFunc &&  NULL == psFunc->pv_func )
			{
				break;
			}
			else if(psFunc)
			{
				psFunc->pv_func( psModule->pvArg, psFunc->acInfo, psFunc->uSize );
			}
			if(psFunc)
			skyfend_free( psFunc );
		}
		else
		{
			skyfend_log_info( "no command" );
			if(psFunc) {
				skyfend_free( psFunc );
			}
		}
	}

	skyfend_log_debug( "task %d delete, arg %p", psModule->pid, psModule->pvArg );

	mq_close( psModule->mq );
	mq_unlink( psModule->strName );
	skyfend_free( psModule );

	return NULL;
}

int sysport_task_init( void )
{
	return 0;
}

void* sysport_task_create( char *strName, void* pvArg )
{
	struct sysport_task_module* psModule = skyfend_malloc(sizeof(struct sysport_task_module));

	if( psModule )
	{
		psModule->strName	= strName;
		psModule->pvArg		= pvArg;
	
		struct mq_attr attr;
		attr.mq_maxmsg = SYSPORT_TASK_MSG_SIZE;
		attr.mq_msgsize = sizeof(struct sysport_task_func*);
		psModule->mq = mq_open( psModule->strName, O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr );
		if( 0 > psModule->mq )
		{
			skyfend_log_error( "task create mq err" );
			skyfend_free( psModule );
			psModule = NULL;
		}
		else
		{
			if( 0 > pthread_create(&(psModule->pid), NULL, sysport_task_pthread, psModule) )
			{
				skyfend_log_error( "task create pthread err" );
			
				mq_close( psModule->mq );
				mq_unlink( psModule->strName );
				skyfend_free( psModule );
				psModule = NULL;
			}
			else
			{
				skyfend_log_info( "task create %d", psModule->pid );
			}
		}
	}
	else
	{
		skyfend_log_error( "task create malloc err" );
	}
	
	return psModule;
}

int sysport_task_delete( void* pvModule )
{
	int eRet = -1;

	if( pvModule )
	{
		void* pvInfo = sysport_task_malloc( pvModule, 0 );
		if( pvInfo )
		{
			eRet = sysport_task_func( pvModule, NULL, pvInfo );
		}
	}

	return eRet;
}

void* sysport_task_malloc( void* pvModule, uint32_t uSize )
{
	void* pvData = NULL;

	if(pvModule) {
		struct sysport_task_module *psModule = (struct sysport_task_module*)pvModule;
		struct sysport_task_func* psFunc = skyfend_malloc( sizeof(struct sysport_task_func) + uSize );
		(void)psModule;

		if( psFunc )
		{
			psFunc->uSize = uSize;
			pvData = (void*)(&(psFunc->acInfo));
		}
		else
		{
			skyfend_log_error( "task malloc err" );
		}
	}

	return pvData;
}

int sysport_task_func( void* pvModule, void (*pv_func)(void* pvArg, void* pvInfo, uint32_t uSize), void* pvInfo )
{
	int eRet = -1;
	
	if( pvModule && pvInfo )
	{
		struct sysport_task_module *psModule = (struct sysport_task_module*)pvModule;
		struct sysport_task_func* psFunc = (struct sysport_task_func *)((char *)pvInfo - (((size_t)&((struct sysport_task_func*)0)->acInfo)));
		psFunc->pv_func = pv_func;

		struct mq_attr attr;
		mq_getattr( psModule->mq, &attr );
		eRet = mq_send( psModule->mq, (char*)(&psFunc), attr.mq_msgsize, 0 );
		if( 0 > eRet )
		{
			skyfend_log_error( "task send func %p err", pv_func );
			skyfend_free( psFunc );
		}
	}

	return eRet;
}
