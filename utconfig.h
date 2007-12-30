#ifndef __UTYPING_CONFIG
#define __UTYPING_CONFIG

enum{
CONFIG_NEVER,
CONFIG_QUERY,
CONFIG_ALWAYS,
};

#include <stdio.h>
#include <string.h>

class UTypingConfig{
public:
	UTypingConfig();
	void init();
	void read();
public:
	//bool f_useMultiThread;
	int loadSoundType;
	int volume;
	
	char defaultReplayFile[256];
	int overwriteReplay;
	
	bool f_rankingCheckDate;
	bool f_rankingCheckChallenge;
	
	bool f_failByGauge;
	
	bool f_fullScreen;
	bool f_showFPS;
	bool f_waitVSync;
	
	bool f_debugMode;
	bool f_debugBeat;	/* false ���ʂ� debug / true BEATLINE �������ɂ��� */
	int debugTime;	/* �f�o�b�O�J�n�ʒu�i�~���b�j */
	
	bool f_showProcessTime;
};

UTypingConfig g_config;

UTypingConfig::UTypingConfig(){
	init();
}

void UTypingConfig::init(){
	//f_useMultiThread = true;
	loadSoundType = DX_SOUNDDATATYPE_MEMPRESS;
	volume = -1;
	
	strcpy(defaultReplayFile, "default.rep");
	overwriteReplay = CONFIG_QUERY;
	
	f_rankingCheckDate = false;
	f_rankingCheckChallenge = true;
	
	f_failByGauge = false;
	
	f_fullScreen = false;
	f_showFPS = false;
	f_waitVSync = true;
	
	f_debugMode = false;
	f_debugBeat = false;
	debugTime = 0;
	
	f_showProcessTime = false;
}

void UTypingConfig::read(){
	FILE *fp;
	fp = fopen("UTyping_config.txt", "r");
	if(!fp){
		/* config���Ȃ���΁A���ɉ������Ȃ��ł悢�B */
		/* config���Ȃ����Ƃ��G���[�ɂ���͔̂���������B */
		return;
	}
	char buf[256];
	while(fgetline(buf, fp)){
		char *ptr1 = strtok(buf, "=");
		if(ptr1 == NULL || ptr1[0] == '#'){	/* ����������Ă��Ȃ���'#'�Ŏn�܂�R�����g�s */
			continue;
		}
		char *ptr2 = strtok(NULL, "");
		if(ptr2 == NULL){
			ptr2 = "";	/* ""���w�����Ă��� */
		}
		/*
		if(!strcmp(ptr1, "UseMultiThread")){
			if(!strcmp(ptr2, "true")){
				f_useMultiThread = true;
			}else if(!strcmp(ptr2, "false")){
				f_useMultiThread = false;
			}else{
				throw "[config] UseMultiThread �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: true �j";
			}
		}else */
		if(!strcmp(ptr1, "LoadSoundType")){
			if(!strcmp(ptr2, "0")){	/* �����ǂݍ��ݎ��Ԃ������A�Đ����ׂ͌y�� */
				loadSoundType = DX_SOUNDDATATYPE_MEMNOPRESS;	/* �ǂݍ��ނƂ��Ɉ��k��W�J */
			}else if(!strcmp(ptr2, "1")){	/* ���X�̓ǂݍ��ݎ��ԂƍĐ����� */
				loadSoundType = DX_SOUNDDATATYPE_MEMPRESS;	/* �Đ�����Ƃ��Ɉ��k��W�J */
			}else if(!strcmp(ptr2, "2")){	/* �ǂݍ��ݎ��Ԃ͒Z���A�Đ����ׂ͏d�� */
				loadSoundType = DX_SOUNDDATATYPE_FILE;	/* �������Ƀ��[�h���Ȃ� */
			}else{
				throw "[config] LoadSoundType �� 0, 1 �܂��� 2 �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: 1 �j";
			}
		}else if(!strcmp(ptr1, "Volume")){
			if(!strcmp(ptr2, "default")){
				volume = -1;
			}else{
				int n;
				int tmp;
				tmp = sscanf(ptr2, "%d", &n);
				if(tmp < 1 || n < 0 || n > 255){
					throw "[config] Volume �� default �܂��� 0 �ȏ� 255 �ȉ� �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: default �j";
				}
				volume = n;
			}
		}else if(!strcmp(ptr1, "DefaultReplayFile")){
			strcpy(defaultReplayFile, ptr2);
			/* ���Ɏ��s�͂��Ȃ� */
			//throw "";
		}else if(!strcmp(ptr1, "OverwriteReplay")){
			if(!strcmp(ptr2, "never")){
				overwriteReplay = CONFIG_NEVER;
			}else if(!strcmp(ptr2, "query")){
				overwriteReplay = CONFIG_QUERY;
			}else if(!strcmp(ptr2, "always")){
				overwriteReplay = CONFIG_ALWAYS;
			}else{
				throw "[config] OverwriteReplay �� never, query �܂��� always �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: query �j";
			}
		}else if(!strcmp(ptr1, "RankingCheckDate")){
			if(!strcmp(ptr2, "true")){
				f_rankingCheckDate = true;
			}else if(!strcmp(ptr2, "false")){
				f_rankingCheckDate = false;
			}else{
				throw "[config] RankingCheckDate �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: false �j";
			}
		}else if(!strcmp(ptr1, "RankingCheckChallenge")){
			if(!strcmp(ptr2, "true")){
				f_rankingCheckChallenge = true;
			}else if(!strcmp(ptr2, "false")){
				f_rankingCheckChallenge = false;
			}else{
				throw "[config] RankingCheckChallenge �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: true �j";
			}
		}else if(!strcmp(ptr1, "FailByGauge")){
			if(!strcmp(ptr2, "true")){
				f_failByGauge = true;
			}else if(!strcmp(ptr2, "false")){
				f_failByGauge = false;
			}else{
				throw "[config] FailByGauge �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: false �j";
			}
		}else if(!strcmp(ptr1, "FullScreen")){
			if(!strcmp(ptr2, "true")){
				f_fullScreen = true;
			}else if(!strcmp(ptr2, "false")){
				f_fullScreen = false;
			}else{
				throw "[config] FullScreen �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: false �j";
			}
		}else if(!strcmp(ptr1, "ShowFPS")){
			if(!strcmp(ptr2, "true")){
				f_showFPS = true;
			}else if(!strcmp(ptr2, "false")){
				f_showFPS = false;
			}else{
				throw "[config] ShowFPS �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: false �j";
			}
		}else if(!strcmp(ptr1, "WaitVSync")){
			if(!strcmp(ptr2, "true")){
				f_waitVSync = true;
			}else if(!strcmp(ptr2, "false")){
				f_waitVSync = false;
			}else{
				throw "[config] WaitVSync �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: true �j";
			}
		}else if(!strcmp(ptr1, "DebugMode")){
			if(!strcmp(ptr2, "true")){
				f_debugMode = true;
				f_debugBeat = false;
			}else if(!strcmp(ptr2, "beat")){
				f_debugMode = true;
				f_debugBeat = true;
			}else if(!strcmp(ptr2, "false")){
				f_debugMode = false;
			}else{
				throw "[config] DebugMode �� true �܂��� beat �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: false �j";
			}
		}else if(!strcmp(ptr1, "DebugTime")){
			int n;
			int tmp;
			tmp = sscanf(ptr2, "%d", &n);
			if(tmp < 1 || n < 0){
				throw "[config] DebugTime �� 0 �ȏ� �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: 0 �j";
			}
			debugTime = n;
		}else if(!strcmp(ptr1, "ShowProcessTime")){
			if(!strcmp(ptr2, "true")){
				f_showProcessTime = true;
			}else if(!strcmp(ptr2, "false")){
				f_showProcessTime = false;
			}else{
				throw "[config] ShowProcessTime �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: false �j";
			}
		}else{
			//throw "[config] �ݒ�ł���l�� UseMultiThread, LoadSoundType, DebugMode �ł���B";
			//throw "[config] �ݒ�ł���l�� LoadSoundType, Volume, FullScreen, ShowFPS, WaitVSync, DebugMode �ł���B";
			throw "[config] �ݒ�ł��Ȃ����ڂ�ݒ肵�悤�Ƃ����B";
		}
	}
	fclose(fp);
}

#endif
