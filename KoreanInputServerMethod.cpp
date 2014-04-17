
#include "KoreanInputServerLooper.h"
#include "KoreanInputServerMethod.h"

#include "defines.h"
#include "icon_default.h"

#ifdef DEBUG
#include <Debug.h>
#endif

KoreanInputServerMethod::KoreanInputServerMethod()
: BInputServerMethod(INPUT_SERVER_METHOD_NAME, icon_default)
{
	inputServerLooper = new KoreanInputServerLooper(this);
	inputServerLooper->Run();
}

KoreanInputServerMethod::~KoreanInputServerMethod()
{
	if (inputServerLooper != NULL)
	{
		inputServerLooper->Lock();
		inputServerLooper->Quit();
	}
}

status_t
KoreanInputServerMethod::MethodActivated(bool active)
{
#ifdef DEBUG
	_sPrintf("MethodActivated(%s)\n", (active ? "True" : "False"));
#endif
	return inputServerLooper->Activated(active);
}

filter_result
KoreanInputServerMethod::Filter(BMessage *message, BList *outList)
{
//#ifdef DEBUG
//	_sPrintf("KoreanInputServerMethod::MessageReceived: %.4s\n", (char*) &message->what);
//#endif

	if (message->what == B_KEY_DOWN)
	{
		if (inputServerLooper != NULL)
		{
			inputServerLooper->PostMessage(message);
		}

		return B_SKIP_MESSAGE;
	}

	return B_DISPATCH_MESSAGE;
}


BInputServerMethod*
instantiate_input_method()
{
#ifdef DEBUG
	_sPrintf("Instantiating input method..\n");
#endif
	return new KoreanInputServerMethod();
}
