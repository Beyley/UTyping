#ifndef __UTYPING_UTILITY
#define __UTYPING_UTILITY

enum{
EDIT_BUFFER_NONE,
EDIT_BUFFER_OK,
EDIT_BUFFER_CANCEL,
};

/* �G���Ȋ֐� */

bool isJapanese1st(char ch);
bool isSubstr(const char *s1, const char *s2);	/* s1 �� s2 �̕��������񂩂�Ԃ� */

bool readInt(int &n, FILE *fp);
void writeInt(int n, FILE *fp);
bool readChars(char *str, int n, FILE *fp);
void writeChars(const char *str, int n, FILE *fp);

void getDirFromPath(char *directoryName, const char *fileName);

//char *fgetline(char buf[], FILE *fp);

void getOrdinal(char *buf, int n);
/* �������擾 */

void getTimeStr(char *buf, int sec);
/* �b���玞�Ԃ̕�����𓾂� */

int getDateInt();
/* ���t�𐮐��ŕۑ� */
void getDateStrFromInt(char *buf, int date);
/* �����ŕۑ��������t�𕶎���ɂ��� */

void getDateStr(char *buf);
/* ���t�Ǝ�����buf�Ɋi�[ */

void outputError(char *str);
/* �G���[�o�� */

//int editBuffer_1(char ch, char buf[], int &len);

/* ============================================================ */

bool isJapanese1st(char ch){
	return (ch&0x80)!=0;
}

bool isSubstr(const char *s1, const char *s2){
	int len1 = strlen(s1);
	if(len1 == 0){
		return true;
	}
	for(const char *ptr = s2; *ptr != '\0'; ptr++){
		if(memcmp(s1, ptr, len1) == 0){	/* ��v���� */
			return true;
		}
		if(isJapanese1st(*ptr)){	/* ���{��1�o�C�g�ڂ������Ȃ� */
			ptr++;	/* ���̃o�C�g����ɂ͂��Ȃ� */
			if(*ptr == '\0'){
				break;
			}
		}
	}
	return false;
}

bool readInt(int &n, FILE *fp){
	int b[4];
	for(int i=0; i<4; i++){
		b[i] = getc(fp);
		if(b[i] == EOF){
			return false;
		}
	}
	n = b[0] | b[1]<<8 | b[2]<<16 | b[3]<<24;
	return true;
}

void writeInt(int n, FILE *fp){
	int b[4];
	for(int i=0; i<4; i++){
		b[i] = n >> (8*i);
	}
	for(int i=0; i<4; i++){
		putc(b[i], fp);
	}
	return;
}

bool readChars(char *str, int n, FILE *fp){
	int b[16];
	for(int i=0; i<n; i++){
		b[i] = getc(fp);
		if(b[i] == EOF){
			return false;
		}
	}
	for(int i=0; i<n; i++){
		str[i] = b[i];
	}
	str[n] = '\0';
	return true;
}

void writeChars(const char *str, int n, FILE *fp){
	int b[16];
	for(int i=0; i<n; i++){
		b[i] = str[i];
	}
	for(int i=1; i<n; i++){
		if(b[i-1] == '\0'){
			b[i] = '\0';
		}
	}
	for(int i=0; i<n; i++){
		putc(b[i], fp);
	}
	return;
}

void getDirFromPath(char *directoryName, const char *fileName){
	strcpy(directoryName, fileName);
	for(int i = strlen(directoryName) - 1; i >= 0; i--){	/* ��납�炽�ǂ� */
		if(directoryName[i] == '/' || directoryName[i] == '\\'){	/* '/'��'\'�����ꂽ�� */
			directoryName[i + 1] = '\0';	/* �����܂łȂ̂ŁA����'\0'�� */
			return;
		}
	}
	/* �Ō�܂Ō���Ȃ������� */
	directoryName[0] = '\0';
	return;
}

#define fgetline(buf, fp) fgetline_1(buf, sizeof(buf), fp)
/* fp����1�s�ǂށB�������A�Ō��'\n'�͍폜 */

char *fgetline_1(char *buf, size_t size, FILE *fp){
	if(fgets(buf, size, fp) == NULL){
		return NULL;
	}
	int len = strlen(buf) - 1;
	if(buf[len] == '\n'){
		buf[len] = '\0';
	}
	return buf; 
}

void getOrdinal(char *buf, int n){
	if((n / 10) % 10 == 1){	/* ����11,12,13(10-19���ׂ�)��th */
		sprintf(buf, "%2dth", n);
	}else{
		switch(n % 10){	/* 1�̈ʂŔ��� */
		case 1:
			sprintf(buf, "%2dst", n);
			break;
		case 2:
			sprintf(buf, "%2dnd", n);
			break;
		case 3:
			sprintf(buf, "%2drd", n);
			break;
		default:
			sprintf(buf, "%2dth", n);
			break;
		}
	}
}

void getTimeStr(char *buf, int sec){
	int min,hour,day;
	min = sec/60;
	sec %= 60;
	hour = min/60;
	min %= 60;
	day = hour/24;
	hour %= 24;
	
	if(day > 0){
		sprintf(buf, "%d �� %2d ���� %2d �� %2d �b", day, hour, min, sec);
	}else if(hour > 0){
		sprintf(buf, "%d ���� %2d �� %2d �b", hour, min, sec);
	}else if(min > 0){
		sprintf(buf, "%d �� %2d �b", min, sec);
	}else{
		sprintf(buf, "%d �b", sec);
	}
}

int getDateInt(){
	DATEDATA date;
	GetDateTime(&date);
	return (date.Year << 16) | (date.Mon << 8) | date.Day;
}

void getDateStrFromInt(char *buf, int date){
	if(date > 0){
		sprintf(buf, "%4d/%2d/%2d", (date>>16)&0xffff, (date>>8)&0xff, date&0xff);
	}else{
		strcpy(buf,"----/--/--");
	}
}

void getDateStr(char *buf){
	DATEDATA date;
	GetDateTime(&date);
	sprintf(buf, "%d/%02d/%02d %02d:%02d:%02d", date.Year, date.Mon,
		date.Day, date.Hour, date.Min, date.Sec);
}

void outputError(char *str){
	char buf[256];
	getDateStr(buf);
	FILE *fp;
	fp = fopen("�G���[���O.txt","a");
	fprintf(fp, "[%s] %s\n", buf, str);
	fclose(fp);
}

/* ------------------------------------------------------------ */
/* �L�[���͂Ńo�b�t�@�𑀍삷�� */

#define editBuffer(ch, buf, len) editBuffer_1(ch, buf, len, sizeof(buf))

int editBuffer_1(char ch, char *buf, int &len, int bufSize){
	if(ch < CTRL_CODE_CMP){	/* �����R�[�h�łȂ��Ƃ� */
		switch(ch){
		case CTRL_CODE_CR:	/* ���s�Ȃ�m�� */
			return EDIT_BUFFER_OK;
		case CTRL_CODE_BS:	/* BackSpace�⍶�L�[��Delete�Ȃ� */
		case CTRL_CODE_LEFT:
		case CTRL_CODE_DEL:
			if(len > 0){
				len--;	/* �ꕶ���폜 */
			}
			buf[len] = '\0';
			break;
		case CTRL_CODE_ESC:
			return EDIT_BUFFER_CANCEL;
		}
		return 0;
	}
	if(len < bufSize - 1){
		buf[len++] = ch;
		buf[len] = '\0';
	}
	return 0;
}

/* ============================================================ */

#if 0
HANDLE newThread(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter);
void deleteThread(HANDLE handle);

//#ifdef UTYPING_USE_MULTI_THREAD

/* DWORD WINAPI (*func)(LPVOID)��V�����X���b�h�Ƃ��č�� */
HANDLE newThread(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter){
	DWORD dwID;
	return CreateThread(NULL,0,lpStartAddress,lpParameter,0,&dwID);
}

void deleteThread(HANDLE handle){
	WaitForSingleObject(handle, INFINITE);	/* 3000���炢�ɂ��Ă���������������������Ȃ� */
	/*
	if(!TerminateThread(handle, 0)){
		throw __LINE__;
	}
	*/
	CloseHandle(handle);
}

//#endif
#endif


#endif
