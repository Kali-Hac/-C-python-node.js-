#pragma once
#include "process.h"
#include<queue>
using namespace std;
#define p_table_miss -1
typedef int MB;
typedef long long ll;
//32λ�����ַ��ÿ��ҳ��8kb��Ӧ����ҳ����524288��
//ʵ��64M�ڴ�ҳ����8192��
/*int physical_pframes[2500];*///���ڼ�¼�����ڴ�ҳ���Ƿ����ҳ��
class page_table {
public:
	//��ҳ����Ĭ��Ϊ524288
	page_table(int num=524288) {
		for (int i = 0; i < 64; i++) {
			first_page[i] = 0;//һ��ҳ��δ���룬��ʼ�����Ϊ0
		}
		//for (int i = 0; i < num; i++) {
		//	p_table[i] = -1;
		//}
		for (int i = 0; i < 8192; i++) {
			physical_pframes[i] = 0;
		}
		//while (!allocate_ord.empty()) {
		//	allocate_ord.pop();
		//}
		v_page_seek = 0;
	}
	static int physical_pframes[8192];//���ڼ�¼�����ڴ�ҳ���Ƿ����ҳ���ҽ��н���ռ�ñ��
	static int LRU_flag;//���ڱ�����ȷ����ȥ��ҳ��
	int first_page[64];//�ж�һ��ҳ���Ƿ�������
	//�˴�����ҳ������ҳ��Ȩ��λ��ռ��,��Ҫ������ɼ����趨Ϊ4096
	ll **p_table = new ll*[64];//��Ӧ����ҳ��ṹ��ҳĿ¼(���ҳ��)��64����ÿ��ҳ����(���ҳ�ڵ�ַ)��8192��(��Ӧ8k��ƫ����)
	ll find_Ppage(ll Vpage);
	ll get_p_page() { return v_p_reflect[1]; }
	//void copy_Pframes_available(ll Pframes_available[]);//��ϵͳ���ڴ�ҳ�������Ϣ����
	//void get_Pframes_available(ll Pframes_available[]);//��ϵͳ���ڴ�ҳ�������Ϣ����
private:
	//ll p_table[524288];//��Ӧһ��ҳ��ṹ
	//�˴�����ҳ������ҳ��Ȩ��λ��ռ��,��Ҫ������ɼ����趨Ϊ4096
	//queue<long long>allocate_ord;
	int v_page_seek;
	ll v_p_reflect[2];
};
//��̬ȫ�ֱ�����ʼ��
//��������ҳ���Ǻ����ȷ����ҳ����
int page_table::physical_pframes[8192] = { 0 };
int page_table::LRU_flag = 0;

int seek_page_frames(int &v_page_seek) {
	int cnt = 0;
	while (page_table::physical_pframes[v_page_seek] != 0 && v_page_seek < 8192) {
		v_page_seek++;
		cnt++;
		v_page_seek %= 8192;
		if (cnt == 8192) {
			return cnt = -1;
			break;
		}//����һ���غ��Ҳ����ɷ����ҳ��,������ҳ���ѱ����䣬��ʱ��Ҫ����
	}
	return v_page_seek;
}

ll page_table::find_Ppage(ll Vpage) {
	//�ȶ�λ��Ӧ��һ��ҳ���Լ�����ҳ��(һ��ҳ���ַƫ����)
	int first_p = Vpage / 8192;
	int second_p = Vpage % 8192;
	int Available_frame=-1;
	v_p_reflect[0] = Vpage;
	//��ҳ���Ҳ�����Ӧʵҳ�����ڴ��ж�ȡ������ҳ��+TLB
	if (first_page[first_p] == 1) {
		if (p_table[first_p][second_p] == -1) {
			Available_frame = seek_page_frames(v_page_seek);
			if (Available_frame == -1) {
				//���õ�һ���������ҳ��Ϊ0ҳ��Ȼ���ڽ����и�����ռ��0ҳ���ҳ��
				//������Ϊ�Ľ���ռ�ã�Ȼ���ڸ���ά����˽��ҳ��ռ�ñ��н��бȶԸ���
				p_table[first_p][second_p] = LRU_flag;
				v_p_reflect[1] = LRU_flag;
				LRU_flag = (LRU_flag + 1) % 8192;
				//��ҳ�������ȷ����ҳ������ظ����ã��˴����ö��б����Ⱥ�˳��ȡ��
				//��ʵ�����Ǵ�� ��Ϊͬһ����ռ�õ�ҳ���ܻ��գ���
					//ll v_page = allocate_ord.front();
					//int pre_first = v_page / 8192;
					//int pre_second = v_page % 8192;
					//p_table[first_p][second_p] = p_table[pre_first][pre_second];
					//v_p_reflect[1] = p_table[first_p][second_p];
					//allocate_ord.push(Vpage);
					//allocate_ord.pop();
					//p_table[pre_first][pre_second] = -1;
				
				//���ȷ���ҳ�����ҳ���ʱ�Ѿ�������ҳ��Ҫ��ʼ��Ϊ-1
			}
			else {
				//��ҳ����Ϊ����
				physical_pframes[v_page_seek] = 1;
				v_p_reflect[1] = v_page_seek;
				p_table[first_p][second_p] = v_page_seek++;
				//allocate_ord.push(Vpage);
			}
			return p_table_miss;
		}
		else {
			v_p_reflect[1] = p_table[first_p][second_p];
			return p_table[first_p][second_p];
		}
	}
	else {
		p_table[first_p] = new ll[8192];
		for (int i = 0; i < 8192; i++)
			p_table[first_p][i] = -1;
		first_page[first_p] = 1;
		Available_frame = seek_page_frames(v_page_seek);
		if (Available_frame == -1) {
			p_table[first_p][second_p] = LRU_flag;
			v_p_reflect[1] = LRU_flag;
			LRU_flag = (LRU_flag + 1) % 8192;
			//��ҳ�������ȷ����ҳ������ظ����ã��˴����ö��б����Ⱥ�˳��ȡ��
			//��ʵ�����Ǵ�� ��Ϊͬһ����ռ�õ�ҳ���ܻ��գ���
			/*ll v_page = allocate_ord.front();
			int pre_first = v_page / 8192;
			int pre_second = v_page % 8192;
			p_table[first_p][second_p] = p_table[pre_first][pre_second];
			v_p_reflect[1] = p_table[first_p][second_p];
			allocate_ord.push(Vpage);
			allocate_ord.pop();
			p_table[pre_first][pre_second] = -1;*/
			//���ȷ���ҳ�����ҳ���ʱ�Ѿ�������ҳ��Ҫ��ʼ��Ϊ-1
		}
		else {
			//��ҳ����Ϊ����
			physical_pframes[v_page_seek] = 1;
			//��process.h���ٽ�һ�����Ϊ���̺�
			v_p_reflect[1] = v_page_seek;
			p_table[first_p][second_p] = v_page_seek++;
			//allocate_ord.push(Vpage);
		}
		return p_table_miss;
	}
}

//void page_table:: copy_Pframes_available(ll Pframes_available[]) {
//	for (int i = 0; i < 2500; i++)
//		Pframes_available[i] = physical_pframes[i];
//}
//void page_table::get_Pframes_available(ll Pframes_available[]) {
//	for (int i = 0; i < 2500; i++)
//		physical_pframes[i] = Pframes_available[i];
//}