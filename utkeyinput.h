#ifndef __UTYPING_KEYINPUT
#define __UTYPING_KEYINPUT

#include "dxlib.h"

#define KEYBOARD_INPUT_BUFFER_SIZE 256

struct KeyboardInputData{
	char ch;
	LONGLONG timeCount;	/* �L�[�������ꂽ���� */
};

class KeyboardInput{
public:
	KeyboardInput();
	~KeyboardInput();
	void clear();
	void push(KeyboardInputData &data);
	KeyboardInputData pop();
	bool empty();
private:
	void EnterCS();
	void LeaveCS();
private:
	KeyboardInputData m_queue[KEYBOARD_INPUT_BUFFER_SIZE];
	int m_queueBegin, m_queueEnd;
	CRITICAL_SECTION m_cs;
};

KeyboardInput g_keyboardInput;
/* �O���[�o���ō���Ă��� */

KeyboardInput::KeyboardInput(){
	InitializeCriticalSection(&m_cs);
	m_queueEnd = 0;
	clear();
}

KeyboardInput::~KeyboardInput(){
	DeleteCriticalSection(&m_cs);
}

void KeyboardInput::clear(){
	m_queueBegin = m_queueEnd;
}

void KeyboardInput::push(KeyboardInputData &data){
	EnterCS();
	if(m_queueEnd - m_queueBegin < KEYBOARD_INPUT_BUFFER_SIZE){
	/* �o�b�t�@�������ς��ł͂Ȃ� */
		m_queue[m_queueEnd % KEYBOARD_INPUT_BUFFER_SIZE] = data;
		m_queueEnd++;
	}
	LeaveCS();
}

KeyboardInputData KeyboardInput::pop(){
	EnterCS();
	KeyboardInputData data;
	data = m_queue[m_queueBegin % KEYBOARD_INPUT_BUFFER_SIZE];
	m_queueBegin++;
	LeaveCS();
	return data;
}

bool KeyboardInput::empty(){
	bool isEmpty;
	EnterCS();
	if(m_queueBegin == m_queueEnd){
		isEmpty = true;
	}else{
		isEmpty = false;
	}
	LeaveCS();
	return isEmpty;
}

void KeyboardInput::EnterCS(){
	EnterCriticalSection(&m_cs);
}

void KeyboardInput::LeaveCS(){
	LeaveCriticalSection(&m_cs);
}

/* ============================================================ */

long FAR PASCAL KeyboardInputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	static char CtrlCode[][ 2 ] =
	{
		VK_DELETE	,	CTRL_CODE_DEL	,	// �c�d�k�L�[

		VK_LEFT		,	CTRL_CODE_LEFT	,	// ���L�[
		VK_RIGHT	,	CTRL_CODE_RIGHT	,	// ���L�[
		VK_UP		,	CTRL_CODE_UP	,	// ���L�[
		VK_DOWN		,	CTRL_CODE_DOWN	,	// ���L�[
		0			,	0
	} ;

	switch(message){
	// �����R�[�h���b�Z�[�W
	case WM_CHAR :
		{
			// �������o�b�t�@�ɃR�s�[
			//ST_StokInputChar( ( char )wParam ) ;
			KeyboardInputData data;
			data.ch = (char)wParam;
			data.timeCount = GetNowHiPerformanceCount();
//printfDx("%c(%d), %f\n", data.ch, data.ch, (double)data.timeCount);
			g_keyboardInput.push(data);
		}
		break ;
	case WM_KEYDOWN:
		{
			int VKey = ( int )wParam ;
			char *CCode = ( char * )CtrlCode ;

			// �R���g���[�������R�[�h�ɑΉ�����L�[��
			// ������Ă�����o�b�t�@�Ɋi�[����
			while( *CCode )
			{
				if( *CCode == VKey )
				{
					// �o�b�t�@�ɕ����R�[�h����
					//ST_StokInputChar( *( CCode + 1 ) ) ;
					KeyboardInputData data;
					data.ch = *(CCode + 1);
					data.timeCount = GetNowHiPerformanceCount();
//printfDx("%c(%d), %f\n", data.ch, data.ch, (double)data.timeCount);
					g_keyboardInput.push(data);
					break ;
				}
				CCode += 2 ;
			}
		}
		break ;
	}
	return 0;
}

/* �L�[��������������K�v�Ƃ��Ȃ��i�Ԃ��Ȃ��j�o�[�W���� */
char GetKeyboardInput(){
	char ch;
	if(g_keyboardInput.empty()){	/* �L���[���� */
		ch = 0;
	}else{	/* �L���[����łȂ��̂ł�����擾 */
		KeyboardInputData data = g_keyboardInput.pop();
		ch = data.ch;
		//timeCount = data.timeCount;
	}
	return ch;
}

char GetKeyboardInput(LONGLONG &timeCount){
	char ch;
	if(g_keyboardInput.empty()){	/* �L���[���� */
		ch = 0;
	}else{	/* �L���[����łȂ��̂ł�����擾 */
		KeyboardInputData data = g_keyboardInput.pop();
		ch = data.ch;
		timeCount = data.timeCount;
	}
	return ch;
}

#endif
