#include "Windows.h"
#include <assert.h>

//Thunkȡ��ATL��Դ��
class WndProcThunk {
public:
	WndProcThunk() {
		m_thunk  = (_WndProcStdCallThunk*)VirtualAlloc(NULL, sizeof(_WndProcStdCallThunk),
			MEM_RESERVE | MEM_COMMIT,
			PAGE_EXECUTE_READWRITE);
	};
	~WndProcThunk() {
		VirtualFree(m_thunk, sizeof(_WndProcStdCallThunk), MEM_RELEASE);
	}


	BOOL Init(
		_In_ DWORD_PTR proc,
		_In_opt_ void *pThis) {
		return m_thunk->Init(proc,pThis);
	}
	WNDPROC GetWndProcThunk() {return (WNDPROC)m_thunk->GetCodeAddress();}
protected:


#if defined(_WIN64)

#pragma pack(push,2)
struct _WndProcStdCallThunk
{
	USHORT  RcxMov;         // mov rcx, pThis
	ULONG64 RcxImm;         //
	USHORT  RaxMov;         // mov rax, target
	ULONG64 RaxImm;         //
	USHORT  RaxJmp;         // jmp target
	BOOL Init(
		_In_ DWORD_PTR proc,
		_In_opt_ void *pThis)
	{
		RcxMov = 0xb948;          // mov rcx, pThis
		RcxImm = (ULONG64)pThis;  //
		RaxMov = 0xb848;          // mov rax, target
		RaxImm = (ULONG64)proc;   //
		RaxJmp = 0xe0ff;          // jmp rax
		FlushInstructionCache(GetCurrentProcess(), this, sizeof(_WndProcStdCallThunk));
		return TRUE;
	}
	//some thunks will dynamically allocate the memory for the code
	void* GetCodeAddress()
	{
		return this;
	}
};
#pragma pack(pop)


#elif(_WIN32)


#pragma pack(push,1)
	struct _WndProcStdCallThunk
	{
		DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
		DWORD   m_this;         //
		BYTE    m_jmp;          // jmp WndProc
		DWORD   m_relproc;      // relative jmp
		BOOL Init(
			_In_ DWORD_PTR proc,
			_In_opt_ void* pThis)
		{
			m_mov = 0x042444C7;  //C7 44 24 0C
			m_this = PtrToUlong(pThis);
			m_jmp = 0xe9;
			m_relproc = DWORD((INT_PTR)proc - ((INT_PTR)this + sizeof(_WndProcStdCallThunk)));
			// write block from data cache and
			//  flush from instruction cache
			FlushInstructionCache(GetCurrentProcess(), this, sizeof(_WndProcStdCallThunk));
			return TRUE;
		}
		//some thunks will dynamically allocate the memory for the code
		void* GetCodeAddress()
		{
			return this;
		}
	};
#pragma pack(pop)




#else
#error Only X86, X64 supported
#endif
_WndProcStdCallThunk* m_thunk;
};

