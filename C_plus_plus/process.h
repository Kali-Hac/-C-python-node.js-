#pragma once
#include<math.h>
#include<time.h>
#include<iostream>
#include<stdio.h>
#include<fstream>
#include "page_table.h"
#include "TLB.h"
using namespace std;
typedef int MB;
typedef long long ll;
#define mb (ll)pow(2,20)
inline ll move_bits(ll ori,int bits) {
	while (ori > 0&&bits>0) {
		ori /= 2;
		bits--;
	}
	return ori;
}
class process {
public:
	process(MB size) {
		process_id++;
		process_No = process_id;
		this->size = size;
		ll  t = mb*size/2;
		high_addr[0] = (ll)pow(2, 32)-t ;
		high_addr[1] = (ll)pow(2, 32) - 1;
		low_addr[0] = 0;
		low_addr[1] = t - 1;
		run_time = 0;
		page_fault=0;
		access_times=0;
		tlb_miss=0;
		for (int i = 0; i < 8192; i++) {
			owned_p_page[i] = 0;
		}
	}
	//��ȡnum��������ʵ�ַ
	void get_access_addrs(int num);
	MB get_Size() const { return size; }
	void set_Size(MB val) { size = val; }
	//ֻ���õ�λ��ַ��ĩ�˵�ַ�͸�λ��ַ����ʼ��ַ��
	//���ڵ�λ��ַ����ʼ��ַĬ��Ϊ0x0����λ��ַ��ĩ�˵�ַĬ��Ϊ0xffffffff
	ll High_addr() const { return high_addr[0]; }
	void High_addr(ll val) { high_addr[0] = val; }
	ll Low_addr() const { return low_addr[1]; }
	void Low_addr(ll val) { low_addr[1] = val; }
	//void print_access_addr();
	int Run_time() const { return run_time; }
	void Run_time(int val) { run_time = val; }
	void run();
	TLB saveTLB() {
		return tlb;
	}
	//���ڽ�������TLB
	page_table p_table = page_table();
	static TLB tlb;
	int Process_No() const { return process_No; }
	void Process_No(int val) { process_No = val; }
	void print_page_table();
	void update_page_table();
private:
	int run_time;//�������еĴ���
	int process_No;//���̱��
	MB size;//��¼������ռ�ڴ��С
	ll high_addr[2];
	//addr[0]Ϊ��ʼ��ַ��addr[1]Ϊĩ�˵�ַ
	ll low_addr[2];
	ll access_addrs[5001];//ÿ��������ʵ�5000��ַ����
	static int process_id;//����������Ӷ�����
	int owned_p_page[8192];
	//���Դ���ı�����ҳ��ʹ����������ڶԱȡ����±����յ�ҳ�������ҳ��
	int page_fault;//ȱҳ��¼
	int access_times;//��ַ�����ܴ�����¼
	int tlb_miss;//TLB��miss��¼
};
//��̬ȫ�ֱ�����¼�ѿ��Ľ�����
int process::process_id = 0;
TLB process::tlb = TLB(16);
//����TLB����ʼ��
void process::get_access_addrs(int num) {
	access_times += num;
	run_time++;
	ll cnt = 0;
	int xh = num / 20;
	srand((unsigned)time(NULL));
	//�ߵ�ַѰַ10�Σ��͵�ַѰַ10��
	for (int i = 0; i < xh; i++) {
		int mid = max(16, size );
		ll temp = rand() %(max(32,size))+mid;
		//cout << "size:" <<size<<" "<< temp << endl;
		//��ǿ����ԣ������������temp����8k=2^13Ϊ���ӵ�λ
		access_addrs[cnt++]=rand() % low_addr[1]+size*temp*pow(2,14);
		for (int j = 0; j < 9; j++) {
			access_addrs[cnt++] = access_addrs[cnt - 1] + 1;
		}
		ll t2 = rand() % (high_addr[1] - high_addr[0]) + high_addr[0];
		temp = size*temp*pow(2,13)+t2;
		//���������⼰����
		while (temp>pow(2,32)) {
			temp /= 2;
		}
		while (temp>Low_addr()&&temp<High_addr()) {
			temp /= 2;
		}
		access_addrs[cnt++] =temp;
		for (int j = 0; j < 9; j++) {
			access_addrs[cnt++] = access_addrs[cnt - 1] + 1;
		}
	}
}

void process::update_page_table() {
	for (int i = 0; i < 8192; i++) {
		if (owned_p_page[i] == process_No&&p_table.physical_pframes[i] != process_No) {
			//˵��ҳ���ѱ���Ľ���ռ�ã���Ҫ����ҳ��
			for (int j = 0; j < 64; j++) {
				if (p_table.first_page[j] == 1) {
					//һ��ҳ��������ĲŸ���
					for (int k = 0; k < 8192; k++) {
						if (p_table.p_table[j][k] == i) {
							p_table.p_table[j][k] = -1;
							//�������ַ�´η��ʵ�ʱ����Ҫ��Ѱҳ��
						}
					}
				}
			}
		}
	}
}

void process::run() {
	update_page_table();
	//����ҳ��ĸ���
	char id[20];
	sprintf_s(id, "visit_seq_%d.txt", process_No);
	ofstream outfile(id,ios::app);
	//���л����д�������������
	cout<< "-----������Ϣ-----���-----"<<process_No<<"-----�ڴ棺" << get_Size() << "M-----" << "���н���-----���л����д�����" << run_time << "-----" << endl;
	outfile << "-----������Ϣ-----�ڴ棺" << get_Size() << "M-----" << "���л����д�����" << run_time <<"-----"<< endl << "-----500�������ַ��������-----"<<endl;
	outfile << "Address       VP number    PF number    TLB    PageTable\n";
	char addr_str[20],v_page[20];
	for (int i = 0; i < 500; i++) {
		ll t = move_bits(access_addrs[i], 13);
		int tlb_result=tlb.find_Ppage(t),p_table_result=-2;
		char *TLB_hit = "hit";
		char *p_table_hit = "hit";
		char LRU_frames[100];
		//ҳ��ȫ��������Ҫ���գ�LRU_framesΪLRU�������ҳ�����ʾ���
		ll p_page;
		if (tlb_result == TLB_miss) {
			p_table_result = p_table.find_Ppage(t);
			p_page = p_table.get_p_page();
			if (p_table_result != -1) {
				p_page = p_table_result;
			}
			tlb.add_TLB_item(t, p_page);
		}
		else {
			p_page = tlb.find_Ppage(t);
		}
		if (tlb_result == -1) {
			TLB_hit = "miss";
			tlb_miss++;
		}
		if (p_table_result == -1) {
			p_table_hit = "miss";
			page_fault++;
			if (p_table.physical_pframes[p_page] != 0) {
				sprintf_s(LRU_frames, "-----���޿���ҳ�򣬽��������ȷ����ҳ��%lld�ӽ���id��%d���գ������¸ý��̵�ҳ��-----", p_page,p_table.physical_pframes[p_page]);
				outfile << LRU_frames << endl;
			}
			p_table.physical_pframes[p_page] = process_No;
			owned_p_page[p_page] = process_No;
			//������ϵͳ��ҳ�򣬼�¼������Ĵ洢���ձ���
		}
		//cout << t << endl;
		sprintf_s(addr_str,"0x%08x",access_addrs[i]);
		//sprintf_s(v_page, "0x%05x", t);
		outfile << addr_str <<"\t"<<t<<"\t      "<<p_page<<"\t       "<<TLB_hit<<"\t"<<p_table_hit<< '\n';
		//cout << access_addrs[i] << endl;
	}
	outfile.close();
}
//��������ʱ����ϵͳTLB
//�˹����Ѳ���Ҫ TLBΪ���н���������

void process::print_page_table() {
	char id[20];
	sprintf_s(id, "page_table_%d.txt", process_No);
	ofstream outfile(id, ios::app);
	outfile << "-----[ֻ��ʾ�����ʺ�����/���µ�ҳ�������ʾʵҳƫ����]-----" << endl;
	for (int i = 0; i < 64; i++) {
		if (p_table.first_page[i] == 1) {
			outfile << "-----[һ��ҳ��]-----" << "-----��" << i << "ҳ-----" << endl;
			outfile << "-----[����ҳ��]-----��ҳ��-----ʵҳ��-----" << endl;
			for (int j = 0; j < 8192; j++) {
				if (p_table.p_table[i][j] != -1) {
					ll v_page = i * 8192 + j;
					outfile << "-------------------" << v_page << "-----" << p_table.p_table[i][j] << endl;
				}
			}
		}
	}
	outfile << "\n-----Page Fault Rate-----" << (double)page_fault / (double)access_times *100.0<<"%"<< endl;
	outfile << "\n-----TLB Miss Rate-----" << (double)tlb_miss / (double)access_times *100.0 << "%" << endl;
	outfile.close();
}
//int binary_search(ll v_page,ll page_table[]) {
//	int l = 0; 
//	int r = 524287;//��ҳ�������524288��
//	int mid = (l + r) / 2;
//	while (l != r) {
//		if (page_table[mid] == v_page)
//			return mid;
//		if (page_table[mid] < v_page)
//			l = mid + 1;
//		else
//			r = mid;
//	}
//	return -1;
//}