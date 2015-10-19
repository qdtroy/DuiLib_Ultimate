#ifndef MacroDefine_h__
#define MacroDefine_h__


#define NAMESPACE_BEGIN(n)		namespace n {
#define NAMESPACE_END(n)		}	

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(hHandle)					{if((hHandle)!=NULL)::CloseHandle((hHandle));(hHandle)=NULL;};
#endif

#ifndef SAFE_FREE_LIBRARY
#define SAFE_FREE_LIBRARY(hHandle)					{if((hHandle)!=NULL)::FreeLibrary((hHandle));(hHandle)=NULL;};
#endif

#ifndef SAFE_DELETE_OBJECT
#define SAFE_DELETE_OBJECT(hHandle)					{if((hHandle)!=NULL)::DeleteObject((hHandle));(hHandle)=NULL;};
#endif

#ifndef IS_SAFE_HANDLE
#define IS_SAFE_HANDLE(hWnd)						(((hWnd)!=NULL)&&((hWnd)!=INVALID_HANDLE_VALUE))
#endif

#ifndef IS_INVALID_HANDLE
#define IS_INVALID_HANDLE(hWnd)						(((hWnd)==NULL)||((hWnd)==INVALID_HANDLE_VALUE))
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(pMem)							{if((pMem)!=NULL){delete((pMem));(pMem)=NULL;}};
#endif

#ifndef SAFE_DELETE_LIST
#define SAFE_DELETE_LIST(pMem)						{if((pMem)!= NULL) {delete[]((pMem));(pMem)=NULL;}};
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(pMem)								{if((pMem)!=NULL){free((pMem));(pMem)=NULL;}};
#endif

#endif // MacroDefine_h__

