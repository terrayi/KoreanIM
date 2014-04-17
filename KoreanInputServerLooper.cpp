
#include "KoreanInputServerLooper.h"

#include <Input.h>
#include <InputServerMethod.h>
#include <interface/InterfaceDefs.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Message.h>
#include <Messenger.h>

#include "defines.h"
#include "upper_caps.h"
#include "icon_default.h"
#include "icon_2bul.h"
#include "icon_3bul.h"

#ifdef DEBUG
#include <Debug.h>
#endif

extern uchar icon_default[256];

static	HangulInputContext *hangulInputContext;

KoreanInputServerLooper::KoreanInputServerLooper(BInputServerMethod *method)
: BLooper(INPUT_SERVER_METHOD_NAME), inputServerMethod(method), inputIsPending(false)
{
	BMenuItem *menuItem = NULL;

#ifdef DEBUG
	_sPrintf("KoreanInputServerLooper instance created..\n");
#endif

	hangulInputContext = hangul_ic_new(DEFAULT_METHOD_INPUT_TYPE);

	methodMenu = new BMenu(INPUT_SERVER_METHOD_NAME);
	methodMenu->AddItem(new BMenuItem("2à€­bul", new BMessage(KOREAN_IM_MENU_2_BUL)));
	methodMenu->AddItem(new BMenuItem("3à€­bul", new BMessage(KOREAN_IM_MENU_3_BUL)));
	methodMenu->AddSeparatorItem();
	methodMenu->AddItem(new BMenuItem("About", new BMessage(KOREAN_IM_MENU_ABOUT)));

	menuItem = methodMenu->FindItem(KOREAN_IM_MENU_2_BUL);
	menuItem->SetMarked(true);
}

KoreanInputServerLooper::~KoreanInputServerLooper()
{
	hangul_ic_delete(hangulInputContext);
	inputServerMethod->SetMenu(NULL, this);
	delete methodMenu;
}

void
KoreanInputServerLooper::ProcessMessage(BMessage *message)
{
	int32 modifiers = 0;
	int32 key = 0;
	int32 ascii = 0;

	const ucschar *commit;
	const ucschar *preedit;
	bool res = false;
	bool shift_pressed = false;

	if (message->what != B_KEY_DOWN)
	{
		this->SkipInput();
		return;
	}

	message->FindInt32("modifiers", &modifiers);
	message->FindInt32("key", &key);
	message->FindInt32("raw_char", &ascii);

#ifdef DEBUG
	_sPrintf("Key Down message received. %02x (Key: %lu with modifiers: %lu)\n", (char)ascii, key, modifiers);
#endif

	if (modifiers & (B_COMMAND_KEY | B_OPTION_KEY))
	{
		this->SkipInput();
		return;
	}
	else if (modifiers & B_SHIFT_KEY)
	{
		shift_pressed = true;
	}

	if (key == 0x1e)
	{
		res = hangul_ic_backspace(hangulInputContext);
	}
	else
	{
		if (shift_pressed && ascii >= 0x2c && ascii <= 0x7a)
		{
			ascii = upperCap[ascii - 0x2c];
		}

#ifdef DEBUG
		_sPrintf("Ascii: %02x (%c)\n", (char)ascii, (char)ascii);
#endif
		res = hangul_ic_process(hangulInputContext, ascii);
	}

	commit = hangul_ic_get_commit_string(hangulInputContext);
	preedit = hangul_ic_get_preedit_string(hangulInputContext);

#ifdef DEBUG
	_sPrintf("Commit: 0x%02x%02x%02x%02x\n",
		commit[0] & 0x0ff, commit[1] & 0x0ff, commit[2] & 0x0ff, commit[3] & 0x0ff);
	_sPrintf("Preedit: 0x%02x%02x%02x%02x\n",
		preedit[0] & 0x0ff, preedit[1] & 0x0ff, preedit[2] & 0x0ff, preedit[3] & 0x0ff);
	_sPrintf("Pending? %s\n", (inputIsPending ? "Yes" : "No"));
	_sPrintf("Processed? %s\n", ((res) ? "Yes" : "No"));
#endif

	if (commit[0] != 0)
	{
		if (!inputIsPending)
		{
			this->StartInput();
		}

#ifdef DEBUG
		_sPrintf("Commit\n");
#endif

		this->SendInput(commit, true);
		this->StopInput();

		if (inputIsPending && (ascii < 'A' || (ascii > 'Z' && ascii < 'a') || ascii > 'z'))
		{
			inputIsPending = false;
			this->SkipInput();
			return;
		}

		inputIsPending = false;
	}

	if (preedit[0] != 0)
	{
		if (!inputIsPending)
		{
			this->StartInput();
		}

#ifdef DEBUG
		_sPrintf("Preedit\n");
#endif

		this->SendInput(preedit, false);
		inputIsPending = true;
	}
	else if (res && commit[0] == 0)
	{
		if (!inputIsPending)
		{
			this->StartInput();
		}

#ifdef DEBUG
		_sPrintf("Delete\n");
#endif

		this->SendInput(0, true);
		this->StopInput();
		inputIsPending = false;
	}

	if (!res)
	{
#ifdef DEBUG
		_sPrintf("Flush any pending...\n");
#endif

		if (preedit[0] != 0)
		{
			this->StartInput();
			this->SendInput(preedit, true);
			this->StopInput();
			inputIsPending = false;
		}
		else
		{
#ifdef DEBUG
			_sPrintf("Not processed here...\n");
#endif
			this->SkipInput();
		}
	}
}

void
KoreanInputServerLooper::MessageReceived(BMessage *message)
{
	BAlert *alert = NULL;
	BMenuItem *menuItem = NULL;

#ifdef DEBUG
	_sPrintf("KoreanInputServerLooper::MessageReceived: %.4s\n", (char*) &message->what);
#endif

	switch (message->what)
	{
		case B_KEY_DOWN:
		{
			this->ProcessMessage(message);
			break;
		}
		case KOREAN_IM_MENU_2_BUL:
		{
#ifdef DEBUG
			_sPrintf("User selected 2-bul\n");
#endif

			hangul_ic_select_keyboard(hangulInputContext, METHOD_INPUT_TYPE_2_BUL);

			menuItem = methodMenu->FindItem(KOREAN_IM_MENU_2_BUL);
			menuItem->SetMarked(true);
			menuItem = methodMenu->FindItem(KOREAN_IM_MENU_3_BUL);
			menuItem->SetMarked(false);
			inputServerMethod->SetMenu(methodMenu, this);
			inputServerMethod->SetIcon(icon_2bul);
			break;
		}
		case KOREAN_IM_MENU_3_BUL:
		{
#ifdef DEBUG
			_sPrintf("User selected 3-bul\n");
#endif

			hangul_ic_select_keyboard(hangulInputContext, METHOD_INPUT_TYPE_3_BUL_FINAL);

			menuItem = methodMenu->FindItem(KOREAN_IM_MENU_2_BUL);
			menuItem->SetMarked(false);
			menuItem = methodMenu->FindItem(KOREAN_IM_MENU_3_BUL);
			menuItem->SetMarked(true);
			inputServerMethod->SetMenu(methodMenu, this);
			inputServerMethod->SetIcon(icon_3bul);
			break;
		}
		case KOREAN_IM_MENU_ABOUT:
		{
#ifdef DEBUG
			_sPrintf("User selected About menu\n");
#endif

			alert = new BAlert("", KOREAN_IM_MENU_ABOUT_TEXT, "OK");
			alert->Go();
			inputServerMethod->SetMenu(methodMenu, this);
			break;
		}
		case B_INPUT_METHOD_EVENT:
		{
			switch (message->FindInt32("be:opcode"))
			{
				case B_INPUT_METHOD_STOPPED:
				{
#ifdef DEBUG
					_sPrintf("stopped message received\n");
#endif
					this->Clear();
					break;
				}
				case B_INPUT_METHOD_LOCATION_REQUEST:
				{
#ifdef DEBUG
					_sPrintf("location request message received\n");
#endif
					break;
				}
			}

			break;
		}
		default:
		{
#ifdef DEBUG
			_sPrintf("unknown message. fall-through");
#endif
			BLooper::MessageReceived(message);
		}
	}
}

status_t
KoreanInputServerLooper::Activated(bool active)
{
	if (active)
	{
#ifdef DEBUG
		_sPrintf("KoreanIM is activated\n");
#endif
		inputServerMethod->SetMenu(methodMenu, this);
		inputServerMethod->SetIcon(icon_2bul);
	}
	else
	{
		hangul_ic_flush(hangulInputContext);
		hangul_ic_reset(hangulInputContext);
		inputServerMethod->SetMenu(NULL, this);
		inputServerMethod->SetIcon(icon_default);

#ifdef DEBUG
		_sPrintf("KoreanIM is deactivated\n");
#endif
	}

	return B_OK;	
}

void
KoreanInputServerLooper::SkipInput(void)
{
#ifdef DEBUG
	_sPrintf("Input skipped\n");
#endif
	this->EnqueueMessage(this->DetachCurrentMessage());
}

void
KoreanInputServerLooper::StartInput(void)
{
#ifdef DEBUG
	_sPrintf("Input method started\n");
#endif
	this->EnqueueMessage(this->CreateNewMessage(B_INPUT_METHOD_STARTED));
}

void
KoreanInputServerLooper::SendInput(const ucschar *src, bool confirmed)
{
	BMessage *msg = NULL;
	char hangul_in_utf8[4] = { 0, 0, 0, 0 };
	int32 length = 0;

	if (src)
	{
		this->ConvertToUTF8(src, hangul_in_utf8);
	}

	length = strlen(hangul_in_utf8);

	msg = this->CreateNewMessage(B_INPUT_METHOD_CHANGED);
	msg->AddString("be:string", hangul_in_utf8);
	msg->AddBool("be:confirmed", confirmed);
	msg->AddInt32("be:clause_start", 0);
	msg->AddInt32("be:clause_end", length);
	msg->AddInt32("be:selection", length);
	msg->AddInt32("be:selection", length);

#ifdef DEBUG
	_sPrintf("Input method changed\n");
#endif
	this->EnqueueMessage(msg);
}

void
KoreanInputServerLooper::RequestLocation(void)
{
#ifdef DEBUG
	_sPrintf("Input method location request\n");
#endif
	this->EnqueueMessage(this->CreateNewMessage(B_INPUT_METHOD_LOCATION_REQUEST));
}

void
KoreanInputServerLooper::StopInput(void)
{
#ifdef DEBUG
	_sPrintf("Input method stopped\n");
#endif
	this->EnqueueMessage(this->CreateNewMessage(B_INPUT_METHOD_STOPPED));
}

void
KoreanInputServerLooper::Clear(void)
{
	if (inputIsPending)
	{
		this->SendInput(0, true);
		this->StopInput();
		inputIsPending = false;
	}

	hangul_ic_flush(hangulInputContext);
	hangul_ic_reset(hangulInputContext);
}

BMessage*
KoreanInputServerLooper::CreateNewMessage(int32 opcode)
{
	BMessage *msg = NULL;

	msg = new BMessage(B_INPUT_METHOD_EVENT);
	msg->AddInt32("be:opcode", opcode);
	msg->AddMessenger("be:reply_to", BMessenger(this));

	return msg;
}

void
KoreanInputServerLooper::EnqueueMessage(BMessage *message)
{
	inputServerMethod->EnqueueMessage(message);
}

//TODO: FIX TO INCLUDE ALL CASES!
void
KoreanInputServerLooper::ConvertToUTF8(const ucschar *src, char *dst)
{
	unsigned int ud = *src;

	dst[0] = 0xe0 | ((ud >> 12) & 0x0f);
	dst[1] = 0x80 | ((ud >> 6) & 0x3f);
	dst[2] = 0x80 | (ud & 0x3f);
	dst[3] = 0;

#ifdef DEBUG
	_sPrintf("UTF-8: %s (0x%02x%02x%02x)\n",
		dst, dst[0] & 0x0ff, dst[1] & 0x0ff, dst[2] & 0x0ff);
#endif
}

