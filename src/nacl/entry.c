#include <stddef.h>
#include <string.h>

#include <ppapi/c/pp_bool.h>
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/ppp.h>

#include <blis/blis.h>

#include <nacl/interfaces.h>
#include <nacl/stringvars.h>

PP_EXPORT int32_t PPP_InitializeModule(PP_Module module, PPB_GetInterface get_browser_interface) {
	coreInterface = get_browser_interface(PPB_CORE_INTERFACE_1_0);
	if (coreInterface == NULL) {
		return PP_ERROR_NOINTERFACE;
	}
	consoleInterface = get_browser_interface(PPB_CONSOLE_INTERFACE_1_0);
	if (consoleInterface == NULL) {
		return PP_ERROR_NOINTERFACE;
	}
	messagingInterface = get_browser_interface(PPB_MESSAGING_INTERFACE_1_0);
	if (messagingInterface == NULL) {
		return PP_ERROR_NOINTERFACE;
	}
	varInterface = get_browser_interface(PPB_VAR_INTERFACE_1_1);
	if (varInterface == NULL) {
		return PP_ERROR_NOINTERFACE;
	}
	dictionaryInterface = get_browser_interface(PPB_VAR_DICTIONARY_INTERFACE_1_0);
	if (dictionaryInterface == NULL) {
		return PP_ERROR_NOINTERFACE;
	}
	err_t blisError = bli_init();
	if (blisError != BLIS_SUCCESS) {
		return PP_ERROR_FAILED;
	}
	stringVarsInit();

	return PP_OK;
}

PP_EXPORT void PPP_ShutdownModule(void) {
	stringVarsRelease();
	bli_finalize();
}

PP_EXPORT const void* PPP_GetInterface(const char* interface_name) {
	if (strcmp(interface_name, PPP_INSTANCE_INTERFACE_1_1) == 0) {
		return &pluginInstanceInterface;
	} else if (strcmp(interface_name, PPP_MESSAGING_INTERFACE_1_0) == 0) {
		return &pluginMessagingInterface;
	} else {
		return NULL;
	}
}
