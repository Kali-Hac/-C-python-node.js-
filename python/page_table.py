#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
☆*°☆*°(∩^o^)~━━  2017/8/15 12:39        
      (ˉ▽￣～) ~~ 一捆好葱 (*˙︶˙*)☆*°
      Fuction： 页表的类的构造  √ ━━━━━☆*°☆*°
"""
class page_table(object):
	physical_size=8192
	physical_frames=[0]*physical_size
	seek_frames_flag=0
	first_alloc_flag=0
	__slots__=('physical_frames','seek_frames_flag','first_alloc_flag','_f_size','_s_size','_p_table','_used_frames','_v_p_mapper')
	def __init__(self,f_size=64,s_size=8192):
		self._f_size=f_size
		self._p_table=[]
		temp=[]
		self._v_p_mapper=[0]*2
		for i in range(f_size):
			self._p_table.append(temp)
		self._used_frames=[0]*page_table.physical_size
		self._s_size=s_size

	def __str__(self):
		op_str=''
		for i in range(len(self._p_table)):
			f_table=self._p_table[i]
			if not len(f_table)==0:
				op_str+='-----[一级页表]----------第 %d 页-----\n' % i
				op_str+= "-----[二级页表]-----虚页项-----实页项-----\n"
				for j in range(len(f_table)):
					v_page=f_table[j]
					if v_page is not -1:
						op_str+='-------------------%6d-----%6d-----\n' % (j+i*64,v_page)
		des_str='-----[只显示被访问和载入/更新的页表项，不显示实页偏移量]-----'
		return  des_str+op_str

	def find_page(self,v_page):
		f_number=v_page/8192
		s_number=v_page%8192
		if not len(self._p_table[f_number])==0:
			if not self._p_table[f_number][s_number]==-1:
				return self._p_table[f_number][s_number]
			else:
				self._p_table[f_number][s_number] =self.seek_avail_frames()
		else:
			for i in range(8192):
				self._p_table[f_number].append(-1)
			self._p_table[f_number][s_number] = self.seek_avail_frames()
		self._v_p_mapper[0]=v_page
		self._v_p_mapper[1]=self._p_table[f_number][s_number]
		self._used_frames[self._v_p_mapper[1]]=1
		return -1

	def seek_avail_frames(self):
		cnt=0
		while not page_table.physical_frames[page_table.seek_frames_flag] == 0:
			page_table.seek_frames_flag=(page_table.seek_frames_flag+1)%page_table.physical_size
			cnt+=1
			if cnt>page_table.physical_size:
				page_table.first_alloc_flag=(page_table.first_alloc_flag+1)%page_table.physical_size
				return (page_table.first_alloc_flag-1+page_table.physical_size)%page_table.physical_size
				break
		return page_table.seek_frames_flag

