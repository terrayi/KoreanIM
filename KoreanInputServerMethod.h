
#ifndef KOREAN_INPUT_SERVER_METHOD_H
#define KOREAN_INPUT_SERVER_METHOD_H

#include <InputServerMethod.h>

#if __POWERPC__
#pragma export on
#endif
extern "C" _EXPORT BInputServerMethod* instantiate_input_method();
#if __POWERPC__
#pragma export off
#endif

class BList;
class BMessage;

class KoreanInputServerMethod : public BInputServerMethod
{
public:
	KoreanInputServerMethod();
	virtual ~KoreanInputServerMethod();

	virtual status_t MethodActivated(bool active);
	virtual filter_result Filter(BMessage *message, BList *outList);

private:
	KoreanInputServerLooper *inputServerLooper;
};

#endif
