//IP�A�h���X���U������IP�p�P�b�g�𑗂邱�Ƃɐ������܂����B
// ���̉摜��100.100.100.100��IP�A�h���X����200.200.200.200�Ɍ�������
// IP�v���g�R���̃p�P�b�g������ꂽ���ƂɂȂ��Ă���̂��m�F�ł��܂�
// �i���ۂɂ�192.168.0.20����192.168.0.2�֑����Ă��܂��j�B
//�@�EWindows��ŃR���p�C���E���s����ꍇ��Cygwin���ōs���Ă��������i���̃\�[�X��Linux�p�ł��j�B
// �ERAW�\�P�b�g���g���Ă���̂Ŏ��s���Ɍ����̃G���[�ɒ�G���邩���ł��B
// ���݂̓t���O�����g�ɑΉ����Ă��܂���O�O
//�ihttp://d.hatena.ne.jp/KAINS/20071112�����p�j

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <netdb.h>

#include <unistd.h>

#include <string.h>

union IpHeader {

	unsigned char c[20];
	unsigned short s[10];
	unsigned int i[5];

};

int rawsock(); unsigned short checksum(unsigned short*); 
void iptoint(char*, int*); void sends(char*, char*, int, char*); 
void printiph(IpHeader?);

extern int errno;

int main() {

	char srcIP = "";
	char destIP = "";
	char prot = "";
	char realdestIP = "";
	char YorN = "";


	printf("--------START\n");
	
	printf("�i�U���������j���M��IP�A�h���X�́H\n");
	scanf("%s\n", &srcIP);
	printf("�i�U���������j����IP�A�h���X�́H\n");
	scanf("%s\n", &destIP);
	printf("�ʐM�v���g�R���̎�ނ́H�H(def=0)\n");
	scanf("%s\n", &prot);
	printf("����IP�A�h���X�́H\n");
	scanf("%s\n", &realdestIP);

	printf("���͓��e�͂���ł�낵���ł����H\n");
	printf("�U�����M��IP�@�E�E�E%s\n", srcIP);
	printf("�U������IP�@�@�E�E�E%s\n", destIP);
	printf("�ʐM�v���g�R���E�E�E%s\n", prot);
	printf("����IP�A�h���X�E�E�E%s\n", realdestIP);
	printf("Y��������N");
	scanf("%s\n", YorN);

	if(YorN == "Y"){
//		sends("100.100.100.100", "200.200.200.200", 0, "192.168.0.2");
		sends(srcIP, destIP, prot, realdestIP);
	}
	if(YorN == "N"){
		printf("������x�N�����Ȃ����Ă�������\n");
	}
	else{
		printf("Y��N�ȊO�����͂��ꂽ���ߏI�����܂��B\n");
	}

	printf("\n--------END\n");
	return 0;

}

void printiph(IpHeader iph) {

	printf("\tVerionAndLength: %d\n", *iph.c);
	printf("\tTOS: %d\n", *(iph.c+1));
	printf("\tDatagramLength: %d\n", *(iph.s+1));
	printf("\tID: %X\n", *(iph.s+2));
	printf("\tFlag&Offset: %X\n", *(iph.s+3));
	printf("\tTTL: %d\n", *(iph.c+8));
	printf("\tProtocol: %d\n", *(iph.c+9));
	printf("\tChecksum: %X\n", *(iph.s+5));
	printf("\tSrcIP: %d.%d.%d.%d\n", *(iph.c+12), *(iph.c+13), *(iph.c+14), *(iph.c+15));
	printf("\tDestIP: %d.%d.%d.%d\n", *(iph.c+16), *(iph.c+17), *(iph.c+18), *(iph.c+19));

}

int rawsock() {

	int sock, on = 1;
	sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sock < 0) {
		perror("socket()");
		exit(1);
	}
	if ((setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on))) <0) {
		perror("setsockopt()");
		exit(1);
	}
	return sock;

}

void sends(char *srcaddress, char *destaddress, int protocol, char *realdestip) {

	IpHeader iph;
	srand(1686);
	*iph.c = 69;	//�o�[�W�����ƃw�b�_���i1�I�N�e�b�g�j
	*(iph.c+1) = 0;	//TOS�i�P�I�N�e�b�g�j
	*(iph.s+1) = 20;	//�f�[�^�O�������i�Q�I�N�e�b�g�j
	*(iph.s+2) = rand()%65536;	//ID�i�Q�I�N�e�b�g�j
	*(iph.s+3) = 0;		//�t���O�����g(�Q�I�N�e�b�g)
	*(iph.c+8) = 64;	//TTL�i�P�I�N�e�b�g�j
	*(iph.c+9) = (unsigned char)protocol;	//�v���g�R���ԍ��i�P�I�N�e�b�g�j
	*(iph.s+5) = 0;	//�`�F�b�N�T���i�Q�I�N�e�b�g�j
	int srcip[4] = {}, destip[4] = {};
	iptoint(srcaddress, srcip);
	iptoint(destaddress, destip);
	*(iph.c+12) = *srcip;	//srcIP�i�S�I�N�e�b�g�j
	*(iph.c+13) = *(srcip+1);
	*(iph.c+14) = *(srcip+2);
	*(iph.c+15) = *(srcip+3);
	*(iph.c+16) = *destip;	//destIP�i�S�I�N�e�b�g�j
	*(iph.c+17) = *(destip+1);
	*(iph.c+18) = *(destip+2);
	*(iph.c+19) = *(destip+3);
	*(iph.s+5) = checksum(iph.s);
	printiph(iph);
	printf("\t(realdestip: %s)\n", realdestip);

	int sock = rawsock();
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(realdestip);
	addr.sin_port = 0;
	int err = sendto(sock, iph.c, 20, 0, (struct sockaddr *) &addr, sizeof(addr));
	if (err < 1) {
		perror("sendto()");
		exit(1);
	}

}

unsigned short checksum(unsigned short *c) {

	unsigned int checksum = 0x0;
	for (int i=0; i<10; i++) {
		checksum += *(c+i);
		if (checksum > 0xFFFF) {
			checksum -= 0x10000;
			checksum += 0x0001;
		}
	}
	return (~(checksum) & 0xFFFF);

}

void iptoint(char *ip, int *result) {

	int n[3], pos=0;
	char buf[4][4] = {};
	for (int i=0; *(ip+i) != '\0'; i++) {
		if (*(ip+i) == '.') {
			*(n+pos) = i;
			pos++;
		}
	}
	memcpy(buf, ip, *(n+1) - *n - 1);
	memcpy((buf+1), (ip + *n + 1), *(n+1) - *n - 1);
	memcpy((buf+2), (ip + *(n+1) + 1), *(n+2) - *(n+1) - 1);	
	memcpy((buf+3), (ip + *(n+2) + 1), strlen(ip) - *(n+2) - 1);
	*result = atoi(*buf);
	*(result+1) = atoi(*(buf+1));
	*(result+2) = atoi(*(buf+2));
	*(result+3) = atoi(*(buf+3));

}