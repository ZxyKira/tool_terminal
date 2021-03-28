/*-----------------------------------------------------------------------------------------
 *    File Name   :terminal.c
 *    Version     :V1.0.0
 *    Create Date :2020-10-15
 *    Modufy Date :2020-10-15
 *    Information :
 */
#include <string.h> 
#include <stdio.h>
#include "terminal.h"
#include "terminal_entity.h"


/*-----------------------------------------------------------------------------------------
 *    Parameter
 */

/*-----------------------------------------------------------------------------------------
 *    Extern Function/Variable
 */

/*-----------------------------------------------------------------------------------------
 *    Local Type/Structure
 */

/*-----------------------------------------------------------------------------------------
 *    Local Variable
 */

/*-----------------------------------------------------------------------------------------
 *    Variable
 */
const char tool_terminal_default_perfix[] = "terminal-> ";


/*-----------------------------------------------------------------------------------------
 *    Inline Function
 */
/*
static inline bool terminal_compairTerminalCommand(const tool_terminal_command_t *src, const tool_terminal_command_t *dst){
	return memcmp(src, dst, sizeof(tool_terminal_command_t))?false:true;
}
*/

static inline bool terminal_compairCommand(const tool_terminal_command_t *dst, const char* command){
	if(dst->command == 0x00000000)
		return false;
	else
		return strcmp(command, dst->command)?false:true;
}

static inline void terminal_copyTerminalCommand(tool_terminal_command_t* dst, const tool_terminal_command_t* src){
	memcpy(dst, src, sizeof(tool_terminal_command_t));
}

/*-----------------------------------------------------------------------------------------
 *    Local Function
 */
static int terminal_insertCharToBuffer(terminal_handle_t* pHandle, char ch){
	if(pHandle->bufferPointer >= pHandle->readBufferSize)
		return -1;
	
	if((ch == '\n') | (ch == '\n')){
		pHandle->readBuffer[pHandle->bufferPointer] = '\0';
		pHandle->bufferPointer++;
		return 1;
	}
	else{
		pHandle->readBuffer[pHandle->bufferPointer] = ch;
		pHandle->bufferPointer++;
		return 0;
	}
}

static uint32_t terminal_convertArgs(terminal_handle_t* pHandle){
	uint32_t args = 0;
	int i;
	pHandle->argv[args] = &pHandle->readBuffer[0];
		
	for(i=0; i<pHandle->bufferPointer; i++){
		switch(pHandle->readBuffer[i]){
			case 0:
			case 10:
			case 13:
				++args;
				return args;
				
			case ' ':
				pHandle->readBuffer[i] = '\0';
				++args;
				pHandle->argv[args] = &pHandle->readBuffer[i+1];
				break;
				
			default:
				break;
		}
		
	}
	return args;
}

static void terminal_doCommand(terminal_handle_t* pHandle){
	bool isHandle = false;
	int args = terminal_convertArgs(pHandle);
	char** argv= pHandle->argv;
	tool_terminal_command_t* commands = pHandle->Abstract.getCommands();
	int i;
	for(i=0; i<pHandle->Abstract.getCommandsSize(); i++){
		if(terminal_compairCommand(&commands[i], argv[0])){
			if(commands[i].handle != 0)
				isHandle = commands[i].handle(&pHandle->Abstract.Xfer, commands[i].userData, args, argv);
			else
				isHandle = false;
				
			break;
		}
		
	}
	
	if(isHandle == false){
		pHandle->Abstract.Event.onUnknownCommand(&pHandle->Abstract.Xfer, args, argv);
	}
}



/*-----------------------------------------------------------------------------------------
 *    Public Function - API
 */
bool terminal_init(tool_terminal_handle_t* handle, const tool_terminal_config_t* config){
	if(!handle)
		return false;
	
	if(!config)
		return false;
	
	memset(handle, 0x00, sizeof(tool_terminal_handle_t));
	
	/* copy Abstract function to handle memory */
	memcpy(&handle->xfer, &config->xferApi, sizeof(tool_terminal_xfer_api_t));
	
	handle->commandList.command = config->commandBuffer.buffer;
	handle->commandList.size = config->commandBuffer.size;
	
	handle->handleBuffer.buffer = config->handleBuffer.buffer;
	handle->handleBuffer.size = config->handleBuffer.size;
	
	if(config->prefix)
		handle->prefix = config->prefix;
	else
		handle->prefix = tool_terminal_default_perfix;
	
	
	tool_fifo_config_t fifo_cfg;
	fifo_cfg.buffer = config->inputBuffer.buffer;
  fifo_cfg.count = config->inputBuffer.size;
  fifo_cfg.itemSize = sizeof(uint8_t);
  tool_fifo_init(&handle->inputBuffer, &fifo_cfg);
  
	return true;
}

bool terminal_setBuffer(terminal_handle_t* pHandle, void* buffer, uint32_t bufferSize){
	if(pHandle->status != 0)
		return false;
	
	pHandle->readBuffer = (char*)buffer;
	pHandle->readBufferSize = bufferSize;
	pHandle->bufferPointer = 0;
	return true;
}

bool terminal_addCommand(terminal_handle_t* pHandle, const tool_terminal_command_t command){
	uint32_t cmdBufSize = pHandle->Abstract.getCommandsSize();
	tool_terminal_command_t* pCmdBuf = pHandle->Abstract.getCommands();
	int i;
	
	/* filter duplicates command */
	for(i=0; i<cmdBufSize; i++){
		if(pCmdBuf[i].command == 0)
			/* this command space is empty */
			continue;
		
		if(terminal_compairCommand(&pCmdBuf[i], command.command))
			/* command is duplicates */
			return false;
	}
	
	/* try to find a empty space to write */
	for(i=0; i<cmdBufSize; i++){
		if(pCmdBuf[i].command == 0){
			terminal_copyTerminalCommand(&pCmdBuf[i], &command);
			
			/* write successful */
			return true;
		}
	}
	
	/* write fail, not found an empty space */
	return false;
}

bool terminal_removeCommand(terminal_handle_t* pHandle, const tool_terminal_command_t command){
	uint32_t cmdBufSize = pHandle->Abstract.getCommandsSize();
	tool_terminal_command_t* pCmdBuf = pHandle->Abstract.getCommands();
	int i;
	
	/* filter duplicates command */
	for(i=0; i<cmdBufSize; i++){
		if(pCmdBuf[i].command == 0)
			/* this command space is empty */
			continue;
		
		if(terminal_compairCommand(&pCmdBuf[i], command.command)){
			memset(&pCmdBuf[i], 0x00, sizeof(tool_terminal_command_t));
			
			/* remove successful */
			return true;
		}
	}
	
	/* remove fail, command not found */
	return false;
}

bool terminal_start(terminal_handle_t* pHandle){
	if(pHandle->status != 0)
		return false;
	
	if(pHandle->readBuffer == 0)
		return false;
	
	if(pHandle->readBufferSize == 0)
		return false;
	
	pHandle->status = 1;
	
	char ch;
	int fResult = 0;
	terminal_writePrefix(pHandle);
	while(pHandle->status){
		if(fResult == 0){
			/* read a ch */
			ch = pHandle->Abstract.Xfer.getCh();
			fResult = terminal_insertCharToBuffer(pHandle, ch);
		}else if(fResult == 1){
			/* read line finish */
			terminal_doCommand(pHandle);
			
			/* clear read buffer */
			terminal_clearInputBuffer(pHandle);
			pHandle->Abstract.Xfer.sendString("\n");
			terminal_writePrefix(pHandle);
			fResult = 0;
		}else{
			/* out of memory range */
			pHandle->Abstract.Event.onOutOfBuffer(&pHandle->Abstract.Xfer);
			/* clear read buffer */
			terminal_clearInputBuffer(pHandle);
			pHandle->Abstract.Xfer.sendString("\n");
			terminal_writePrefix(pHandle);
			fResult = 0;
		}
	}
	return true;
}

bool terminal_stop(terminal_handle_t* pHandle){
	if(pHandle->status != 0){
		pHandle->status = 0;
		return true;
	}else{
		return false;
	}
}

/*-----------------------------------------------------------------------------------------
 *    API Link - API
 */

const terminal_api_t terminal_api = {
	.init = terminal_init,
	.setBuffer = terminal_setBuffer,
	.addCommand = terminal_addCommand,
	.removeCommand = terminal_removeCommand,
	.start = terminal_start,
	.stop = terminal_stop
};



/*-----------------------------------------------------------------------------------------
 *    End of file
 */
