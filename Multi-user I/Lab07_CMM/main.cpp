//Collin Munson
//ASYNC FTP CLIENT

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#include <winsock2.h>

#include <string.h>    
#include <time.h>
#include <stdio.h>
#include <direct.h>    
#include <io.h>
#include <stdlib.h>

#include "resource.h"

//#ifndef IDC_STATIC
//#define IDC_STATIC -1
//#endif

#define WSA_ASYNC 1000

#define MTU_SIZE    1460
#define CMD_SIZE    128
#define RPLY_SIZE   MTU_SIZE
#define MAXNULPUT   1048576

#define BUF_SIZE 1024
#define INPUT_SIZE 8192

#define MAXHOSTNAME 255
#define MAXADDRSTR 16
#define MAXUSERNAME 64
#define MAXPASSWORD 32
#define MAXFILENAME 64

#define SOCKADDR_LEN sizeof(struct sockaddr)

enum { CWD = 1, DELE, PASS, PORT, RETR, STOR, TYPE, USER,
ABOR, LIST, PWD, QUIT };

// Ftp commmand strings 
LPSTR aszFtpCmd[] = {
  "",    "CWD", "DELE", "PASS","PORT","RETR","STOR",
  "TYPE","USER","ABOR","LIST","PWD",  "QUIT"
};;

//----------- Application states -----------
#define NOT_CONNECTED    0
#define CTRLCONNECTED    2
#define DATACONNECTED    4

//------------ Global variables ------------

char szAppName[] = "Async FTP Client";

BOOL nAppState = NOT_CONNECTED;             // Application State 
                                   
BOOL bToNul = FALSE;                // Get to NUL device file 
BOOL bFromNul = FALSE;              // Put from NUL device file 
BOOL bIOBeep = FALSE;               // Beep on FD_READ, FD_WRITE
BOOL bDebug = FALSE;                // Debug output to WinDebug 
BOOL bReAsync = TRUE;              // Call WSAAsyncSelect after accept()
BOOL bLogFile = TRUE;              // Write Cmds and Replies to logfile 

SOCKET hCtrlSock = INVALID_SOCKET;           // Ftp control socket 
SOCKET hLstnSock = INVALID_SOCKET;           // Listening data socket
SOCKET hDataSock = INVALID_SOCKET;           // Connected data socket

char szHost[MAXHOSTNAME] = {0};   // Remote host name or address
char szUser[MAXUSERNAME] = {0};   // User ID
char szPWrd[MAXPASSWORD] = {0};   // User password 

SOCKADDR_IN stCLclName;     // Control socket name (local client)
SOCKADDR_IN stCRmtName;     //                     (remote server)
SOCKADDR_IN stDLclName;     // Data socket name (local client)          
SOCKADDR_IN stDRmtName;     //                  (remote server) 
                              
char achInBuf  [INPUT_SIZE];/* Network input data buffer */
char achOutBuf [INPUT_SIZE];/* Network output buffer */
char szFtpRply [RPLY_SIZE] = {0}; /* Ftp reply (input) buffer */
char szDataFile[MAXFILENAME] = {0};/* Filename */
char szFtpCmd  [CMD_SIZE] = {0};  /* Ftp command buffer */
char achRplyBuf[BUF_SIZE];  /* Reply display buffer */

typedef struct stFtpCmd 
{
	int nFtpCmd;                   /* Ftp command value */
	char szFtpParm[CMD_SIZE];       /* Ftp parameter string */
}FTPCMD;

#define MAX_CMDS 4
/* first one (index=0) is awaiting a reply
 * second (index=1) is next to be sent, etcetera */ 
FTPCMD astFtpCmd[MAX_CMDS]; /* Ftp command queue */
int nQLen;                  /* Number of entries in Ftp cmd queue */
 
int nFtpRplyCode;           /* Ftp reply code from server */
int iNextRply;              /* Index to next reply string */
int iLastRply;

HFILE hDataFile = HFILE_ERROR;            /* File handle for open data file */
LONG lStartTime;            /* Start time for data transfer */
LONG lByteCount;

char szLogFile[] = "ac_ftp.log";           /* Ftp command and reply log file */
HFILE hLogFile = HFILE_ERROR;

WSADATA stWSAData;              /* WinSock DLL Info */

char *aszWSAEvent[] = {         /* For error messages */
  "unknown FD_ event",
  "FD_READ",
  "FD_WRITE",
  "FD_OOB",
  "FD_ACCEPT",
  "FD_CONNECT",
  "FD_CLOSE"
};

char achTempBuf [BUF_SIZE]={0};  // Screen I/O data buffer and such 
char szTempFile []="delete.me";  // Temporary work file 

HWND hWinMain;                   // Main window (dialog) handle 
HINSTANCE hInst;                 // Instance handle 

int nAddrSize = sizeof(SOCKADDR);



//------------- Function prototypes --------------
int CALLBACK Dlg_Main(HWND,UINT,UINT,LPARAM); // Dialog procedures
int CALLBACK Dlg_Login(HWND,UINT,UINT,LPARAM);
int CALLBACK Dlg_File(HWND,UINT,UINT,LPARAM);
int CALLBACK Dlg_Options(HWND,UINT,UINT,LPARAM);

SOCKET InitCtrlConn(PSOCKADDR_IN, HWND, u_int);  // Control connection
int QueueFtpCmd(int, LPSTR);
int  SendFtpCmd(void);
void AbortFtpCmd(void);
int  RecvFtpRply(SOCKET, LPSTR, int);
void ProcessFtpRply(LPSTR, int);

SOCKET InitDataConn(PSOCKADDR_IN, HWND, u_int);  // Data connection 
SOCKET AcceptDataConn(SOCKET, PSOCKADDR_IN);
long SendData(SOCKET*, HFILE, int);
int  RecvData(SOCKET, HFILE, LPSTR, int);
void EndData(void);
void not_connected(void);                        // Utility functions
int  CloseFtpConn(SOCKET*, LPSTR, int, HWND); 

LONG GetHostID ();
u_long GetAddr (LPSTR szHost);

BOOL GetLclDir(LPSTR szTempFile);
HFILE CreateLclFile (LPSTR szFileName);
BOOL CALLBACK Dlg_File(HWND hDlg, UINT msg, UINT wParam, LPARAM lParam);

BOOL CALLBACK Dlg_About (HWND hDlg, UINT msg, UINT wParam, LPARAM lParam);

int GetBuf(SOCKET hSock, int nBigBufSize, int nOptval);


//--------------------------------------------------------------------
//  Function: WinMain()
//
//  Description: 
//     initialize WinSock and open main dialog box
//
//--------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR  lpszCmdLine, int nCmdShow)
{
	MSG msg;
    int nRet;

    lpszCmdLine   = lpszCmdLine;   // avoid warning 
    hPrevInstance = hPrevInstance;
    nCmdShow      = nCmdShow;
                      
    hInst = hInstance;	// save instance handle 
                                                                
    //-------------initialize WinSock DLL------------
    nRet = WSAStartup(MAKEWORD(2,0), &stWSAData);
    // WSAStartup() returns error value if failed (0 on success)
    if (nRet != 0) 
	{    
		MessageBox(0, "Error with WSAStartup()", "Dammit", 0);
    } 
	else 
	{
          
		DialogBox (hInst, MAKEINTRESOURCE(AC_FTP), NULL, Dlg_Main);
    
		//---------------release WinSock DLL--------------
		nRet = WSACleanup();
		if (nRet == SOCKET_ERROR)
		{
		}
    }    
        
    return 1;
} /* end WinMain() */

/*--------------------------------------------------------------------
 * Function: Dlg_Main()
 *
 * Description: Do all the message processing for the main dialog box
 */
BOOL CALLBACK Dlg_Main(HWND hDlg, UINT msg, UINT wParam, LPARAM lParam)
{                      
    int nAddrSize = sizeof(SOCKADDR);
    WORD WSAEvent, WSAErr;
    SOCKET hSock;
    BOOL bOk, bRet = FALSE;
    int  nRet;
    LONG lRet;
   
	switch (msg) 
	{
		case WSA_ASYNC+1:
			/*------------------------------------------------- 
			 * Data socket async notification message handlers 
			 *-------------------------------------------------*/ 
			hSock = (SOCKET)wParam;                 /* socket */
			WSAEvent = WSAGETSELECTEVENT (lParam);  /* extract event */
			WSAErr   = WSAGETSELECTERROR (lParam);  /* extract error */
			/* if error, display to user (listen socket should not have
			 *  an error, but some WinSocks incorrectly post it) */
			if ((WSAErr) && (hSock == hDataSock))  
			{
				int i,j;
				for (i=0, j=WSAEvent; j; i++, j>>=1); /* convert bit to index */
				//WSAperror(WSAErr,aszWSAEvent[i], hInst);
				/* fall-through to call reenabling function for this event */
			}
			switch (WSAEvent) 
			{
				case FD_READ:
					if (bIOBeep)
						MessageBeep(0xFFFF);
					if (hDataSock != INVALID_SOCKET) 
					{
					  /* Receive file data or directory list */
					  RecvData(hDataSock, hDataFile, achInBuf, INPUT_SIZE);
					}
					break;
				case FD_ACCEPT:
					if (hLstnSock != INVALID_SOCKET) 
					{
						/* Accept the incoming data connection request */
						hDataSock = AcceptDataConn(hLstnSock, &stDRmtName);
						nAppState |= DATACONNECTED;
						/* Close the Listening Socket */
						closesocket(hLstnSock);
						hLstnSock = INVALID_SOCKET;
						lStartTime = GetTickCount();
						/* Data transfer should begin with FD_WRITE or FD_READ.  We 
						* fall through to jumpstart sends since FD_WRITE is not 
						* always implemented correctly */
					}
				case FD_WRITE:
					 /* Send file data */
					if (astFtpCmd[0].nFtpCmd == STOR) 
					{
						lRet = SendData(&hDataSock, hDataFile, MTU_SIZE);
					}
					break;
			  case FD_CLOSE:                    /* Data connection closed */
				if (hSock == hDataSock) 
				{
				  /* Read any remaining data into buffer and close connection */  
				  CloseFtpConn(&hDataSock, (astFtpCmd[0].nFtpCmd != STOR) ? achInBuf : (LPSTR)0, INPUT_SIZE, hDlg);
				  EndData ();
				}
				break;
			  default:
				 break;
			} /* end switch(WSAEvent) */
			break;
        
      case WSA_ASYNC: 
        /*----------------------------------------------------- 
         * Control socket async notification message handlers 
         *-----------------------------------------------------*/ 
        WSAEvent = WSAGETSELECTEVENT (lParam);  /* extract event */
        WSAErr   = WSAGETSELECTERROR (lParam);  /* extract error */
        if (WSAErr) 
		{ /* if error, display to user */
          int i,j;
          for (i=0, j=WSAEvent; j; i++, j>>=1); /* convert bit to index */
			//WSAperror(WSAErr,aszWSAEvent[i], hInst);
          /* fall-through to call reenabling function for this event */
        }
        hSock = (SOCKET)wParam;                  
        switch (WSAEvent) 
		{
          case FD_READ:
             if (!iNextRply) 
			 {
                /* Receive reply from server */
                iLastRply = RecvFtpRply(hCtrlSock, szFtpRply, RPLY_SIZE);
             }
            if (iLastRply && (iLastRply != SOCKET_ERROR)) 
			{
                /* Display the reply Message */
                GetDlgItemText (hWinMain, IDC_REPLY, achRplyBuf, 
                  RPLY_SIZE-strlen(szFtpRply));
                wsprintf (achTempBuf, "%s%s", szFtpRply, achRplyBuf);
                SetDlgItemText (hWinMain, IDC_REPLY, achTempBuf);

		        /* Save index to next reply (if there is one) */
                nRet = strlen(szFtpRply);
                if (iLastRply > nRet+2) 
				{
                  iNextRply = nRet+3;
                  /* Adjust if reply only had LF (no CR) */
                  if (szFtpRply[nRet+2])
                    iNextRply = nRet+2;
                }
             }

            /* Figure out what to do with reply based on last command */
            ProcessFtpRply (szFtpRply, RPLY_SIZE);
            break;
          case FD_WRITE:
            /* Send command to server */
            if (astFtpCmd[1].nFtpCmd)
              SendFtpCmd();
            break;
          case FD_CONNECT:
            /* Control connected at TCP level */
            nAppState = CTRLCONNECTED;
            wsprintf(achTempBuf, "Server: %s", szHost);
            SetDlgItemText (hDlg, IDC_SERVER, achTempBuf);
            SetDlgItemText (hDlg, IDC_STATUS, "Status: connected");
            break;
          case FD_CLOSE:
            if (nAppState & CTRLCONNECTED) 
			{
              nAppState = NOT_CONNECTED;        /* Reset app state */
              AbortFtpCmd();
              if (hCtrlSock != INVALID_SOCKET)  /* Close control socket */
                CloseFtpConn(&hCtrlSock, (PSTR)0, 0, hDlg);
              SetDlgItemText (hDlg, IDC_SERVER, "Server: none");
              SetDlgItemText (hDlg, IDC_STATUS, "Status: not connected");
            }
            break;
          default:
             break;
        } /* end switch(WSAEvent) */
        break;

      case WM_COMMAND:
        switch (wParam) 
		{
          case IDC_CONNECT:
             /* If we already have a socket open, tell user to close it */
             if (nAppState & CTRLCONNECTED) 
			 {
                 MessageBox (hDlg,"Close the active connection first",
                    "Can't Connect", MB_OK | MB_ICONASTERISK);
             } 
			 else 
			 {
             
               /* Prompt user for server and login user information */
               bOk = DialogBox (hInst, MAKEINTRESOURCE(IDD_SERVER),
                      hDlg, Dlg_Login);
    
               if (bOk) 
			   {
                 /* Check the destination address and resolve if necessary */
                 stCRmtName.sin_addr.s_addr = GetAddr(szHost);
                 if (stCRmtName.sin_addr.s_addr == INADDR_ANY) {
               
                   /* Tell user to enter a host */
                   wsprintf(achTempBuf, 
                     "Sorry, server %s is invalid.  Try again", szHost);
                   MessageBox (hDlg, achTempBuf,
                     "Can't connect!", MB_OK | MB_ICONASTERISK);
                 } 
				 else 
				 {
                 
                   /* Initiate connect attempt to server */
                   hCtrlSock = InitCtrlConn(&stCRmtName, hDlg, WSA_ASYNC);
                 }
               }
             }
             break;
                     
           case IDC_CLOSE:
             if (nAppState & CTRLCONNECTED) 
			 {
               /* Set application state so nothing else is processed */
               nAppState = NOT_CONNECTED;
               
               /* If we're listening, stop now */
               if (hLstnSock != INVALID_SOCKET)
			   {
                 closesocket(hLstnSock);
                 hLstnSock = INVALID_SOCKET;
               }
               /* If there is a data connection, then abort it */
               if (hDataSock != INVALID_SOCKET)
                 QueueFtpCmd (ABOR, 0);
               /* Quit the control connection */
               if (hCtrlSock != INVALID_SOCKET)
                 QueueFtpCmd (QUIT, 0);
               SetDlgItemText (hDlg, IDC_SERVER, "Server: none");
               SetDlgItemText (hDlg, IDC_STATUS, "Status: not connected");
             }
             break;
                     
           case IDC_RETR:
             /* Prompt for name of remote file to get */
             if (nAppState & CTRLCONNECTED) {
               bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME),
                 hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
                 
               if (bOk && szDataFile[0]) {
                 if (!bToNul) {
                   /* If user provided a filename open same name here for write.
                    *  Truncate the filename to 8 chars plus 3, if necessary */
                   hDataFile = CreateLclFile (szDataFile);
                 }
                 if (hDataFile != HFILE_ERROR || bToNul) {
                   /* Tell the Server where to connect back to us */
                   hLstnSock =
                     InitDataConn(&stDLclName, hDlg, WSA_ASYNC+1);
                   if (hLstnSock != INVALID_SOCKET) {
                      /* Queue PORT, TYPE and RETR cmds */
                     if (QueueFtpCmd(PORT, 0)) {
                       if (QueueFtpCmd (TYPE, "I"))  
                         QueueFtpCmd(RETR, szDataFile);
                     }
                   }
                 }
               }
             } else
               not_connected();
             break;
            
           case IDC_STOR:
             /* Prompt for name of local file to send */
             if (nAppState & CTRLCONNECTED) {
               bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME),
                 hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
                                                                   
               if (bOk && szDataFile[0]) {
                 if (!bFromNul) {
                   /* If user provided filename, try to open same name locally */
                   hDataFile = _lopen(szDataFile, 0);
                   if (hDataFile == HFILE_ERROR) {
                     wsprintf(achTempBuf, "Unable to open file: %s", szDataFile);
                     MessageBox (hWinMain, (LPSTR)achTempBuf,"File Error",  
                       MB_OK | MB_ICONASTERISK);
                   }
                 }
                 if (hDataFile != HFILE_ERROR || bFromNul) {
                   /* Tell the Server where to connect back to us */
                   hLstnSock =
                     InitDataConn(&stDLclName, hDlg, WSA_ASYNC+1);
                   if (hLstnSock != INVALID_SOCKET) {
                     /* Queue PORT, TYPE and STOR cmds */
                     if (QueueFtpCmd (PORT, 0)) {
                       if (QueueFtpCmd (TYPE, "I"))
                         QueueFtpCmd (STOR, szDataFile);
                     }                     
                   }
                 }
               }
             } else
               not_connected();
             break;
             
           case IDC_ABOR:
             if (hCtrlSock != INVALID_SOCKET)
               /* Abort the pending ftp command */
               QueueFtpCmd (ABOR, 0);
             break;
             
           case IDC_LCWD:
             /* Prompt for directory name, and move to it on local system. */
             bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME), 
                     hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
             
             if (bOk && szDataFile[0]) {
               if (!(_chdir (szDataFile))) {
                 getcwd (szDataFile, MAXFILENAME-1);
                 SetDlgItemText (hDlg, IDC_LPWD, szDataFile);
               }
             }
             break;
             
           case IDC_LDEL:
             /* Prompt for filename, and delete it from local system */
             bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME), 
                     hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);

             if (bOk && szDataFile[0]) {
               /* If user provided filename, then delete it */
               remove (szDataFile);
             }
             break;
             
           case IDC_LDIR:
             /* Get local file directory, and display in notepad */
             if (GetLclDir(szTempFile)) {
               wsprintf (achTempBuf, "notepad %s", szTempFile);
               WinExec (achTempBuf, SW_SHOW);
             }             
             break;
        
           case IDC_RCWD:
             /* Prompt for directory name, and move to it on remote system. */
             if (nAppState & CTRLCONNECTED) {
               szDataFile[0] = 0;
               bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME), 
                 hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
             
               if (bOk && szDataFile[0]) {
                 QueueFtpCmd (CWD, szDataFile);
               }
             } else
               not_connected();
             break;
             
           case IDC_RDEL:
             /* Prompt for filename, and delete it from remote system */
             if (nAppState & CTRLCONNECTED) {
               szDataFile[0] = 0;
               bOk = DialogBoxParam (hInst, MAKEINTRESOURCE(IDD_FILENAME), 
                 hDlg, Dlg_File, (LONG)(LPSTR)szDataFile);
             
               if (bOk && szDataFile[0]) {
                 /* If user provided a filename, send command to delete it */
                 QueueFtpCmd (DELE, szDataFile);
               }
             } else
               not_connected();
             break;
                                                   
           case IDC_RDIR:
             /* Get remote file directory, and display in notepad file */
             if (nAppState & CTRLCONNECTED) {
               hDataFile = CreateLclFile (szTempFile);
               if (hDataFile != HFILE_ERROR) {
                 /* Prepare to receive the incoming connection from server */
                 hLstnSock =
                   InitDataConn(&stDLclName, hDlg, WSA_ASYNC+1);
                 if (hLstnSock != INVALID_SOCKET) {
                   if (QueueFtpCmd (PORT, 0))          /* Queue PORT, TYPE and LIST cmds */
                     if (QueueFtpCmd (TYPE, "A"))
                       QueueFtpCmd (LIST, 0);
                 }
               }
             } else 
               not_connected();
             break;
             
           case IDC_OPTIONS:
             DialogBox (hInst, MAKEINTRESOURCE(IDD_OPTIONS), 
                 hDlg, Dlg_Options);
             break;
             
           case IDABOUT:
             DialogBox (hInst, MAKEINTRESOURCE(IDD_ABOUT), hDlg, Dlg_About);
             break;

	       case WM_DESTROY:
           case IDC_EXIT:
             SendMessage (hDlg, WM_COMMAND, IDC_CLOSE, 0L);
             if (hLogFile != HFILE_ERROR) 
               _lclose(hLogFile);
             EndDialog(hDlg, msg);
             bRet = TRUE;
             break;
                       
           default:
              break;
         } /* end case WM_COMMAND: */
         break;
           
      case WM_INITDIALOG:
        hWinMain = hDlg;	/* save our main window handle */
        
        /* Assign an icon to dialog box */


        /* Initialize FTP command structure array */
        memset (astFtpCmd, 0, (sizeof(struct stFtpCmd))*MAX_CMDS);
        
        /* Display current working directory */
        getcwd (szDataFile, MAXFILENAME-1);
        SetDlgItemText (hDlg, IDC_LPWD, szDataFile);
        
        /* Open logfile, if logging enabled */
        if (bLogFile) 
		{
          hLogFile = _lcreat (szLogFile, 0);
          if (hLogFile == HFILE_ERROR) { 
            MessageBox (hWinMain, "Unable to open logfile",
              "File Error", MB_OK | MB_ICONASTERISK);
            bLogFile = FALSE;
          }
        }
        
        break;
             
      default:
        break;
  } /* end switch (msg) */

  return (bRet);
} /* end Dlg_Main() */

/*---------------------------------------------------------------------
 * Function: Dlg_Login
 *
 * Description: Prompt for destination host, user name, and password
 */                                        
BOOL CALLBACK Dlg_Login (
  HWND hDlg,
  UINT msg,
  UINT wParam,
  LPARAM lParam)
{
   BOOL bRet = FALSE;
   lParam = lParam;  // avoid warning

   switch (msg) {
     case WM_INITDIALOG:
       SetDlgItemText (hDlg, IDC_SERVER, szHost);
       SetDlgItemText (hDlg, IDC_USERNAME, szUser);
       SetDlgItemText (hDlg, IDC_PASSWORD, szPWrd);
       break;
     case WM_COMMAND:
       switch (wParam) 
	   {
         case IDOK:
           GetDlgItemText (hDlg, IDC_SERVER, szHost, MAXHOSTNAME);
           GetDlgItemText (hDlg, IDC_USERNAME, szUser, MAXUSERNAME);
           GetDlgItemText (hDlg, IDC_PASSWORD, szPWrd, MAXPASSWORD);
           EndDialog (hDlg, TRUE);
           bRet = TRUE;
           break;
         case IDCANCEL:
           EndDialog (hDlg, FALSE);
           bRet = FALSE;
           break;
         default:
           break;
       }
   }        
   return(bRet);
} /* end Dlg_Login */

/*---------------------------------------------------------------------
 * Function: Dlg_Options()
 *
 * Description: Allow user to change a number of run-time parameters
 *  that affect the operation, to allow experimentation and debugging.
 */                                        
BOOL CALLBACK Dlg_Options ( HWND hDlg, UINT msg, UINT wParam, LPARAM lParam)
{
   BOOL bRet = FALSE;
   lParam = lParam;  // avoid warning

   switch (msg) 
   {
     case WM_INITDIALOG:
       CheckDlgButton (hDlg, IDC_TO_NUL,  bToNul);
       CheckDlgButton (hDlg, IDC_FROM_NUL,bFromNul);
       CheckDlgButton (hDlg, IDC_LOGFILE, bLogFile);
       CheckDlgButton (hDlg, IDC_IOBEEP,  bIOBeep);
       CheckDlgButton (hDlg, IDC_REASYNC, bReAsync);
       break;
       
     case WM_COMMAND:
       switch (wParam) 
	   {
         case IDC_TO_NUL:
           bToNul = !bToNul;
           break;
         case IDC_FROM_NUL:
           bFromNul = !bFromNul;
           break;
         case IDC_LOGFILE:
           bLogFile = !bLogFile;
           break;
         case IDC_IOBEEP:
           bIOBeep = !bIOBeep;
           break;
         case IDC_REASYNC:
           bReAsync = !bReAsync;
           break;
         case IDOK:
           if (bLogFile && hLogFile == HFILE_ERROR) 
		   {
             bLogFile = FALSE;
           } 
		   else if (!bLogFile && hLogFile != HFILE_ERROR) 
		   {
             _lclose(hLogFile);
             hLogFile = HFILE_ERROR;
           } 
		   else if (bLogFile && hLogFile == HFILE_ERROR) 
		   {
             hLogFile = _lcreat (szLogFile, 0);
             if (hLogFile == HFILE_ERROR) 
			 { 
               MessageBox (hWinMain, "Unable to open logfile",
                 "File Error", MB_OK | MB_ICONASTERISK);
               bLogFile = FALSE;
             }
           }
           EndDialog (hDlg, TRUE);
           bRet = TRUE;
           break;
       }
   }        
   return(bRet);
} // end Dlg_Options()

/*---------------------------------------------------------------
 * Function: not_connected()
 *
 * Description: tell the user that the client needs a connection.
 */
void not_connected(void) 
{
  MessageBox (hWinMain, "You need to connect to an FTP Server",
   "Not Connected",  MB_OK | MB_ICONASTERISK);
}

/*--------------------------------------------------------------
 * Function: CloseFtpConn()
 *
 * Description: Close the connection gracefully and robustly,
 *  reading all remaining data into buffer before closure.
 */
int CloseFtpConn(SOCKET *hSock, LPSTR achInBuf, int len, HWND hWnd)
{
  char achDiscard[BUF_SIZE];
  int nRet=0;
  
  if (*hSock != INVALID_SOCKET) 
  {
    /* disable asynchronous notification if window handle provided */
    if (hWnd) 
	{
      nRet = WSAAsyncSelect(*hSock, hWnd, 0, 0);
      if (nRet == SOCKET_ERROR)
      ;//  WSAperror (WSAGetLastError(), "CloseFtpConn() WSAAsyncSelect()", hInst);
    }
  
    /* Half-close the connection to close neatly */
    nRet = shutdown (*hSock, 1);
    if (nRet == SOCKET_ERROR) {
      ;//WSAperror (WSAGetLastError(), "shutdown()", hInst);
    } else {
      /* Read and discard remaining data (until EOF or error) */
      nRet = 1;
      while (nRet && (nRet != SOCKET_ERROR)) {
        if (achInBuf) 
          nRet = RecvData(*hSock, hDataFile, achInBuf, len);
        else
          nRet = recv (*hSock, (LPSTR)achDiscard, BUF_SIZE, 0);
      }
      /* close the socket, and ignore any error 
       *  (since we can't do much about them anyway */
      closesocket (*hSock);
    }
    *hSock = INVALID_SOCKET;  /* we always invalidate socket */
  }
  return (nRet);
} /* end CloseFtpConn() */



SOCKET InitCtrlConn(PSOCKADDR_IN pstName, HWND hDlg, u_int nAsyncMsg) 
{
  int nRet;
  SOCKET hCtrlSock;
                     
  if (bDebug) 
    OutputDebugString("InitCtrlConn()\n");
    
  /* Get a TCP socket for control connection */
  hCtrlSock = socket (AF_INET, SOCK_STREAM, 0);
  if (hCtrlSock == INVALID_SOCKET) 
  {
    ;//WSAperror(WSAGetLastError(), "socket()", hInst);
  }
  else 
  {
                 
    /* Request async notification for most events */
    nRet = WSAAsyncSelect(hCtrlSock, hDlg, nAsyncMsg, 
           (FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE));
    if (nRet == SOCKET_ERROR) 
	{
      ;//WSAperror(WSAGetLastError(), "WSAAsyncSelect()", hInst);
      closesocket(hCtrlSock);
      hCtrlSock = INVALID_SOCKET;
    } 
	else 
	{
                   
      /* Initiate non-blocking connect to server */
      pstName->sin_family = PF_INET;
      pstName->sin_port   = htons(IPPORT_FTP);
      nRet = connect(hCtrlSock,(LPSOCKADDR)pstName,SOCKADDR_LEN);
      if (nRet == SOCKET_ERROR) 
	  {
	    int WSAErr = WSAGetLastError();
		               
	    /* Anything but "would block" error is bad */
	    if (WSAErr != WSAEWOULDBLOCK) 
		{
	      /* Report error and clean up */
	     ;// WSAperror(WSAErr, "connect()", hInst);
	      closesocket(hCtrlSock);
	      hCtrlSock = INVALID_SOCKET;
	    }
      }
    }
  }
  return (hCtrlSock);
} /* end InitCtrlConn() */

/*--------------------------------------------------------------
 * Function: SendFtpCmd()
 *
 * Description: Format and send an FTP command to the server
 */
int SendFtpCmd(void)
{
  int nRet, nLen, nBytesSent = 0;
  int nFtpCmd = astFtpCmd[1].nFtpCmd;
  
  if (bDebug) 
  {
    wsprintf(achTempBuf, 
      "SendFtpCmd()    Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
      nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd, 
      astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
    OutputDebugString (achTempBuf);
  }

  /* Create a command string (if we don't already have one) */
  if (szFtpCmd[0] == 0) 
  {
    
    switch (nFtpCmd) 
	{
      case PORT:
        wsprintf (szFtpCmd, "PORT %d,%d,%d,%d,%d,%d\r\n", 
          stDLclName.sin_addr.S_un.S_un_b.s_b1,  /* local addr */
          stDLclName.sin_addr.S_un.S_un_b.s_b2,
          stDLclName.sin_addr.S_un.S_un_b.s_b3,
          stDLclName.sin_addr.S_un.S_un_b.s_b4,
          stDLclName.sin_port & 0xFF,            /* local port */
         (stDLclName.sin_port & 0xFF00)>>8);
        break;
      case CWD:
      case DELE:
      case PASS:
      case RETR:
      case STOR:
      case TYPE:
      case USER:
        /* Ftp commmand and parameter */
        wsprintf (szFtpCmd, "%s %s\r\n", 
          aszFtpCmd[nFtpCmd], &(astFtpCmd[1].szFtpParm));
        break;
      case ABOR:
      case LIST:
      case PWD:
      case QUIT:
        /* Solitary Ftp command string (no parameter) */
        wsprintf (szFtpCmd, "%s\r\n", aszFtpCmd[nFtpCmd]);
        break;
      default: 
        return (0);  /* we have a bogus command! */
    }
  }
  nLen = strlen(szFtpCmd);
  
  if (hCtrlSock != INVALID_SOCKET) 
  {         
    /* Send the ftp command to control socket */
    while (nBytesSent < nLen) 
	{
      nRet = send(hCtrlSock, (LPSTR)szFtpCmd, nLen-nBytesSent, 0);
      if (nRet == SOCKET_ERROR) 
	  {
        int WSAErr = WSAGetLastError();
      
        /* If "would block" error we'll pickup again with async
         *  FD_WRITE notification, but any other error is bad news */
        if (WSAErr != WSAEWOULDBLOCK) 
          ;//WSAperror(WSAErr, "SendFtpCmd()", hInst);
        break;
      }
      nBytesSent += nRet;
    }
  }
  /* if we sent it all, update our status and move everything up 
   *  in command queue */
  if (nBytesSent == nLen) 
  {
    int i;

    if (nFtpCmd == PASS)                 /* hide password */
      memset (szFtpCmd+5, 'x', 10);
        
    if (bLogFile)                         /* log command */
      _lwrite (hLogFile, szFtpCmd, strlen(szFtpCmd));

    GetDlgItemText (hWinMain, IDC_REPLY,  /* display command */
      achRplyBuf, RPLY_SIZE-strlen(szFtpCmd));
    wsprintf (achTempBuf, "%s%s", szFtpCmd, achRplyBuf);
    SetDlgItemText (hWinMain, IDC_REPLY, achTempBuf);

    szFtpCmd[0] = 0;  /* disable Ftp command string */
    
    /* move everything up in the command queue */
    for (i=0; i < nQLen; i++) 
	{
      astFtpCmd[i].nFtpCmd = astFtpCmd[i+1].nFtpCmd;
      astFtpCmd[i+1].nFtpCmd = 0;        /* reset old command */
      if (*(astFtpCmd[i+1].szFtpParm)) 
	  {
        memcpy (astFtpCmd[i].szFtpParm, astFtpCmd[i+1].szFtpParm, CMD_SIZE);
        *(astFtpCmd[i+1].szFtpParm) = 0; /* terminate old string */
      } 
	  else 
	  {
        *(astFtpCmd[i].szFtpParm) = 0;   /* terminate unused string */
      }
    }
    nQLen--;          /* decrement the queue length */
    
    switch (nFtpCmd) 
	{
      case (USER):
        SetDlgItemText (hWinMain, IDC_STATUS,"Status: connecting");
        break;
      case (STOR):
        SetDlgItemText (hWinMain, IDC_STATUS,"Status: sending a file");
        break;
      case (RETR):
        SetDlgItemText (hWinMain, IDC_STATUS,"Status: receiving a file");
        break;
      case (LIST):
        SetDlgItemText (hWinMain, IDC_STATUS,"Status: receiving directory");
        break;
      case (QUIT):
        SetDlgItemText (hWinMain, IDC_SERVER, "Server: none");
        SetDlgItemText (hWinMain, IDC_STATUS, "Status: not connected");
        break;
    }
  }
  return (nBytesSent);
} /* end SendFtpCmd() */
                     
/*--------------------------------------------------------------
 * Function: QueueFtpCmd()
 *
 * Description: Put FTP command in command queue for sending after we
 *  receive responses to pending commands or now if nothing is pending
 */
BOOL QueueFtpCmd(int nFtpCmd, LPSTR szFtpParm) {
   
  if (bDebug) {
    wsprintf(achTempBuf, 
      "QueueFtpCmd()    Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, pend:%d\n", 
      nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
      astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, 
      recv(hCtrlSock, achTempBuf, BUF_SIZE, MSG_PEEK));
    OutputDebugString (achTempBuf);    
  }
  if ((nFtpCmd == ABOR) || (nFtpCmd == QUIT)) 
  {
    AbortFtpCmd();
    if (hCtrlSock != INVALID_SOCKET)
      SetDlgItemText (hWinMain, IDC_STATUS,"Status: connected");
  } 
  else if (nQLen == MAX_CMDS) 
  {
    /* Notify user if they can't fit in the queue */
    MessageBox (hWinMain, "Ftp command queue is full, try again later", 
       "Can't Queue Command", MB_OK | MB_ICONASTERISK);
    return (FALSE);            /* not queued */
  }
  nQLen++;  /* increment Ftp command counter */

  /* Save command vitals */  
  astFtpCmd[nQLen].nFtpCmd = nFtpCmd;
  if (szFtpParm)
    lstrcpy (astFtpCmd[nQLen].szFtpParm, szFtpParm);

  if (!(astFtpCmd[0].nFtpCmd) && astFtpCmd[1].nFtpCmd) 
  {
    /* If nothing pending reply, then send the next command */
    SendFtpCmd();
  }
  return (TRUE);   /* queued! */
}  /* end QueueFtpCmd() */

/*--------------------------------------------------------------
 * Function: AbortFtpCmd()
 *
 * Description: Clean up routine to abort a pending FTP command and
 * clear the command queue
 */
void AbortFtpCmd(void) 
{
  int i;

  if (hLstnSock != INVALID_SOCKET) 
  {/* Close listen socket */
    closesocket(hLstnSock);
    hLstnSock = INVALID_SOCKET;
  }
  if (hDataSock != INVALID_SOCKET)
  { /* Close data socket */
    CloseFtpConn(&hDataSock,  
      (astFtpCmd[0].nFtpCmd != STOR) ? achInBuf : (PSTR)0, 
      INPUT_SIZE, hWinMain);
    EndData();
  }
  for (i=0;i<MAX_CMDS;i++)          /* Clear command queue */
    astFtpCmd[i].nFtpCmd = 0;         
  nQLen = 0;
} /* end AbortFtpCmd() */

//--------------------------------------------------------------
// Function: RecvFtpRply()
//
// Description: Read the FTP reply from server (and log it)
//--------------------------------------------------------------
int RecvFtpRply(SOCKET hCtrlSock, LPSTR szFtpRply, int nLen)
{
	int nRet=0;

	if (bDebug) 
	{
	    wsprintf(achTempBuf, 
		"RecvFtpRply()    Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
		nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
		astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
		OutputDebugString (achTempBuf);
	}
	if (hCtrlSock != INVALID_SOCKET) 
	{
		memset(szFtpRply,0,nLen);   // Init receive buffer
  
		// Read as much as we can 
		nRet = recv(hCtrlSock,(LPSTR)szFtpRply,nLen,0);
    
		if (nRet == SOCKET_ERROR) 
		{
			int WSAErr = WSAGetLastError();
			if (WSAErr != WSAEWOULDBLOCK) 
			{
				MessageBox(0, "Error with RecvFtpRply()", "Error", MB_OK);
			}
		} 
		else if (bLogFile)   // log reply
			_lwrite (hLogFile, szFtpRply, nRet);     
	}
	return (nRet);  
} // end RecvFtpReply() 

//--------------------------------------------------------------
// Function: ProcessFtpRply()
//
// Description: Figure out what happened, and what to do next.
//---------------------------------------------------------------
void ProcessFtpRply (LPSTR szRply, int nBufLen) 
{
	LPSTR szFtpRply;
	int nPendingFtpCmd, i;
  
	if (bDebug) 
	{
		wsprintf(achTempBuf, "ProcessFtpRply() Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
		nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
		astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
		OutputDebugString (achTempBuf);    
	}

	// Skip continuation lines (denoted by a dash after reply code
	//  or with a blank reply code and no dash) 
	szFtpRply = szRply;
	while ((szFtpRply[3] == '-') || 
	      ((szFtpRply[0] == ' ') && 
		   (szFtpRply[1] == ' ') &&
		   (szFtpRply[2] == ' '))) 
	{
		/* find end of reply line */
		for (i=0; szFtpRply[0] != 0x0a && szFtpRply[0] && i < nBufLen-3; szFtpRply++, i++);
		szFtpRply++;       // go to beginning of next reply 
		if (!szFtpRply[0]) // quit if end of string 
			return;
	}
  
	szFtpCmd[0]  = 0;                        // Disable old command string
	nPendingFtpCmd = astFtpCmd[0].nFtpCmd; // Save last Ftp Cmd 
	if ((szFtpRply[0] != '1') && 
        (nPendingFtpCmd != LIST) &&
        (nPendingFtpCmd != STOR) &&
        (nPendingFtpCmd != RETR))
		// For any but preliminary reply, clear old command 
		astFtpCmd[0].nFtpCmd = 0;

  // First digit in 3-digit Ftp reply code is the most significant 
	switch (szFtpRply[0]) {
		case ('1'):  // Positive preliminary reply
		break;
		case ('2'):  // Positive completion reply 
			switch(nPendingFtpCmd) 
			{
				case 0:  
				  // Check for "220 Service ready for new user" reply, and
				  //  send user command to login if login message found 
				  if ((szFtpRply[1] == '2') && (szFtpRply[2] == '0'))
					QueueFtpCmd(USER, szUser);
				  break;
				case CWD:
				case USER:
				case PASS:
					// We're logged in!  Get remote working directory
					QueueFtpCmd(PWD, 0);
					break;
				case PWD:
					// Display remote working directory
					SetDlgItemText (hWinMain, IDC_RPWD, &szFtpRply[4]);
					break;
				case TYPE:
				case PORT:
					// Send next command (it's already queued) 
					SendFtpCmd();
					break;
				case ABOR:
					// Close the data socket 
					if (hDataSock != INVALID_SOCKET)
						CloseFtpConn(&hDataSock, (PSTR)0, 0, hWinMain);
					break;
				case QUIT:
					// Close the control socket
					if (hCtrlSock != INVALID_SOCKET)
						CloseFtpConn(&hCtrlSock, (PSTR)0, 0, hWinMain);
					break;
				default: break; // Nothing to do after most replies 
			}
			break;     
		case ('3'):  // Positive intermediate reply
			if (nPendingFtpCmd == USER)
				QueueFtpCmd(PASS, szPWrd);
			break;
		case ('4'):  // Transient negative completion reply 
		case ('5'):  // Permenant negative completion reply 
			// If Port failed, forget about queued commands 
			if (nPendingFtpCmd != ABOR)
				QueueFtpCmd(ABOR, 0);
			break;
	}
} // end ProcessFtpRply()

//--------------------------------------------------------------
// Function: InitDataConn()
//
// Description: Set up a listening socket for a data connection
//--------------------------------------------------------------
SOCKET InitDataConn(PSOCKADDR_IN lpstName, HWND hDlg, u_int nAsyncMsg)
{
	int nRet;
	SOCKET hLstnSock;
	int nLen = SOCKADDR_LEN;
  
	if (bDebug) 
	{
		wsprintf(achTempBuf, 
		"InitDataConn()   Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
		nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
		astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
		OutputDebugString (achTempBuf);    
	}
	lByteCount = 0;  // init byte counter
  
	// Get a TCP socket to use for data connection listen
	hLstnSock = socket (AF_INET, SOCK_STREAM, 0);
	if (hLstnSock == INVALID_SOCKET)  
	{
	    MessageBox(0, "Error with hLstnSock - InitDataConn() function", "Error", MB_OK);
	} 
	else 
	{
		// Request async notification for most events 
		nRet = WSAAsyncSelect(hLstnSock, hDlg, nAsyncMsg, 
           (FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE));
		if (nRet == SOCKET_ERROR) 
		{
			MessageBox(0, "Error with WSAAsyncSelect()", "ERROR", MB_OK);
		} 
		else
		{         
			// Name the local socket with bind() 
			lpstName->sin_family = PF_INET;
			lpstName->sin_port   = 0;  // any port will do 
			nRet = bind(hLstnSock,(LPSOCKADDR)lpstName,SOCKADDR_LEN);
			if (nRet == SOCKET_ERROR) 
			{
				MessageBox(0, "Error with bind()", "ERROR", MB_OK);
			} 
			else 
			{
				// Get local port number assigned by bind() 
				nRet = getsockname(hLstnSock,(LPSOCKADDR)lpstName, 
				(int FAR *)&nLen);
				if (nRet == SOCKET_ERROR) 
				{
					MessageBox(0, "Error with getsockname", "ERROR", MB_OK);
				} 
				else 
				{
					// Listen for incoming connection requests 
					nRet = listen(hLstnSock, 5);
					if (nRet == SOCKET_ERROR) 
					{
						MessageBox(0, "Error with listen()", "ERROR", MB_OK);
					}
				}
			}
		}
		// If we haven't had an error but we still don't know the local 
		// IP address, then we need to try to get it before we return 
		if (!lpstName->sin_addr.s_addr)	
		{
			lpstName->sin_addr.s_addr = GetHostID();
			if (!lpstName->sin_addr.s_addr) 
			{
				MessageBox (hDlg, "Can't get local IP address", 
				"InitDataConn() Failed", MB_OK | MB_ICONASTERISK);
				nRet = SOCKET_ERROR;
			}
		}
		// If we had an error or we still don't know our IP address, 
		// then we have a problem.  Clean up 
		if (nRet == SOCKET_ERROR) 
		{
			closesocket(hLstnSock);
			hLstnSock = INVALID_SOCKET;
		}
	}
	return (hLstnSock);
} // end InitDataConn()

//--------------------------------------------------------------
// Function: AcceptDataConn()
//
// Description: Accept an incoming data connection
//--------------------------------------------------------------
SOCKET AcceptDataConn(SOCKET hLstnSock, PSOCKADDR_IN pstName)
{
	SOCKET hDataSock;
	int nRet, nLen = SOCKADDR_LEN, nOptval;
  
	if (bDebug) 
	{
		wsprintf(achTempBuf, 
		"AcceptDataConn() Qlen:%d Cmd[0]:%d [1]:%d [2]:%d [3]:%d, State:%d\n", 
		nQLen, astFtpCmd[0].nFtpCmd, astFtpCmd[1].nFtpCmd,
		astFtpCmd[2].nFtpCmd, astFtpCmd[3].nFtpCmd, nAppState);
		OutputDebugString (achTempBuf);    
	}

	hDataSock = accept (hLstnSock, (LPSOCKADDR)pstName, (LPINT)&nLen);
	if (hDataSock == SOCKET_ERROR) 
	{
	    int WSAErr = WSAGetLastError();
		if (WSAErr != WSAEWOULDBLOCK)
			MessageBox(0, "Error with accept", "ERROR", MB_OK);
	} 
	else if (bReAsync) 
	{
		// This SHOULD be unnecessary, since all new sockets are supposed
		//  to inherit properties of the listening socket (like all the
		// asynch events registered but some WinSocks don't do this.
		// Request async notification for most events 
		nRet = WSAAsyncSelect(hDataSock, hWinMain, WSA_ASYNC+1, 
			(FD_READ | FD_WRITE | FD_CLOSE));
		if (nRet == SOCKET_ERROR) 
		{
			MessageBox(0, "Error with WSAAsyncSelect()", "ERROR", MB_OK);
		}
		// Try to get lots of buffer space 
		nOptval = astFtpCmd[0].nFtpCmd==STOR ? SO_SNDBUF : SO_RCVBUF;
		GetBuf(hDataSock, INPUT_SIZE*2, nOptval);
	}
  return (hDataSock);
} // end AcceptData() 

//--------------------------------------------------------------
// Function: SendData()
//
// Description: Open data file, read and send
//--------------------------------------------------------------
long SendData(SOCKET *hDataSock, HFILE hDataFile, int len)
{
	static int cbReadFromFile;         // bytes read from file 
	static int cbSentToServer;         // number of buffered bytes sent 
	static HFILE hLastFile;            // handle of last file sent 
	long cbTotalSent = 0;              // total bytes sent 
	int nRet, WSAErr, cbBytesToSend;

	// Reset our counters when we access a new file 
	if (hLastFile != hDataFile) 
	{
		cbReadFromFile = 0;
		cbSentToServer = 0;
		hLastFile = hDataFile;
	}
    
	// Read data from file, and send it. 
	do 
	{
		if (bIOBeep)
			MessageBeep(0xFFFF);

		// calculate what's left to send 
		cbBytesToSend = cbReadFromFile - cbSentToServer; 
		if (cbBytesToSend <= 0) 
		{
    
			// read data from input file, if we need it 
			if (!bFromNul) 
			{
				cbReadFromFile = _lread(hDataFile, achOutBuf, INPUT_SIZE);
				if (cbReadFromFile == HFILE_ERROR) 
				{
					MessageBox (hWinMain, "Error reading data file", 
					"SendData() Failed", MB_OK | MB_ICONASTERISK);
					break;
				} 
				else if (!cbReadFromFile)
				{
					// EOF: no more data to send 
					CloseFtpConn(hDataSock, (PSTR)0, 0, hWinMain);
					EndData();
					break;
				} 
				else 
				{
					cbBytesToSend = cbReadFromFile; // send as much as we read 
				} 
			} 
			else 
			{
				// just send whatever's in memory (up to our max)
				if (lByteCount < MAXNULPUT) 
				{
					cbBytesToSend = INPUT_SIZE;
				} 
				else 
				{
					CloseFtpConn(hDataSock, (PSTR)0, 0, hWinMain);
					EndData();
				}
			}
			cbSentToServer = 0;  // reset tally 
		}
		// Send data to server 
		nRet = send (*hDataSock, &(achOutBuf[cbSentToServer]), ((len < cbBytesToSend) ? len : cbBytesToSend), 0);

		if (nRet == SOCKET_ERROR) 
		{
			WSAErr = WSAGetLastError();
			// Display significant errors 
			if (WSAErr != WSAEWOULDBLOCK)
				MessageBox(0, "Error with send()", "Error", MB_OK);
		} 
		else 
		{
			// Update byte counter, and display. 
			lByteCount += nRet;
			_ltoa(lByteCount, achTempBuf, 10);
			SetDlgItemText(hWinMain, IDC_DATA_RATE, achTempBuf);
			cbSentToServer += nRet;// tally bytes sent since last file read
			cbTotalSent    += nRet;// tally total bytes sent since we started
		}
	}while (nRet != SOCKET_ERROR);

	return (cbTotalSent);
} // end SendData() 

//--------------------------------------------------------------
// Function: RecvData()
//
// Description: Receive data from net and write to open data file 
//--------------------------------------------------------------
int RecvData(SOCKET hDataSock, HFILE hDataFile, LPSTR achInBuf, int len)
{
	static HFILE hLastFile;          // handle of last file sent
	static int cbBytesBuffered;      // total bytes received
	int cbBytesRcvd = 0;
	int nRet=0, WSAErr;

	if (hDataFile != hLastFile) 
	{
		hLastFile = hDataFile;
		cbBytesBuffered = 0; 
	}
                                                       
	// Read as much as we can from server
	while (cbBytesBuffered < len) 
	{

		nRet = recv (hDataSock,&(achInBuf[cbBytesBuffered]), 
			len-cbBytesBuffered, 0);

		if (nRet == SOCKET_ERROR) 
		{
	        WSAErr = WSAGetLastError();
			// Display significant errors 
			if (WSAErr != WSAEWOULDBLOCK)
				MessageBox(0, "Error with recv()", "Error", MB_OK);
			// exit recv() loop on any error 
			goto recv_end;
		} 
		else if (nRet == 0) 
		{ // Other side closed socket
			// quit if server closed connection
			goto recv_end;
          
		} 
		else 
		{
			// Update byte counter, and display
			lByteCount += nRet;
			_ltoa(lByteCount, achTempBuf, 10);
			SetDlgItemText(hWinMain, IDC_DATA_RATE, achTempBuf);
			cbBytesRcvd += nRet;     /* tally bytes read */
			cbBytesBuffered += nRet;
		}
	}
	recv_end:
		if (!bToNul && ((cbBytesBuffered > (len-MTU_SIZE)) || 
			((nRet == SOCKET_ERROR) && WSAGetLastError() != WSAEWOULDBLOCK) || 
			(nRet == 0))) 
		{
			// If we have a lot buffered, write to data file 
			nRet = _lwrite(hDataFile, achInBuf, cbBytesBuffered);
			
			if (nRet == HFILE_ERROR)
				MessageBox (hWinMain, "Can't write to local file","RecvData() Failed", MB_OK | MB_ICONASTERISK);
			cbBytesBuffered = 0;
		} 
		else if (bToNul)
			cbBytesBuffered = 0;
	return (cbBytesRcvd);
} /// end RecvData() 

//--------------------------------------------------------------
// Function: EndData()
//
// Description: Close up the data connection
//--------------------------------------------------------------
void EndData (void) 
{
	LONG dByteRate;
	LONG lMSecs;

	// Calculate data transfer rate, and display 
	lMSecs = (LONG) GetTickCount() - lStartTime;
	if (lMSecs <= 55)
		lMSecs = 27;  // about half of 55Msec PC clock resolution
              
	// Socket Check should not be necessary, but some WinSocks            
	//  mistakenly post FD_CLOSE to listen socket after close 
	nAppState &= ~(DATACONNECTED);
	SetDlgItemText (hWinMain, IDC_STATUS, "Status: connected");
            
	if (lByteCount > 0L) 
	{
		dByteRate = (lByteCount/lMSecs); // data rate (bytes/Msec) 
		wsprintf (achTempBuf,"%ld bytes %s in %ld.%ld seconds (%ld.%ld Kbytes/sec)",
		lByteCount, 
		((astFtpCmd[0].nFtpCmd==STOR) ? "sent":"received"),
		lMSecs/1000, lMSecs%1000, 
		(dByteRate*1000)/1024, (dByteRate*1000)%1024);
		SetDlgItemText (hWinMain, IDC_DATA_RATE, achTempBuf);
		if (hLogFile != HFILE_ERROR)
			_lwrite (hLogFile, achTempBuf, strlen(achTempBuf));
	}
	lStartTime = 0L;
	if (hDataFile != HFILE_ERROR) 
	{
		_lclose (hDataFile);
		hDataFile = HFILE_ERROR;
		if (astFtpCmd[0].nFtpCmd == LIST) 
		{
			wsprintf (achTempBuf, "notepad %s", szTempFile);
			WinExec (achTempBuf, SW_SHOW);
		}
	}
	astFtpCmd[0].nFtpCmd = 0;  // reset pending command
} // end EndData()           


//-----------------------------------------------------------
// Function: GetHostID()
//
// Description: 
//  Get the Local IP address using the following algorithm:
//    - get local hostname with gethostname()
//    - attempt to resolve local hostname with gethostbyname()
//    if that fails:
//    - get a UDP socket
//    - connect UDP socket to arbitrary address and port
//    - use getsockname() to get local address
//-----------------------------------------------------------
LONG GetHostID () 
{
	char szLclHost [MAXHOSTNAME];
    LPHOSTENT lpstHostent;
    SOCKADDR_IN stLclAddr;
    SOCKADDR_IN stRmtAddr;
    int nAddrSize = sizeof(SOCKADDR);
    SOCKET hSock;
    int nRet;
    
    // Init local address (to zero) 
    stLclAddr.sin_addr.s_addr = INADDR_ANY;
    
    // Get the local hostname
    nRet = gethostname(szLclHost, MAXHOSTNAME); 
    if (nRet != SOCKET_ERROR)
	{
		// Resolve hostname for local address
		lpstHostent = gethostbyname((LPSTR)szLclHost);
		if (lpstHostent)
	        stLclAddr.sin_addr.s_addr = *((u_long FAR*) (lpstHostent->h_addr));
    } 
    
    // If still not resolved, then try second strategy 
	if (stLclAddr.sin_addr.s_addr == INADDR_ANY) 
	{
		// Get a UDP socket
		hSock = socket(AF_INET, SOCK_DGRAM, 0);
		if (hSock != INVALID_SOCKET)  
		{
			// Connect to arbitrary port and address (NOT loopback) 
			stRmtAddr.sin_family = AF_INET;
			stRmtAddr.sin_port   = htons(IPPORT_ECHO);
			stRmtAddr.sin_addr.s_addr = inet_addr("128.127.50.1");
			nRet = connect(hSock,
                       (LPSOCKADDR)&stRmtAddr,
                       sizeof(SOCKADDR));
			if (nRet != SOCKET_ERROR) 
			{
				// Get local address 
				getsockname(hSock, 
                      (LPSOCKADDR)&stLclAddr, 
                      (int FAR*)&nAddrSize);
			}
			closesocket(hSock);   // we're done with the socket
		}
    }
    return (stLclAddr.sin_addr.s_addr);
} // GetHostID()


//-----------------------------------------------------------
// Function: GetAddr()
//
// Description: Given a string, it will return an IP address.
//   - first it tries to convert the string directly
//   - if that fails, it tries o resolve it as a hostname
//
// WARNING: gethostbyname() is a blocking function
//------------------------------------------------------------
unsigned long GetAddr (LPSTR szHost) 
{
	LPHOSTENT lpstHost;
	unsigned long lAddr = INADDR_ANY;
  
	// check that we have a string
	if (*szHost) 
	{
  
		// check for a dotted-IP address string 
		lAddr = inet_addr (szHost);
  
		// If not an address, then try to resolve it as a hostname
		if ((lAddr == INADDR_NONE) && (_fstrcmp (szHost, "255.255.255.255"))) 
		{
      
			lpstHost = gethostbyname(szHost);
			if (lpstHost) 
			{  // success 
				lAddr = *((unsigned long FAR *) (lpstHost->h_addr));
			} 
			else 
			{  
				lAddr = INADDR_ANY;   // failure 
			}
		}
	}
	return (lAddr); 
} /// end GetAddr() 

//---------------------------------------------------------------
// Function: GetLclDir()
//
// Description: Get the local file directory and write to
//   temporary file for later display.
//---------------------------------------------------------------
int GetLclDir(LPSTR szTempFile)
{
	struct _finddata_t stFile;  // Microsoft's 32bit find file structure

	HFILE hTempFile;
	int nNext;

	hTempFile = CreateLclFile (szTempFile);

	if (hTempFile != HFILE_ERROR) 
	{
		nNext =_findfirst("*.*", &stFile);
		while (!nNext)  
		{
			wsprintf(achTempBuf, " %-12s %.24s  %9ld\n",
               stFile.name, ctime( &( stFile.time_write ) ), stFile.size );
			_lwrite(hTempFile, achTempBuf, strlen(achTempBuf));
			nNext = _findnext(nNext, &stFile);
		}

		_lclose (hTempFile);
		return (1);
	}
  return (0);  
} // end GetLclDir()

//---------------------------------------------------------------
// Function: CreateLclFile()
//
// Description: Try to create a file on local system, and if it
//  fails notify user and prompt for new local filename;
//---------------------------------------------------------------
HFILE CreateLclFile (LPSTR szFileName) 
{
	HFILE hFile;
	char szRmtFile[MAXFILENAME];

	hFile = _lcreat (szFileName, 0);  // create the file
  
	strcpy (szRmtFile, szFileName);   // save remote filename 
	while (hFile == HFILE_ERROR) 
	{
		wsprintf(achTempBuf, 
		"Unable to create file %s.  Change the name.", szFileName);
		MessageBox (hWinMain, achTempBuf,
		"File Error", MB_OK | MB_ICONASTERISK);
		if (!DialogBox (hInst, MAKEINTRESOURCE(IDD_FILENAME),hWinMain, Dlg_File)) 
		{
			// No new filename provided, so quit
			break;
		} 
		else 
		{
			// Try to create new filename 
			hFile = _lcreat (szFileName, 0);
		}
	}
	strcpy (szFileName, szRmtFile);  // replace remote filename
  
	return (hFile);
} // end CreateLclFile() 

//---------------------------------------------------------------------
// Function: Dlg_File()
//
// Description:  Prompt for a file name (also used for directory names)
//---------------------------------------------------------------------                                        
int CALLBACK Dlg_File(HWND hDlg, UINT msg, UINT wParam, LPARAM lParam)  
{
	int bRet = 0;
	static LPSTR szDataFile;
   
	lParam = lParam;  // avoid warning 

	switch (msg) 
	{
		case WM_INITDIALOG:
			szDataFile = (LPSTR) lParam;       
			break;
		case WM_COMMAND:
			switch (wParam) 
			{
				case IDOK:
					GetDlgItemText (hDlg, IDC_FILE, szDataFile, MAXFILENAME);
					EndDialog (hDlg, TRUE);
					bRet = 1;
				break;
				case IDCANCEL:
					EndDialog (hDlg, FALSE);
					bRet = 0;
				break;
			}
	}        
	return(bRet);
} // end Dlg_File()

//---------------------------------------------------------------------
//
// Function: Dlg_About()
//
// Description:
//---------------------------------------------------------------------
int CALLBACK Dlg_About(HWND hDlg, UINT msg, UINT wParam, LPARAM lParam)
{
    int bRet = 0;
    char achDataBuf[WSADESCRIPTION_LEN+1];
    
    lParam = lParam;	// avoid warning

    switch (msg) 
	{
		case WM_INITDIALOG:
 	        wsprintf (achDataBuf, "(Compiled: %s, %s)\n", 
  		        (LPSTR)__DATE__, (LPSTR)__TIME__);
            SetDlgItemText (hDlg, IDC_COMPILEDATE, (LPCSTR)achDataBuf);
            wsprintf (achDataBuf, 
            	"Version: %d.%d", 
            	LOBYTE(stWSAData.wVersion),  // major version 
            	HIBYTE(stWSAData.wVersion)); // minor version 
            SetDlgItemText (hDlg, IDS_DLLVER, (LPCSTR)achDataBuf);
            wsprintf (achDataBuf, 
            	"HiVersion: %d.%d", 
            	LOBYTE(stWSAData.wVersion),  // major version 
            	HIBYTE(stWSAData.wVersion)); // minor version 
            SetDlgItemText(hDlg,IDS_DLLHIVER, achDataBuf);            
            SetDlgItemText(hDlg,IDS_DESCRIP,(LPCSTR)(stWSAData.szDescription));
            SetDlgItemText(hDlg,IDS_STATUS,(LPCSTR)(stWSAData.szSystemStatus));
            wsprintf (achDataBuf,"MaxSockets: %u", stWSAData.iMaxSockets);
            SetDlgItemText (hDlg, IDS_MAXSOCKS, (LPCSTR)achDataBuf);
            wsprintf (achDataBuf,"iMaxUdp: %u", stWSAData.iMaxUdpDg);
            SetDlgItemText (hDlg, IDS_MAXUDP, (LPCSTR)achDataBuf);
            
            break;
	    
			case WM_COMMAND:
    			switch (wParam) 
				{
        			case IDOK:
            			EndDialog (hDlg, 0);
            			bRet = 1;
				}
				break;
	}
    return (bRet);
} // end Dlg_About() 

int GetBuf(SOCKET hSock, int nBigBufSize, int nOptval)
{
	int nRet, nTrySize, nFinalSize = 0;
	for (nTrySize = nBigBufSize; nTrySize > MTU_SIZE; nTrySize >>=1)
	{
		nRet = setsockopt(hSock, SOL_SOCKET, nOptval, (char FAR*)&nTrySize, sizeof(int));
		if (nRet == SOCKET_ERROR)
		{
			int WSAErr = WSAGetLastError();
			if ((WSAErr == WSAENOPROTOOPT) || (WSAErr == WSAEINVAL))
				break;
		}
		else
		{
			nRet = sizeof(int);
			getsockopt(hSock, SOL_SOCKET, nOptval, (char FAR*)&nFinalSize, &nRet);
			break;
		}
		
	}
	return(nFinalSize);
}