/*-----------------------------------------------------------------------------------------
 *    File Name   :terminal_entity.h
 *    Create Date :2020-10-15
 *    Modufy Date :2020-10-15
 *    Information :
 */
#ifndef TERMINAL_ENTITY_H_
#define TERMINAL_ENTITY_H_
#include <stdint.h>
#include <stdbool.h>

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

typedef struct _tool_terminal_xfer_api_t{
	void (*send)(const void* data, int len);
	void (*sendByte)(uint8_t data);
	void (*sendString)(const char* string);
}tool_terminal_xfer_api_t;

typedef struct _tool_terminal_command_t{
	const char* command;
	void* attachment;
	bool (*execute)(const tool_terminal_xfer_api_t* api, void* attachment, int argc, char **argv);
}tool_terminal_command_t;

/*-----------------------------------------------------------------------------------------
 *    Variable
 */
 
/*-----------------------------------------------------------------------------------------
 *    Function
 */
 
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //TERMINAL_ENTITY_H_
/*-----------------------------------------------------------------------------------------
 *    End of file
 */
