
//#ifdef UTYPING_USE_MULTI_THREAD
/* �}���`�X���b�h�p */

struct KeyboardInput{
	char ch;
	LONGLONG timeCount;	/* �L�[�������ꂽ���� */
};

deque<KeyboardInput> g_deqKeyboardInput;
CRITICAL_SECTION g_csKeyboardInput;

/* �L�[��������������K�v�Ƃ��Ȃ��i�Ԃ��Ȃ��j�o�[�W���� */
char MT_GetKeyboardInput(){
	char ch;
	EnterCriticalSection(&g_csKeyboardInput);
	{
		if(g_deqKeyboardInput.empty()){	/* �L���[���� */
			ch = 0;
		}else{	/* �L���[����łȂ��̂ł�����擾 */
			KeyboardInput &ki = g_deqKeyboardInput.front();
			ch = ki.ch;
			//timeCount = ki.timeCount;
			g_deqKeyboardInput.pop_front();	/* �ǂ񂾂�폜 */
		}
	}
	LeaveCriticalSection(&g_csKeyboardInput);
	return ch;
}

/* �L�[��������������Ԃ��o�[�W���� */
char MT_GetKeyboardInput(LONGLONG &timeCount){
	char ch;
	EnterCriticalSection(&g_csKeyboardInput);
	{
		if(g_deqKeyboardInput.empty()){	/* �L���[���� */
			ch = 0;
		}else{	/* �L���[����łȂ��̂ł�����擾 */
			KeyboardInput &ki = g_deqKeyboardInput.front();
			ch = ki.ch;
			timeCount = ki.timeCount;
			g_deqKeyboardInput.pop_front();	/* �ǂ񂾂�폜 */
		}
	}
	LeaveCriticalSection(&g_csKeyboardInput);
	return ch;
}

DWORD WINAPI KeyboardInputThread(LPVOID lpParameter){
#if 0
	char keyState[256];	/* �L�[�̏�� */
	memset(keyState, 0, 256);	/* �ŏ��͑S��������Ă��Ȃ��Ƃ݂Ȃ� */
#endif
	LONGLONG lastCount = GetNowHiPerformanceCount();
	
	ClearInputCharBuf();	/* �ŏ��Ƀo�b�t�@�������Ă��� */
	while(1){
		LONGLONG count = GetNowHiPerformanceCount();
		if(count - lastCount < 1000){	/* 1�~���b�ȏソ���Ă��Ȃ��Ȃ牽�����Ȃ� */
			Sleep(0);
			continue;
		}
		lastCount = count;
		
		KeyboardInput ki;
#if 0
		while(1){
			char buf[256];
			if(GetHitKeyStateAll(buf) == -1){	/* �G���[���N������I�� */
				goto L1;
			}
			if(memcmp(keyState, buf, 256) != 0){	/* �L�[�̉�����Ԃ��O��ƈقȂ� */
				ki.timeCount = GetNowHiPerformanceCount();	/* �����ۑ� */
				memcpy(keyState, buf, 256);
				break;
			}
			Sleep(0);
		}
#endif
//ProcessStreamSoundMemAll();
//g_check.begin();
		ki.timeCount = GetNowHiPerformanceCount();
		/* ProcessMessage�̑O�Ɏ�����ۑ����AProcessMessage���x���Ă����v�ɂ���B */
		if(ProcessMessage() == -1){
			break;
		}
//g_check.end(100);
		while(1){
			ki.ch = GetInputChar(TRUE);	/* �L�[���́iTRUE:��x�ǂݍ��񂾂�����j */
			if(ki.ch == 0){	/* �u�Ȃ��v�ƋA���Ă�����I��� */
				break;
			}
			EnterCriticalSection(&g_csKeyboardInput);
			{
				g_deqKeyboardInput.push_back(ki);	/* �L���[�ɒǉ� */
			}
			LeaveCriticalSection(&g_csKeyboardInput);
		}
	}
L1:
	return 0;
}

//#else
/* �V���O���X���b�h�p */

char ST_GetKeyboardInput(){
	return GetInputChar(TRUE);
}

char ST_GetKeyboardInput(LONGLONG &timeCount){
	timeCount = GetNowHiPerformanceCount();
	return GetInputChar(TRUE);
}

//#endif
