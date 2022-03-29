#include <windows.h>
#include <iostream>
int exec(char *cmd, char *output, DWORD maxbuffer)
{
    HANDLE readHandle;
    HANDLE writeHandle;
    HANDLE stdHandle;
    DWORD bytesRead;
    DWORD retCode;
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    ZeroMemory(&sa,sizeof(SECURITY_ATTRIBUTES));
    ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si,sizeof(STARTUPINFO));

    sa.bInheritHandle=true;
    sa.lpSecurityDescriptor=NULL;
    sa.nLength=sizeof(SECURITY_ATTRIBUTES);
    si.cb=sizeof(STARTUPINFO);
    si.dwFlags=STARTF_USESHOWWINDOW;
    si.wShowWindow=SW_HIDE;

    if (!CreatePipe(&readHandle,&writeHandle,&sa,NULL))
    {
        OutputDebugString("cmd: CreatePipe failed!\n");
        return 0;
    }

    stdHandle=GetStdHandle(STD_OUTPUT_HANDLE);

    if (!SetStdHandle(STD_OUTPUT_HANDLE,writeHandle))
    {
        OutputDebugString("cmd: SetStdHandle(writeHandle) failed!\n");
        return 0;
    }

    if (!CreateProcess(NULL,cmd,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
    {
        OutputDebugString("cmd: CreateProcess failed!\n");
        return 0;
    }

    GetExitCodeProcess(pi.hProcess,&retCode);
    while (retCode==STILL_ACTIVE)
    {
        GetExitCodeProcess(pi.hProcess,&retCode);
    }

    if (!ReadFile(readHandle,output,maxbuffer,&bytesRead,NULL))
    {
        OutputDebugString("cmd: ReadFile failed!\n");
        return 0;
    }
    output[bytesRead]='\0';

    if (!SetStdHandle(STD_OUTPUT_HANDLE,stdHandle))
    {
        OutputDebugString("cmd: SetStdHandle(stdHandle) failed!\n");
        return 0;
    }

    if (!CloseHandle(readHandle))
    {
        OutputDebugString("cmd: CloseHandle(readHandle) failed!\n");
    }
    if (!CloseHandle(writeHandle))
    {
        OutputDebugString("cmd: CloseHandle(writeHandle) failed!\n");
    }

    return 1;
}

//int main() {
//
//    char PingOutput[4048];
//    exec("ping -n 1 -w 2000 192.168.1.14", PingOutput, 4048);
//    std::cout << PingOutput;
//    return EXIT_SUCCESS;
//}
