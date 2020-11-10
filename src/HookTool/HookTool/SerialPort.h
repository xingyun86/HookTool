
/*
**	FILENAME			CSerialPort.h
**
**	PURPOSE				This class can read, write and watch one serial port.
**						It sends messages to its owner when something happends on the port
**						The class creates a thread for reading and writing so the main
**						program is not blocked.
**
**	CREATION DATE		15-09-1997
**	LAST MODIFICATION	12-11-1997
**
**	AUTHOR				Remon Spekreijse
**
**
*/

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#pragma once

#include <windows.h>
#include <assert.h>
#if defined(_DEBUG) || defined(DEBUG)
#define MY_PRINT_DEBUG printf
#else
#define MY_PRINT_DEBUG
#endif

#define WM_COMM_BREAK_DETECTED		WM_USER+1	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_USER+2	// The CTS (clear-to-send) signal changed state. 
#define WM_COMM_DSR_DETECTED		WM_USER+3	// The DSR (data-set-ready) signal changed state. 
#define WM_COMM_ERR_DETECTED		WM_USER+4	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. 
#define WM_COMM_RING_DETECTED		WM_USER+5	// A ring indicator was detected. 
#define WM_COMM_RLSD_DETECTED		WM_USER+6	// The RLSD (receive-line-signal-detect) signal changed state. 
#define WM_COMM_RXCHAR				WM_USER+7	// A character was received and placed in the input buffer. 
#define WM_COMM_RXFLAG_DETECTED		WM_USER+8	// The event character was received and placed in the input buffer.  
#define WM_COMM_TXEMPTY_DETECTED	WM_USER+9	// The last character in the output buffer was sent.  
#define MaxSerialPortNum			1024

class CSerialPort
{
public:
	// contruction and destruction
	//
	// Constructor
	//
	CSerialPort()
	{
		m_hComm = NULL;

		// initialize overlapped structure members to zero
		m_ov.Offset = 0;
		m_ov.OffsetHigh = 0;

		// create events
		m_ov.hEvent = NULL;
		m_hWriteEvent = NULL;
		m_hShutdownEvent = NULL;

		m_bThreadAlive = FALSE;
	}

	//
	// Delete dynamic memory
	//
	virtual ~CSerialPort()
	{
		do
		{
			SetEvent(m_hShutdownEvent);
		} while (m_bThreadAlive);

		MY_PRINT_DEBUG("Thread ended\n");
	}

public:
	// port initialisation											
	//
	// Initialize the port. This can be port 1 to 4.
	//
	BOOL InitPort(HWND hWndPort,// the owner (CWnd) of the port (receives message)
		UINT  portnr = 1,		// portnumber (1..4)
		UINT  baud = 9600,		// baudrate
		CHAR  parity = 'N',		// parity 
		UINT  databits = 8,		// databits 
		UINT  stopbits = 1,		// stopbits 
		DWORD dwCommEvents = EV_RXCHAR | EV_CTS,	// EV_RXCHAR, EV_CTS etc
		UINT  writebuffersize = USHRT_MAX)	// size to the writebuffer
	{
		assert(portnr > 0 && portnr < MaxSerialPortNum + 1);
		assert(hWndPort != NULL);

		// if the thread is alive: Kill
		if (m_bThreadAlive)
		{
			do
			{
				SetEvent(m_hShutdownEvent);
			} while (m_bThreadAlive);
			MY_PRINT_DEBUG("Thread ended\n");
		}

		// create events
		if (m_ov.hEvent != NULL)
			ResetEvent(m_ov.hEvent);
		m_ov.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

		if (m_hWriteEvent != NULL)
			ResetEvent(m_hWriteEvent);
		m_hWriteEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

		if (m_hShutdownEvent != NULL)
			ResetEvent(m_hShutdownEvent);
		m_hShutdownEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

		// initialize the event objects
		m_hEventArray[0] = m_hShutdownEvent;	// highest priority
		m_hEventArray[1] = m_ov.hEvent;
		m_hEventArray[2] = m_hWriteEvent;

		// initialize critical section
		InitializeCriticalSection(&m_csCommunicationSync);

		// set buffersize for writing and save the owner
		m_hWndPort = hWndPort;

		m_nPortNr = portnr;

		m_nWriteBufferSize = writebuffersize;
		m_dwCommEvents = dwCommEvents;

		BOOL bResult = FALSE;
		CHAR tzPort[MAX_PATH] = { 0 };
		CHAR tzBaud[MAX_PATH] = { 0 };

		// now it critical!
		EnterCriticalSection(&m_csCommunicationSync);

		// if the port is already opened: close it
		if (m_hComm != NULL)
		{
			CloseHandle(m_hComm);
			m_hComm = NULL;
		}

		// prepare port strings
		wsprintfA(tzPort, ("\\\\.\\COM%d"), portnr);
		wsprintfA(tzBaud, ("baud=%d parity=%c data=%d stop=%d"), baud, parity, databits, stopbits);

		// get a handle to the port
		m_hComm = CreateFileA(tzPort,						// communication port string (COMX)
			GENERIC_READ | GENERIC_WRITE,	// read/write types
			0,								// comm devices must be opened with exclusive access
			NULL,							// no security attributes
			OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
			FILE_FLAG_OVERLAPPED,			// Async I/O
			0);							// template must be 0 for comm devices

		if (m_hComm == INVALID_HANDLE_VALUE)
		{
			// port not found
			return FALSE;
		}

		// set the timeout values
		m_CommTimeouts.ReadIntervalTimeout = 1000;
		m_CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
		m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
		m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
		m_CommTimeouts.WriteTotalTimeoutConstant = 1000;

		// configure
		if (SetCommTimeouts(m_hComm, &m_CommTimeouts))
		{
			if (SetCommMask(m_hComm, dwCommEvents))
			{
				if (GetCommState(m_hComm, &m_dcb))
				{
					m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
					if (BuildCommDCBA(tzBaud, &m_dcb))
					{
						if (SetCommState(m_hComm, &m_dcb))
						{
							; // normal operation... continue
						}
						else
						{
							ProcessErrorMessage(("SetCommState()"));
							return FALSE;
						}
					}
					else
					{
						ProcessErrorMessage(("BuildCommDCB()"));
						return FALSE;
					}
				}
				else
				{
					ProcessErrorMessage(("GetCommState()"));
					return FALSE;
				}
			}
			else
			{
				ProcessErrorMessage(("SetCommMask()"));
				return FALSE;
			}
		}
		else
		{
			ProcessErrorMessage(("SetCommTimeouts()"));
			return FALSE;
		}
		// flush the port
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

		// release critical section
		LeaveCriticalSection(&m_csCommunicationSync);

		MY_PRINT_DEBUG("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);

		return TRUE;
	}
	// start/stop comm watching
	//
	// start comm watching
	//
	BOOL StartMonitoring()
	{
		if ((m_Thread = CreateThread(NULL, 0, CommThread, this, 0, NULL)) != INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		MY_PRINT_DEBUG("Thread started\n");
		return TRUE;
	}
	//
	// Restart the comm thread
	//
	BOOL RestartMonitoring()
	{
		ResumeThread(m_Thread);
		MY_PRINT_DEBUG("Thread resumed\n");
		return TRUE;
	}
	//
	// Suspend the comm thread
	//
	BOOL StopMonitoring()
	{
		SuspendThread(m_Thread);
		MY_PRINT_DEBUG("Thread suspended\n");
		return TRUE;
	}
	//
	// Suspend the comm thread
	//
	BOOL CloseMonitoring()
	{
		// if the thread is alive: Kill
		if (m_bThreadAlive)
		{
			do
			{
				SetEvent(m_hShutdownEvent);
			} while (m_bThreadAlive);
			MY_PRINT_DEBUG("Thread ended\n");
		}
		MY_PRINT_DEBUG("Thread closed\n");
		return TRUE;
	}
	//
	// Return the output buffer size
	//
	DWORD GetWriteBufferSize()
	{
		return m_nWriteBufferSize;
	}

	int GetPortNO()
	{
		return m_nPortNr;
	}

	//
	// Return the communication event masks
	//
	DWORD GetCommEvents()
	{
		return m_dwCommEvents;
	}
	//
	// Return the device control block
	//
	DCB GetDCB()
	{
		return m_dcb;
	}

	//
	// Write a string to the port
	//
	void WriteToPort(const CHAR* string)
	{
		assert(m_hComm != 0);

		memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
		m_nWriteBufferLength = (DWORD)strlen(string);
		strcpy(m_szWriteBuffer, string);

		// set event for write
		SetEvent(m_hWriteEvent);
	}
	//
	// Write a string with fixed length to the port
	//
	void WriteToPort(const CHAR* string, DWORD length)
	{
		assert(m_hComm != 0);

		memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
		m_nWriteBufferLength = length;
		memcpy(m_szWriteBuffer, string, m_nWriteBufferLength);

		// set event for write
		SetEvent(m_hWriteEvent);
	}

	static BOOL IsExistsSerialPort(UINT uCommPort)
	{
		CHAR tzPort[MAX_PATH] = { 0 };
		HANDLE hComm = INVALID_HANDLE_VALUE;
		// prepare port strings
		wsprintfA(tzPort, ("\\\\.\\COM%d"), uCommPort);
		// get a handle to the port
		hComm = CreateFileA(tzPort,						// communication port string (COMX)
			GENERIC_READ | GENERIC_WRITE,	// read/write types
			0,								// comm devices must be opened with exclusive access
			NULL,							// no security attributes
			OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
			FILE_FLAG_OVERLAPPED,			// Async I/O
			0);							// template must be 0 for comm devices

		if (hComm == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() != ERROR_FILE_NOT_FOUND)
			{
				// port found
				return TRUE;
			}
			// port not found
			return FALSE;
		}
		CloseHandle(hComm);
		return TRUE;
	}
protected:
	// protected memberfunctions
	//
	// If there is a error, give the right message
	//
	void ProcessErrorMessage(LPCSTR lpErrorText)
	{
		std::string strTemp = ("");

		LPVOID lpMsgBuf = NULL;

		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPSTR)&lpMsgBuf,
			0,
			NULL
		);
		if (lpMsgBuf)
		{
			strTemp.append("WARNING:  ").append(lpErrorText).append(" Failed with the following error: \r\n").append((LPCSTR)lpMsgBuf).append("\nPort: ").append(std::to_string(m_nPortNr)).append("\r\n");
			MessageBoxA(NULL, strTemp.c_str(), ("Application Error"), MB_ICONSTOP);
			MY_PRINT_DEBUG(strTemp.c_str());
			LocalFree(lpMsgBuf);
		}
	}
	//
	//  The CommThread Function.
	//
	static DWORD WINAPI CommThread(LPVOID pParam)
	{
		// Cast the void pointer passed to the thread back to
		// a pointer of CSerialPort class
		CSerialPort* port = (CSerialPort*)pParam;

		// Set the status variable in the dialog class to
		// TRUE to indicate the thread is running.
		port->m_bThreadAlive = TRUE;

		// Misc. variables
		DWORD BytesTransfered = 0;
		DWORD Event = 0;
		DWORD CommEvent = 0;
		DWORD dwError = 0;
		COMSTAT comstat = { 0 };
		BOOL  bResult = TRUE;
		BOOL bTimeout = FALSE;

		// Clear comm buffers at startup
		if (port->m_hComm)		// check if the port is opened
		{
			PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		}
		// begin forever loop.  This loop will run as long as the thread is alive.
		for (;;)
		{

			// Make a call to WaitCommEvent().  This call will return immediatly
			// because our port was created as an async port (FILE_FLAG_OVERLAPPED
			// and an m_OverlappedStructerlapped structure specified).  This call will cause the 
			// m_OverlappedStructerlapped element m_OverlappedStruct.hEvent, which is part of the m_hEventArray to 
			// be placed in a non-signeled state if there are no bytes available to be read,
			// or to a signeled state if there are bytes available.  If this event handle 
			// is set to the non-signeled state, it will be set to signeled when a 
			// character arrives at the port.

			// we do this for each port!

			bResult = WaitCommEvent(port->m_hComm, &Event, &port->m_ov);

			if (!bResult)
			{
				// If WaitCommEvent() returns FALSE, process the last error to determin
				// the reason..
				switch (dwError = GetLastError())
				{
				case ERROR_IO_PENDING:
				{
					// This is a normal return value if there are no bytes
					// to read at the port.
					// Do nothing and continue
					break;
				}
				case ERROR_INVALID_PARAMETER:
				{
					// Under Windows NT, this value is returned for some reason.
					// I have not investigated why, but it is also a valid reply
					// Also do nothing and continue.
					break;
				}
				default:
				{
					// All other error codes indicate a serious error has
					// occured.  Process this error.
					port->ProcessErrorMessage(("WaitCommEvent()"));
					break;
				}
				}
			}
			else
			{
				// If WaitCommEvent() returns TRUE, check to be sure there are
				// actually bytes in the buffer to read.  
				//
				// If you are reading more than one byte at a time from the buffer 
				// (which this program does not do) you will have the situation occur 
				// where the first byte to arrive will cause the WaitForMultipleObjects() 
				// function to stop waiting.  The WaitForMultipleObjects() function 
				// resets the event handle in m_OverlappedStruct.hEvent to the non-signelead state
				// as it returns.  
				//
				// If in the time between the reset of this event and the call to 
				// ReadFile() more bytes arrive, the m_OverlappedStruct.hEvent handle will be set again
				// to the signeled state. When the call to ReadFile() occurs, it will 
				// read all of the bytes from the buffer, and the program will
				// loop back around to WaitCommEvent().
				// 
				// At this point you will be in the situation where m_OverlappedStruct.hEvent is set,
				// but there are no bytes available to read.  If you proceed and call
				// ReadFile(), it will return immediatly due to the async port setup, but
				// GetOverlappedResults() will not return until the next character arrives.
				//
				// It is not desirable for the GetOverlappedResults() function to be in 
				// this state.  The thread shutdown event (event 0) and the WriteFile()
				// event (Event2) will not work if the thread is blocked by GetOverlappedResults().
				//
				// The solution to this is to check the buffer with a call to ClearCommError().
				// This call will reset the event handle, and if there are no bytes to read
				// we can loop back through WaitCommEvent() again, then proceed.
				// If there are really bytes to read, do nothing and proceed.

				bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

				if (comstat.cbInQue == 0)
					continue;
			}	// end if bResult

			// Main wait function.  This function will normally block the thread
			// until one of nine events occur that require action.
			while (port->m_bThreadAlive && (Event = WaitForMultipleObjects(sizeof(port->m_hEventArray) / sizeof(*port->m_hEventArray), port->m_hEventArray, FALSE, WAIT_TIMEOUT/*INFINITE*/)) == WAIT_TIMEOUT);
			if (port->m_bThreadAlive)
			{
				switch (Event)
				{
				case (WAIT_OBJECT_0 + 0):
				{
					// Shutdown event.  This is event zero so it will be
					// the higest priority and be serviced first.

					port->m_bThreadAlive = FALSE;

					// Kill this thread.  break is not needed, but makes me feel better.
					CloseHandle(port->m_Thread);
					ExitThread(0);
					break;
				}
				case (WAIT_OBJECT_0 + 1):	// read event
				{
					GetCommMask(port->m_hComm, &CommEvent);
					if (CommEvent & EV_CTS)
						::SendMessageA(port->m_hWndPort, WM_COMM_CTS_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);
					if (CommEvent & EV_RXFLAG)
						::SendMessageA(port->m_hWndPort, WM_COMM_RXFLAG_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);
					if (CommEvent & EV_BREAK)
						::SendMessageA(port->m_hWndPort, WM_COMM_BREAK_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);
					if (CommEvent & EV_ERR)
						::SendMessageA(port->m_hWndPort, WM_COMM_ERR_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);
					if (CommEvent & EV_RING)
						::SendMessageA(port->m_hWndPort, WM_COMM_RING_DETECTED, (WPARAM)0, (LPARAM)port->m_nPortNr);

					if (CommEvent & EV_RXCHAR)
					{
						// Receive character event from port.
						ReceiveChar(port, comstat);
					}
					break;
				}
				case (WAIT_OBJECT_0 + 2): // write event
				{
					// Write character event from port
					WriteChar(port);
					break;
				}
				} // end switch
			}
		} // close forever loop

		return 0;
	}
	//
	// Character received. Inform the owner
	//
	static void ReceiveChar(CSerialPort* port, COMSTAT comstat)
	{
		BOOL  bRead = TRUE;
		BOOL  bResult = TRUE;
		DWORD dwError = 0;
		DWORD BytesRead = 0;
		unsigned char RXBuff = 0x00;

		for (;;)
		{
			// Gain ownership of the comm port critical section.
			// This process guarantees no other part of this program 
			// is using the port object. 

			EnterCriticalSection(&port->m_csCommunicationSync);

			// ClearCommError() will update the COMSTAT structure and
			// clear any other errors.

			bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

			LeaveCriticalSection(&port->m_csCommunicationSync);

			// start forever loop.  I use this type of loop because I
			// do not know at runtime how many loops this will have to
			// run. My solution is to start a forever loop and to
			// break out of it when I have processed all of the
			// data available.  Be careful with this approach and
			// be sure your loop will exit.
			// My reasons for this are not as clear in this sample 
			// as it is in my production code, but I have found this 
			// solutiion to be the most efficient way to do this.

			if (comstat.cbInQue == 0)
			{
				// break out when all bytes have been read
				MY_PRINT_DEBUG("\n(comstat.cbInQue == 0)%s:%d\n", __func__, __LINE__);
				break;
			}

			EnterCriticalSection(&port->m_csCommunicationSync);

			if (bRead)
			{
				bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
					&RXBuff,				// RX Buffer Pointer
					1,					// Read one byte
					&BytesRead,			// Stores number of bytes read
					&port->m_ov);		// pointer to the m_ov structure
				// deal with the error code 
				if (!bResult)
				{
					switch (dwError = GetLastError())
					{
					case ERROR_IO_PENDING:
					{
						// asynchronous i/o is still in progress 
						// Proceed on to GetOverlappedResults();
						bRead = FALSE;
						break;
					}
					default:
					{
						// Another error has occured.  Process this error.
						port->ProcessErrorMessage(("ReadFile()"));
						break;
					}
					}
				}
				else
				{
					// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
					bRead = TRUE;
				}
			}  // close if (bRead)

			if (!bRead)
			{
				MY_PRINT_DEBUG("\n%s:%d\n", __func__, __LINE__);
				bRead = TRUE;
				bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
					&port->m_ov,		// Overlapped structure
					&BytesRead,		// Stores number of bytes read
					TRUE); 			// Wait flag

				// deal with the error code 
				if (!bResult)
				{
					port->ProcessErrorMessage(("GetOverlappedResults() in ReadFile()"));
				}
			}  // close if (!bRead)

			LeaveCriticalSection(&port->m_csCommunicationSync);

			// notify parent that a byte was received
			::SendMessageA(port->m_hWndPort, WM_COMM_RXCHAR, (WPARAM)RXBuff, (LPARAM)port->m_nPortNr);
			if (RXBuff == '\r')
			{
				MY_PRINT_DEBUG("\n");
			}
			else
			{
				MY_PRINT_DEBUG("%c", RXBuff);
			}
		}// end forever loop
	}

	//
	// Write a character.
	//
	static void WriteChar(CSerialPort* port)
	{
		BOOL bWrite = TRUE;
		BOOL bResult = TRUE;

		DWORD BytesSent = 0;

		ResetEvent(port->m_hWriteEvent);

		// Gain ownership of the critical section
		EnterCriticalSection(&port->m_csCommunicationSync);

		if (bWrite)
		{
			// Initailize variables
			port->m_ov.Offset = 0;
			port->m_ov.OffsetHigh = 0;

			// Clear buffer
			PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

			bResult = WriteFile(port->m_hComm,							// Handle to COMM Port
				port->m_szWriteBuffer,					// Pointer to message buffer in calling finction
				port->m_nWriteBufferLength,				// Length of message to send
				&BytesSent,								// Where to store the number of bytes sent
				&port->m_ov);							// Overlapped structure

			// deal with any error codes
			if (!bResult)
			{
				DWORD dwError = GetLastError();
				switch (dwError)
				{
				case ERROR_IO_PENDING:
				{
					// continue to GetOverlappedResults()
					BytesSent = 0;
					bWrite = FALSE;
					break;
				}
				default:
				{
					// all other error codes
					port->ProcessErrorMessage(("WriteFile()"));
				}
				}
			}
			else
			{
				LeaveCriticalSection(&port->m_csCommunicationSync);
			}
		} // end if(bWrite)

		if (!bWrite)
		{
			bWrite = TRUE;

			bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
				&port->m_ov,		// Overlapped structure
				&BytesSent,		// Stores number of bytes sent
				TRUE); 			// Wait flag

			LeaveCriticalSection(&port->m_csCommunicationSync);

			// deal with the error code 
			if (!bResult)
			{
				port->ProcessErrorMessage(("GetOverlappedResults() in WriteFile()"));
			}
		} // end if (!bWrite)

		// Verify that the data size send equals what we tried to send
		if (BytesSent != port->m_nWriteBufferLength)
		{
			MY_PRINT_DEBUG("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, port->m_nWriteBufferLength);
		}
	}
	// thread
	HANDLE			m_Thread = INVALID_HANDLE_VALUE;

	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync = { 0 };
	BOOL				m_bThreadAlive = FALSE;

	// handles
	HANDLE				m_hShutdownEvent = INVALID_HANDLE_VALUE;
	HANDLE				m_hComm = INVALID_HANDLE_VALUE;
	HANDLE				m_hWriteEvent = INVALID_HANDLE_VALUE;

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3] = { INVALID_HANDLE_VALUE };

	// structures
	OVERLAPPED			m_ov = { 0 };
	COMMTIMEOUTS		m_CommTimeouts = { 0 };
	DCB					m_dcb = { 0 };

	// owner window
	HWND				m_hWndPort = NULL;

	// misc
	UINT				m_nPortNr = 0;
	CHAR				m_szWriteBuffer[USHRT_MAX] = { 0 };
	DWORD				m_nWriteBufferLength = 0;
	DWORD				m_dwCommEvents = 0;
	DWORD				m_nWriteBufferSize = 0;

public:
	static CSerialPort* Inst() {
		static CSerialPort CSerialPortInstance;
		return &CSerialPortInstance;
	}
};

#endif __SERIALPORT_H__


