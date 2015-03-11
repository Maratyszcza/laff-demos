#include <ppapi/c/ppb_core.h>
#include <ppapi/c/ppb_console.h>
#include <ppapi/c/ppb_messaging.h>
#include <ppapi/c/ppb_var.h>
#include <ppapi/c/ppb_var_dictionary.h>

#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/ppp_messaging.h>

extern const struct PPB_Core_1_0* coreInterface;
extern const struct PPB_Console_1_0* consoleInterface;
extern const struct PPB_Messaging_1_0* messagingInterface;
extern const struct PPB_Var_1_1* varInterface;
extern const struct PPB_VarDictionary_1_0* dictionaryInterface;

extern const struct PPP_Messaging_1_0 pluginMessagingInterface;
extern const struct PPP_Instance_1_1 pluginInstanceInterface;
