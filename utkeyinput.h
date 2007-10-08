#ifndef __UTYPING_KEYINPUT
#define __UTYPING_KEYINPUT

#include "DxLib.h"

/* DxLib.h�Œ�`���ĂȂ��L�[�R�[�h */
/* ����Ƃ��Ԃ�08,09,0d,10,1c,1d,1e,1f��20�ȍ~�͔����� */
#define CTRL_CODE_F1 (0x01)	// F1
#define CTRL_CODE_F2 (0x02)	// F2
#define CTRL_CODE_F3 (0x03)	// F3
#define CTRL_CODE_F4 (0x04)	// F4
#define CTRL_CODE_F5 (0x05)	// F5
#define CTRL_CODE_F6 (0x06)	// F6
#define CTRL_CODE_F7 (0x07)	// F7
#define CTRL_CODE_F8 (0x0a)	// F8
#define CTRL_CODE_F9 (0x0b)	// F9
#define CTRL_CODE_F10 (0x0c)	// F10
#define CTRL_CODE_F11 (0x0e)	// F11
#define CTRL_CODE_F12 (0x0f)	// F12


#define KEYBOARD_INPUT_BUFFER_SIZE 256

struct KeyboardInputData{
	char ch;
	double timeCount;	/* �L�[�������ꂽ���� */
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
	EnterCS();
	m_queueBegin = m_queueEnd;
	LeaveCS();
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
		VK_F1		,	CTRL_CODE_F1	,	// F1�L�[
		VK_F2		,	CTRL_CODE_F2	,	// F2�L�[
		VK_F3		,	CTRL_CODE_F3	,	// F3�L�[
		VK_F4		,	CTRL_CODE_F4	,	// F4�L�[
		VK_F5		,	CTRL_CODE_F5	,	// F5�L�[
		VK_F6		,	CTRL_CODE_F6	,	// F6�L�[
		VK_F7		,	CTRL_CODE_F7	,	// F7�L�[
		VK_F8		,	CTRL_CODE_F8	,	// F8�L�[
		VK_F9		,	CTRL_CODE_F9	,	// F9�L�[
		VK_F10		,	CTRL_CODE_F10	,	// F10�L�[
		VK_F11		,	CTRL_CODE_F11	,	// F11�L�[
		VK_F12		,	CTRL_CODE_F12	,	// F12�L�[
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
			data.timeCount = myGetNowHiPerformanceCount();
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
					data.timeCount = myGetNowHiPerformanceCount();
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

char GetKeyboardInput(double &timeCount){
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

int ClearKeyboardInput(){
	g_keyboardInput.clear();
	return 0;
}

#endif
