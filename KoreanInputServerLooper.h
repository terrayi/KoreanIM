
#ifndef KOREAN_INPUT_SERVER_LOOPER_H
#define KOREAN_INPUT_SERVER_LOOPER_H

#include <Alert.h>
#include <Looper.h>
#include <hangul-1.0/hangul.h>

class BInputServerMethod;
class BMenu;
class BMessage;

class KoreanInputServerLooper : public BLooper
{
public:
	KoreanInputServerLooper(BInputServerMethod *method);
	virtual ~KoreanInputServerLooper();
	virtual void MessageReceived(BMessage *message);
	status_t Activated(bool active);

private:
	BInputServerMethod *inputServerMethod;
	BMenu *methodMenu;
	bool inputIsPending;

	void ProcessMessage(BMessage *message);
	void SkipInput(void);
	void StartInput(void);
	void SendInput(const ucschar *src, bool confirmed);
	void RequestLocation();
	void StopInput(void);
	void Clear(void);
	BMessage* CreateNewMessage(int32 opcode);
	void EnqueueMessage(BMessage *message);
	void ConvertToUTF8(const ucschar *src, char *dst);
};

#endif
