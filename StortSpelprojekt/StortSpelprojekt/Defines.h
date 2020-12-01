#pragma once

#define ALIGN16 __declspec(align(16))
#define RELEASE(cptr) if(cptr) { cptr->Release(); cptr = nullptr; }

#define FCAST(T) static_cast<float>(T)
#define DCAST(T) static_cast<double>(T)
#define ICAST(T) static_cast<int>(T)
#define UICAST(T) static_cast<unsigned int>(T)


#define BEFORE_MERGE(msg) std::cout << "BEFORE_MERGE: \"" << msg << "\" @" << __LINE__ << " in " << __FILE__ << "\n";


#define MM_MALLOC_OPERATOR void* operator new(size_t i) { return _mm_malloc(i, 16); }
#define MM_FREE_OPERATOR void operator delete(void* p){ _mm_free(p); }
#define ALIGN16_ALLOC MM_MALLOC_OPERATOR MM_FREE_OPERATOR

#define ASSERT_SHADER(RESULT, eBLOB, wTEXT) \
		if (FAILED(RESULT)) { \
			MessageBox(NULL, wTEXT.c_str(), L"Error compiling shaders. Check output in VS for more information.", MB_OK); \
			if(eBLOB){		\
				OutputDebugStringA((char*)eBLOB->GetBufferPointer()); \
				eBLOB->Release(); \
			}} \

#define MULTITHREAD_PHYSICS FALSE
#define DRAW_COLLIDERS FALSE
#define IMMORTAL FALSE

#define RUNNING_IN_VS TRUE
#define LOAD_FROM_DLL TRUE