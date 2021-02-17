#include <stdio.h>
#include <Windows.h>

void exitOnError(const char* message)
{
	DWORD error = GetLastError();
	if (error)
	{
		printf("[ ERROR %d ]: %s\n",error, message);
		exit(error);
	}
	else
	{
		printf("[ OK ]: %s\n", message);
	}
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: injector <PID> <DLL Path>");
	}
	else
	{
		UINT pathlen = strlen(argv[2]) + 1;
		HANDLE h_process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, atoi(argv[1]));
		exitOnError("OpenProcess");
		LPVOID p_alloc = VirtualAllocEx(h_process, NULL, pathlen, MEM_COMMIT, PAGE_READWRITE);
		exitOnError("VirtualAllocEx");
		WriteProcessMemory(h_process, p_alloc, argv[2], pathlen, 0);
		exitOnError("WriteProcessMemory");
		HMODULE h_kernel32 = LoadLibraryA("kernel32.dll");
		exitOnError("LoadLibraryA");
		FARPROC loadLibrary = GetProcAddress(h_kernel32, "LoadLibraryA");
		exitOnError("GetProcAddress");
		CreateRemoteThread(h_process, NULL, 0, loadLibrary, p_alloc, 0, NULL);
		exitOnError("CreateRemoteThread");
		printf("DLL Loaded!");
	}
	return 0;
}
