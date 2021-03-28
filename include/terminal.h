/*-----------------------------------------------------------------------------------------
 *    File Name   :terminal.h
 *    Version     :V1.0.0
 *    Create Date :2020-10-15
 *    Modufy Date :2020-10-15
 *    Information :
 */
#ifndef TERMINAL_H_
#define TERMINAL_H_
#include <stdint.h>
#include <stdbool.h>

#include "tool_fifo.h"

#include "terminal_entity.h"

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus
/*-----------------------------------------------------------------------------------------
 *    Parameter
 */

/*-----------------------------------------------------------------------------------------
 *    Extern Function/Variable
 */

/*-----------------------------------------------------------------------------------------
 *    Type/Structure
 */

typedef struct _tool_terminal_config_t{
  tool_terminal_xfer_api_t* xferApi;
	const char* prefix;
	
  struct{
	  tool_terminal_command_t* buffer;
	  uint32_t size;
	}commandBuffer;
	
	struct{
	  uint8_t* buffer;
	  uint32_t size;
	}handleBuffer;
	
	struct{
	  uint8_t* buffer;
	  uint32_t size;
	}inputBuffer;
}tool_terminal_config_t;

typedef struct _tool_terminal_handle_t{
	tool_fifo_t inputBuffer;
	tool_terminal_xfer_api_t xfer;
	
	struct{
	  tool_terminal_command_t* command;
	  uint32_t size;
	}commandList;
	
	struct{
	  uint8_t* buffer;
	  uint32_t size;
	}handleBuffer;
	
	struct{
	  char* argv[8];
	}handleCache;
	
	const char* prefix;
	uint32_t flag;
}tool_terminal_handle_t;



typedef struct _terminal_api_t{
	bool (*init)         (tool_terminal_handle_t* handle, const tool_terminal_config_t *config);
	bool (*addCommand)   (tool_terminal_handle_t* handle, const tool_terminal_command_t command);
	bool (*removeCommand)(tool_terminal_handle_t* handle, const tool_terminal_command_t command);
	bool (*insertByte)   (tool_terminal_handle_t* handle, uint8_t data);
	bool (*insert)       (tool_terminal_handle_t* handle, uint8_t* data, uint32_t length);
	bool (*execute)      (tool_terminal_handle_t* handle);
}terminal_api_t;

/*-----------------------------------------------------------------------------------------
 *    Variable
 */
 
extern const terminal_api_t terminal_api;
/*-----------------------------------------------------------------------------------------
 *    Function
 */
 
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //TERMINAL_H_
/*-----------------------------------------------------------------------------------------
 *    End of file
 */
