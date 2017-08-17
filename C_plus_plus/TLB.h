#pragma once
#include "process.h"
#include<queue>
using namespace std;
#define TLB_miss -1
typedef int MB;
typedef long long ll;
//32λ�����ַ��ÿ��ҳ��8kb��Ӧ����ҳ����524288��
//ʵ��64M�ڴ�ҳ����8192��
class TLB {
public:
	//��ҳ����Ĭ��Ϊ524288
	TLB(TLB &tlb) {
		this->size = tlb.Size();
		for (int i = 0; i < this->size; i++) {
			this->TLB_v_pages[i] = tlb.TLB_v_pages[i];
			this->TLB_p_pages[i] = tlb.TLB_p_pages[i];
			this->LRU_order[i] = tlb.LRU_order[i];
		}
	}
	TLB(int num = 16) {
		size = num;
		for (int i = 0; i < num; i++) {
			TLB_v_pages[i] = -1;
			TLB_p_pages[i] = -1;
			LRU_order[i] = -1;
		}
	}
	ll find_Ppage(ll Vpage) {
		for (int i = 0; i < 16; i++) {
			if (TLB_v_pages[i] == Vpage) {
				ll v_temp = Vpage;
				ll p_temp = TLB_p_pages[i];
				for (int j = i; j > 0; j--) {
					TLB_v_pages[j] = TLB_v_pages[j- 1];
					TLB_p_pages[j] = TLB_p_pages[j - 1];
				}
				TLB_p_pages[0] = p_temp;
				TLB_v_pages[0] = v_temp;
				return TLB_p_pages[i];
			}
		}
		return TLB_miss;
	}
	void add_TLB_item(ll v_page, ll p_page);
	void TLB_output(string file);//��ϵͳTLB�����ļ���file���
	int Size() const { return size; }
	void load_TLB(TLB &tlb);
private:
	ll TLB_v_pages[16];//16����ҳ
	ll TLB_p_pages[16];//��Ӧ16��ʵҳ��
	ll LRU_order[16];//��¼���˳�򣬿ɲ���
	int size;
};

void TLB::add_TLB_item(ll v_page, ll p_page) {
	for (int j = 15; j > 0; j--) {
		TLB_v_pages[j] = TLB_v_pages[j - 1];
		TLB_p_pages[j] = TLB_p_pages[j - 1];
	}
	TLB_p_pages[0] = p_page;
	TLB_v_pages[0] = v_page;
}
void TLB::TLB_output(string file) {
	ofstream outfile(file);
	outfile << "V-page-n      " << "P-page-n" << endl;
	for (int i = 0; i < 16; i++) {
		outfile << TLB_v_pages[i] << "\t\t" << TLB_p_pages[i] << endl;
	}
	outfile.close();
}
void TLB::load_TLB(TLB &tlb) {
	if (tlb.size != this->size) {
		cout << "TLB�Ĵ�С����ͬ���޷�����" << endl;
		return;
	}
	for (int i = 0; i < this->size; i++) {
		this->TLB_v_pages[i] = tlb.TLB_v_pages[i];
		this->TLB_p_pages[i] = tlb.TLB_p_pages[i];
		this->LRU_order[i] = tlb.LRU_order[i];
	}
}
