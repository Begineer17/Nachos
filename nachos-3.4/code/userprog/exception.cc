// exception.cc 

//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.



#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32 // Do dai quy uoc cho file name

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	Cac quy dinh su dung thanh ghi:
//
// 	   1. Thanh ghi r2: Chua ma lenh syscall va ket qua thuc hien o syscall se tra nguoc ve
// 	   2. Thanh ghi r4: Tham so thu nhat
//	   3. Thanh ghi r5: Tham so thu hai
//	   4. Thanh ghi r6: Tham so thu ba	   
//	   5. Thanh ghi r7: Tham so thu tu
//
// 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void IncreaseCounter()
{
	int counter = machine->ReadRegister(PCReg);
   	machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, counter);
   	machine->WriteRegister(NextPCReg, counter + 4);
}

char* User2System(int virtAddr, int limit)
{
	int i; 
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1];
	if (kernelBuf == NULL)
		return kernelBuf;
		
	memset(kernelBuf, 0, limit + 1);
	
	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}


// Ham xu ly ngoai le runtime Exception va system call
void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

	// Bien toan cuc cho lop SynchConsole
	
	
	//Bat dau
	switch (which) {
	case NoException:
		return;

	case PageFaultException:
		DEBUG('a', "\n No valid translation found");
		printf("\n\n No valid translation found");
		interrupt->Halt();
		break;

	case ReadOnlyException:
		DEBUG('a', "\n Write attempted to page marked read-only");
		printf("\n\n Write attempted to page marked read-only");
		interrupt->Halt();
		break;

	case BusErrorException:
		DEBUG('a', "\n Translation resulted invalid physical address");
		printf("\n\n Translation resulted invalid physical address");
		interrupt->Halt();
		break;

	case AddressErrorException:
		DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		interrupt->Halt();
		break;

	case OverflowException:
		DEBUG('a', "\nInteger overflow in add or sub.");
		printf("\n\n Integer overflow in add or sub.");
		interrupt->Halt();
		break;

	case IllegalInstrException:
		DEBUG('a', "\n Unimplemented or reserved instr.");
		printf("\n\n Unimplemented or reserved instr.");
		interrupt->Halt();
		break;

	case NumExceptionTypes:
		DEBUG('a', "\n Number exception types");
		printf("\n\n Number exception types");
		interrupt->Halt();
		break;

	case SyscallException:
		switch (type) {
			case SC_Halt:
				// Input: None
				// Purpose: Shutdown allert
				DEBUG('a', "\nShutdown. ");
				printf("\nShutdown. ");
				interrupt->Halt();
				return;

			case SC_ReadInt:
	            // Input: None
	            // Purpose: Return integer read from console
	            char* buffer = new char[255 + 1];
	            int noBytes = gSynchConsole->Read(buffer, 255); // write to buffer at most 255 characters
	            int num = 0; // returned value
	                
	            /* Process buffer to integer */
	    
	            // check negative                     
	            int firstIndex = 0;
	            int lastIndex = 0;
	            if(buffer[0] == '-')
	            {
	                firstIndex = 1;
	                lastIndex = 1;                        			   		
	            }
	            
	            // check validity
	            for(int i = firstIndex; i < noBytes; i++)					
	            {
	                if(buffer[i] < '0' && buffer[i] > '9' || buffer[i] == '.')
	                {
	                    DEBUG('a', "\nNot a valid integer. ");
				        printf("\nNot a valid integer. ");
	                    machine->WriteRegister(2, 0);
	                    IncreaseCounter();
	                    delete buffer;
	                    return;
	                }
	                lastNumIndex = i;    
	            }			
	            
	            for(int i = firstIndex; i<= lastIndex; i++) num = num * 10 + (int)(buffer[i] - 48); 
	            
	            if(buffer[0] == '-') num = -num;
	            
	            machine->WriteRegister(2, number);
	            increaseCounter();
	            delete buffer;
	            return;		


			case SC_PrintInt:
	            int number = machine->ReadRegister(4);
	            int noOfNum = 0;
	            int firstIndex = 0; 
	    
	            if(number < 0)
	            {
	                number = -number;
	                firstNumIndex = 1; 
	            } 	
	            
	            for (int i = number; i > 0; i/=10) noOfnum += 1;
	            
	            // create buffer to print to console
	            char* buffer;
	            buffer = new char[255 + 1];
	            for(int i = firstIndex + noOfNum - 1; i >= firstIndex; i--)
	            {
	                buffer[i] = (char)((number % 10) + 48);
	                number /= 10;
	            }
	            if(number < 0)
	            {
	                buffer[0] = '-';
	                buffer[noOfNum + 1] = 0;
	                noOfNum += 1;
	            }
	            else buffer[noOfNum] = 0;	
	            gSynchConsole->Write(buffer, noOfNum);
	            delete buffer;
	            IncreaseCounter();
	            return;        			

			case SC_ReadChar:
				char* buffer = new char[255];
				int numBytes = gSynchConsole->Read(buffer, 255);

				if(numBytes == 1) machine->WriteRegister(2, buffer[0]);
				else
				{
					printf("Invalid input!");
					DEBUG('a', "\nInvalid input");
					machine->WriteRegister(2, 0);
				}

				delete buffer;
				IncreaseCounter();
				return;

			case SC_PrintChar:
				char c = (char)machine->ReadRegister(4);
				gSynchConsole->Write(&c, 1);
				IncreaseCounter();
				return;

			case SC_ReadString:
				int virtualAddress, len;
				char* buffer;
				virtualAddress = machine->ReadRegister(4); // buffer address
				len = machine->ReadRegister(5); 
				buffer = User2System(virtualAddress, len); 
				gSynchConsole->Read(buffer, len); 
				System2User(virtualAddress, len, buffer); 
				delete buffer; 
				IncreaseCounter();
				return;

			case SC_PrintString:
				int virtAddr;
				char* buffer;
				virtAddr = machine->ReadRegister(4);
				buffer = User2System(virtAddr, 255); 
				int length = 0;
				while (buffer[length] != 0) length++; 
				gSynchConsole->Write(buffer, length + 1); 
				delete buffer; 
				IncreaseCounter(); // Tang Program Counter 
				return;
		}
	}
}