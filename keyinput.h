#ifndef KEYINPUT_H
#define KEYINPUT_H

#include "dxlib.h"
#include <deque>
using namespace std;

struct KeyboardInput{
	char ch;
	LONGLONG timeCount;	/* �L�[�������ꂽ���� */
};

deque<KeyboardInput> g_deqKeyboardInput;
//CRITICAL_SECTION g_csKeyboardInput;

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
			KeyboardInput ki;
			ki.ch = (char)wParam;
			ki.timeCount = GetNowHiPerformanceCount();
//printfDx("%c(%d), %f\n", ki.ch, ki.ch, (double)ki.timeCount);
			g_deqKeyboardInput.push_back(ki);
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
					KeyboardInput ki;
					ki.ch = *(CCode + 1);
					ki.timeCount = GetNowHiPerformanceCount();
//printfDx("%c(%d), %f\n", ki.ch, ki.ch, (double)ki.timeCount);
					g_deqKeyboardInput.push_back(ki);
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
	if(g_deqKeyboardInput.empty()){	/* �L���[���� */
		ch = 0;
	}else{	/* �L���[����łȂ��̂ł�����擾 */
		KeyboardInput &ki = g_deqKeyboardInput.front();
		ch = ki.ch;
		//timeCount = ki.timeCount;
		g_deqKeyboardInput.pop_front();	/* �ǂ񂾂�폜 */
	}
	return ch;
}

char GetKeyboardInput(LONGLONG &timeCount){
	char ch;
	if(g_deqKeyboardInput.empty()){	/* �L���[���� */
		ch = 0;
	}else{	/* �L���[����łȂ��̂ł�����擾 */
		KeyboardInput &ki = g_deqKeyboardInput.front();
		ch = ki.ch;
		timeCount = ki.timeCount;
		g_deqKeyboardInput.pop_front();	/* �ǂ񂾂�폜 */
	}
	return ch;
}

#endif