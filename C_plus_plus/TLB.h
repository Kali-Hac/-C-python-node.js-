#pragma once
#include "process.h"
#include<queue>
using namespace std;
#define TLB_miss -1
typedef int MB;
typedef long long ll;
//32位虚拟地址、每个页面8kb对应的虚页面有524288个
//实际64M内存页框数8192个
class TLB {
public:
	//虚页面数默认为524288
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
	void TLB_output(string file);//将系统TLB表以文件名file输出
	int Size() const { return size; }
	void load_TLB(TLB &tlb);
private:
	ll TLB_v_pages[16];//16个虚页
	ll TLB_p_pages[16];//对应16个实页框
	ll LRU_order[16];//记录存放顺序，可不用
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
		cout << "TLB的大小不相同，无法载入" << endl;
		return;
	}
	for (int i = 0; i < this->size; i++) {
		this->TLB_v_pages[i] = tlb.TLB_v_pages[i];
		this->TLB_p_pages[i] = tlb.TLB_p_pages[i];
		this->LRU_order[i] = tlb.LRU_order[i];
	}
}
