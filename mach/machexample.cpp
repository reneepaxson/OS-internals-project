//
//  Class that provides a wrapper to send and receive Mach messages
//
//  dennisbabkin.com
//

#ifndef MachComm_h
#define MachComm_h

#include <stdio.h>
#include <string>
#include <new>

#include "locks.h"				//Our implementation of the synchronization locks

#include <mach/mach_port.h>
#include <launch.h>

#include <mach/boolean.h>
#include <servers/bootstrap.h>

#include <mach/mach.h>
#include <mach/mach_error.h>



struct MACH_MSG_BASE
{
	//Base structure for sending a Mach message.
	//It must always start with the header, and
	//have the following members if we're sending 
	//a block of memory to another process.
	//
	mach_msg_header_t hdr;
	
	mach_msg_body_t body;
	mach_msg_ool_descriptor_t data;
	mach_msg_type_number_t count;
	
};


struct MACH_MSG_RCV
{
	//Base structure for receiving a Mach message.
	//
	MACH_MSG_BASE msg;

	//Space for mach_msg_trailer_t, which could be of a variable size.
	//It is provided by the kernel. Let's give it some extra space.
	//
	char trailing_bytes[sizeof(void*) * 8];
};

#define INFINITE 0


///Possible results of receiving a Mach message
///
enum MACH_RCV_RES
{
	MRRES_Error = -1,
	MRRES_Success,
	MRRES_TimedOut,
};



///A helper class for receiving binary data of an arbitrary size
///from a Mach port. You technically do not need it for IPC.
///I'm providing it only for a convenience of memory allocation
///and proper de-allocation using the RAII technique...
///
///IMPORTANT: Please be aware that this class will be a bottleneck 
///           for large data transmissions since it uses one extra 
///           heap allocation and memcpy to store the data!
///
struct MACH_RCV_DATA
{
	MACH_RCV_DATA()
	{
	}
	
	~MACH_RCV_DATA()
	{
		//Free data
		freeData();
	}

	
	///Free allocated data
	void freeData()
	{
		if(_pData)
		{
			delete[] _pData;
			_pData = nullptr;
			
			_szcb = 0;
		}
	}
	
	
	///Retrieve pointer to the byte array stored in this class.
	///IMPORTANT: DO NOT free it outside of this class!
	///'pcbOutSz' = if not 0, will receive the size of returned data in bytes
	///RETURN:
	///     - Pointer to data - do not release, or change it!
	///     - 0 if no data is stored here
	const void* getData(size_t* pcbOutSz = nullptr)
	{
		if(pcbOutSz)
			*pcbOutSz = _szcb;
		
		return _pData;
	}
	
	
	///Set data in this class by copying it from 'pMem'
	///'szcb' = size of 'pMem' in bytes
	///RETURN:
	///     - true if no errors
	bool setData(const void* pMem, size_t szcb)
	{
		//Free old data first
		freeData();
		
		if(pMem)
		{
			//Reserve mem
			_pData = new (std::nothrow) char[szcb];
			if(!_pData)
			{
				//No memory
				assert(false);
				
				return false;
			}
			
			memcpy((void*)_pData, pMem, szcb);
			_szcb = szcb;
		}
		
		return true;
	}
	
	
private:
	
	//Copy constructor and assignments are NOT available!
	MACH_RCV_DATA(const MACH_RCV_DATA& s) = delete;
	MACH_RCV_DATA& operator = (const MACH_RCV_DATA& s) = delete;


private:
	const char* _pData = nullptr;
	size_t _szcb = 0;                        //Size of 'pData' in bytes

};



///Defines the function of the MachComm class
///
enum MACH_ENDPOINT_TYPE
{
	MET_Unknown,
	
	MET_SERVER,         //Class acts as the IPC server
	MET_CLIENT,         //Class acts as one of the IPC clients
};




///This is the main class that provides the IPC via Mach messages
///
struct MachComm
{
	MachComm()
	{
		//Constructor
	}
	
	~MachComm()
	{
		//Destructor
		uninitialize();
	}

	

	///Initialize this class for IPC
	///INFO: Does nothing, if this class was already initialized.
	///'type' = type of the end-point to initialize this class as (it cannot be changed later.)
	///'pPortName' = unique port name (in reverse DNS format) - it must match what is specified 
	///              in the MachServices key for the launchd.plist file that is used to launch 
	///              the IPC server in a deamon:
	///               https://www.manpagez.com/man/5/launchd.plist/
	///RETURN:
	///     - true if no errors
	bool initialize(MACH_ENDPOINT_TYPE type,
					const char* pPortName)
	{
		bool bRes = false;
		
		if(true)
		{
			kern_return_t nKRet;
			
			//Act from within a lock
			WRITER_LOCK wrl(_lock);
			
			//See if we haven't initialized previously
			if(_port == MACH_PORT_NULL)
			{
				//Get the port for communication
				if(type == MET_SERVER)
				{
					//Server - get port number from the launchd.plist
					nKRet = _createServerPort(pPortName,
												&_port);
				}
				else if(type == MET_CLIENT)
				{
					//Client - find previously created port by the server
					nKRet = _findServerPort(pPortName,
											&_port);
				}
				else
				{
					//Wrong endpoint type
					nKRet = KERN_INVALID_ARGUMENT;
				}
				
				if(nKRet == KERN_SUCCESS)
				{
					//Remember the port info
					assert(_port);
					
					_strPortName = pPortName;
					_endpoint = type;
					
					//Done!
					bRes = true;
				}
			}
			else
			{
				//Already initialized
				bRes = true;
			}
		}
		
		return bRes;
	}
	
	
	///Uninitialize this class to free resources
	///INFO: Does nothing if it was already uninitialized.
	///RETURN:
	///     - true if no errors
	bool uninitialize()
	{
		bool bRes = false;
		
		if(true)
		{
			//Act from within a lock
			WRITER_LOCK wrl(_lock);
			
			bRes = _uninit();
		}
		
		return bRes;
	}
	
	
	///Send message to the IPC server
	///'pPtrMsg' = pointer to the message to send (as a byte array)
	///'ncbMsgSz' = size of 'pPtrMsg' in bytes
	///'nmsTimeout' = timeout for sending in ms, or INFINITE (or 0) for no timeout
	///RETURN:
	///     - KERN_SUCCESS if success
	///     - Other if error
	kern_return_t sendMsg(const void* pPtrMsg,
							mach_msg_size_t ncbMsgSz,
							unsigned int nmsTimeout = INFINITE)
	{
		kern_return_t nKRet;
		
		MACH_MSG_BASE msg = {};
		if(!_set_msg_for_sending_mem_ptr(msg, pPtrMsg, ncbMsgSz))
		{
			//initialize() was not called successfully prior to this call
			assert(false);
			
			return KERN_NOT_IN_SET;
		}
		
		nKRet = mach_msg(&msg.hdr,
							MACH_SEND_MSG |
							(nmsTimeout != INFINITE ? MACH_SEND_TIMEOUT : 0),
							sizeof(msg),
							0,
							MACH_PORT_NULL,
							nmsTimeout != INFINITE ? nmsTimeout : MACH_MSG_TIMEOUT_NONE,
							MACH_PORT_NULL);
			
		return nKRet;
	}
	
	
	
	///Receive message from the IPC client
	///'rcvData' = object to receive data in (it will be filled only if return is a success)
	///'puiOutClientPID' = if not 0, receives PID of the client process that sent this message, or 0 if error
	///'nmsTimeout' = how long to wait for the message to arrive in ms, or INFINITE (or 0) to wait for as long as it takes
	///'pOutKernErr' = if not 0, and an error occurs, receives a kernel error code of a fault
	///RETURN:
	///     - Result of the operation
	MACH_RCV_RES receiveMsg(MACH_RCV_DATA& rcvData,
							pid_t* puiOutClientPID = nullptr,
							unsigned int nmsTimeout = INFINITE,
							kern_return_t* pOutKernErr = nullptr)
	{
		MACH_RCV_RES res = MRRES_Error;
		kern_return_t nKRet;
		
		mach_port_t port;		
		pid_t pid_client = 0;

		if(true)
		{
			//Act from within a lock
			READER_LOCK rdl(_lock);
			
			port = _port;
		}

		//Only if we have a port
		if(port != MACH_PORT_NULL)
		{
			MACH_MSG_RCV rcv = {};
			rcv.msg.hdr.msgh_local_port = port;
			rcv.msg.hdr.msgh_size = sizeof(rcv);

			//Here we specify flags for the type of a trailing header that we want to receive,
			//and that we want to receive a special "audit" trailer that should contain
			//the process ID of a client that sent this message. That last part is
			//important for us for security reasons, so that we can vet the sending process!
			//
			nKRet = mach_msg(&rcv.msg.hdr,
								MACH_RCV_MSG |
								MACH_RCV_LARGE |
								(nmsTimeout != INFINITE ? MACH_RCV_TIMEOUT : 0) |
								MACH_RCV_TRAILER_TYPE(MACH_MSG_TRAILER_FORMAT_0) |
								MACH_RCV_TRAILER_ELEMENTS(MACH_RCV_TRAILER_AUDIT),
								0,
								sizeof(rcv),
								port,
								nmsTimeout != INFINITE ? nmsTimeout : MACH_MSG_TIMEOUT_NONE,
								MACH_PORT_NULL);
			
			if(nKRet == KERN_SUCCESS)
			{
				//We got the message, see if we have valid data in it...
				const void* pData = rcv.msg.data.address;
				mach_msg_size_t ncbSzData = rcv.msg.data.size;
				
				//INFO: It is important to check for MACH_MSGH_BITS_COMPLEX,
				//      as it will tell us if 'rcv.msg.data.address' was marshalled
				//      by the kernel for us, and thus we can read from it!
				if(pData &&
					(rcv.msg.hdr.msgh_bits & MACH_MSGH_BITS_COMPLEX))
				{
					//Copy raw data
					//
					//IMPORTANT: I am using my helper class to store the data that I receive
					//           from the Mach port. Keep in mind that doing this will be a
					//           definite bottleneck in speed for larger data transmissions!
					//
					//           In that case, my advice is to keep the 'pData' pointer and
					//           to use it directly when you need to read the received data.
					//           After that, make sure to free that memory with a call to 
					//           vm_deallocate(), as I showed below, to avoid memory leaks.
					//
					if(rcvData.setData(pData, ncbSzData))
					{
						//Do compile-time check of our trailing buffer - and make sure that
						//it is large enough to contain the needed trailing header.
						static_assert(sizeof(rcv.trailing_bytes) >= sizeof(mach_msg_audit_trailer_t), "Size of the trailing buffer is too small!");
						
						mach_msg_audit_trailer_t* pTrlr = (mach_msg_audit_trailer_t*)rcv.trailing_bytes;
						
						if(pTrlr->msgh_trailer_size == sizeof(*pTrlr))
						{					
							//Get PID of the sending process
							pid_client = pTrlr->msgh_audit.val[5];
							
							//Done
							res = MRRES_Success;
						}
						else
						{
							//Wrong size returned
							nKRet = KERN_MEMORY_ERROR;
							assert(false);
						}
					}
					else
					{
						//No memory
						nKRet = KERN_MEMORY_FAILURE;
					}
					
					
					//And free the memory that we received from the kernel
					kern_return_t nKRet2 = vm_deallocate(mach_task_self(),
														(vm_address_t)pData,
														ncbSzData);
					if(nKRet2 != KERN_SUCCESS)
					{
						//Error
						nKRet = nKRet2;
						assert(false);
						res = MRRES_Error;
					}
					
					//Reset the pointer & size since we just freed the memory.
					//We technically don't need to do this, but it is safer
					//this way in case of a possible use-after-free that may follow ...
					//
					pData = nullptr;
					ncbSzData = 0;
				}
				else
				{
					//Bad data received
					nKRet = KERN_INVALID_VALUE;
				}
			}
			else if(nKRet == MACH_RCV_TIMED_OUT)
			{
				//Timed out
				res = MRRES_TimedOut;
			}
		}
		else
		{
			//initialize() was not called successfully prior to this call
			assert(false);
			
			nKRet = KERN_NOT_IN_SET;
		}

		if(pOutKernErr)
			*pOutKernErr = nKRet;
		if(puiOutClientPID)
			*puiOutClientPID = pid_client;

		return res;
	}
	
	
	
private:
	
	
	///IMPORTANT: Must be called from within a lock!
	///RETURN:
	///     - true if no errors
	bool _uninit()
	{
		bool bRes = false;
		
		kern_return_t nKRet;
		
		//See if we haven't uninitialized previously
		if(_port != MACH_PORT_NULL)
		{
			if(_endpoint == MET_SERVER)
			{
				//Server
				nKRet = _removeServerPort(_strPortName.c_str());
				if(nKRet == KERN_SUCCESS)
				{
					//Done
					bRes = true;
				}
				else
				{
					//Failed
					assert(false);
				}
			}
			else if(_endpoint == MET_CLIENT)
			{
				//Client - nothing to do
				bRes = true;
			}
			else
			{
				//Bad end-point type
				assert(false);
			}
			
			//Reset internal variables
			_endpoint = MET_Unknown;
			_port = MACH_PORT_NULL;
			_strPortName.clear();
		}
		else
		{
			//Already un-initialized
			bRes = true;
		}

		return bRes;
	}
	
	
	///Prepare 'msg' for sending
	///'pPtr' = memory pointer for the local buffer to send
	///'szcb' = size of 'pPtr' in bytes
	///RETURN:
	///     - true if port is available
	bool _set_msg_for_sending_mem_ptr(MACH_MSG_BASE& msg,
										const void* pPtr,
										mach_msg_size_t szcb)
	{
		mach_port_t port;
		
		if(true)
		{
			//Act from within a lock
			READER_LOCK rdl(_lock);
			
			port = _port;
		}

		if(port == MACH_PORT_NULL)
		{
			//No port
			return false;
		}
		
		msg = {};
	
		//NOTE that the MACH_MSGH_BITS_COMPLEX flag instructs the kernel
		//     to marshal the memory buffer pointed by 'msg.data.address' into the
		//     target process where the Mach message will be received.
		//
		msg.hdr.msgh_remote_port = port;
		msg.hdr.msgh_local_port = MACH_PORT_NULL;
		msg.hdr.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND) |
		                    MACH_MSGH_BITS_COMPLEX;
		msg.hdr.msgh_size = sizeof(msg);
		
		msg.body.msgh_descriptor_count = 1;
		msg.data.address = (void*)pPtr;
		msg.data.size = szcb;
		msg.data.copy = MACH_MSG_VIRTUAL_COPY;
		msg.data.type = MACH_MSG_OOL_DESCRIPTOR;
		msg.count = msg.data.size;

		return true;
	}

	
	
	///Associate a unique name with a port
	///INFO: This name can be used by the IPC client to find the port
	///'pName' = unique port name (in reverse DNS format) - it must match what is specified in the 
	///          MachServices key for the launchd.plist file that is used to launch the deamon:
	///           https://www.manpagez.com/man/5/launchd.plist/
	///'pOutPort' = receives associated port, if success -- call _removeServerPort() to de-associate it
	///RETURN:
	///     - KERN_SUCCESS if success, otherwise
	///     - Kernel error code
	kern_return_t _createServerPort(const char* pName,
									mach_port_t* pOutPort)
	{
		kern_return_t nKRet = KERN_INVALID_ARGUMENT;
		assert(pOutPort);
		
		mach_port_t port = MACH_PORT_NULL;

		if(pName &&
			pName[0])
		{
			//Retrieve the receive rights for our named port from the launchd
			nKRet = bootstrap_check_in(bootstrap_port,
										pName,
										&port);
			
			if(nKRet != KERN_SUCCESS)
			{
				//Error
				//BOOTSTRAP_UNKNOWN_SERVICE = 1102 = 0x44E - if launchd.plist for this daemon does not contain 'pName'
				assert(false);
			}
		}
		
		*pOutPort = port;
		
		return nKRet;
	}
	
	
	
	///Find previously created port by 'pName'
	///'pName' = unique port name (in reverse DNS format) - it must match what is specified 
	//           in the MachServices key for the launchd.plist file that was used to launch the deamon:
	///           https://www.manpagez.com/man/5/launchd.plist/
	///'pOutPort' = receives associated port, if success (there's no need to release it)
	///RETURN:
	///     - KERN_SUCCESS if success, otherwise
	///     - Kernel error code
	kern_return_t _findServerPort(const char* pName,
									mach_port_t* pOutPort)
	{
		kern_return_t nKRet = KERN_INVALID_ARGUMENT;
		assert(pOutPort);
		
		mach_port_t port = MACH_PORT_NULL;

		if(pName &&
			pName[0])
		{
			nKRet = bootstrap_look_up(bootstrap_port, pName, &port);
			if(nKRet != KERN_SUCCESS)
			{
				//BOOTSTRAP_UNKNOWN_SERVICE = 1102 - if there's no such port name
				assert(false);
			}
		}

		*pOutPort = port;

		return nKRet;
	}
	
	
	
	///De-associate 'pName' port - that was previously associated with the _createServerPort() function
	///RETURN:
	///     - KERN_SUCCESS if success, otherwise
	///     - Kernel error code
	kern_return_t _removeServerPort(const char* pName)
	{
		kern_return_t nKRet = KERN_INVALID_ARGUMENT;
		
		if(pName &&
			pName[0])
		{
			//INFO: The following is needed only if port was created here.
			//      In our case the port was created by launchd, that owns it...
			
			//mach_port_t* p_null_port = nullptr;
			nKRet = KERN_SUCCESS;   //bootstrap_check_in(bootstrap_port, pName, p_null_port);
			if(nKRet != KERN_SUCCESS)
			{
				//BOOTSTRAP_SERVICE_ACTIVE == 1103 - if port is still active
				assert(false);
			}
		}
		
		return nKRet;
	}
	
	
	
private:
	//Copy constructor and assignments are NOT available!
	MachComm(const MachComm& s) = delete;
	MachComm& operator = (const MachComm& s) = delete;
	
private:
	
	RDR_WRTR _lock;                                 //Lock for accessing this class' private members
	
	MACH_ENDPOINT_TYPE _endpoint = MET_Unknown;     //Type of the end-point for this class
	mach_port_t _port = MACH_PORT_NULL;             //Port used for IPC
	std::string _strPortName;                       //Name of the port
	
};




#endif /* MachComm_h */