#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
☆*°☆*°(∩^o^)~━━  2017/8/15 12:39        
      (ˉ▽￣～) ~~ 一捆好葱 (*˙︶˙*)☆*°
      Fuction：TLB表的类的构造 √ ━━━━━☆*°☆*°
"""
#TLB有个共享页表的bug---多个进程可能通过TLB先寻到了非己进程的页表项然后直接使用-----打完再修复
class TLB(object):
	__slots__=('size','TLB_v_page','TLB_p_page')
	def __init__(self,size=16):
		self.size=size
		self.TLB_v_page=[-1]*size
		self.TLB_p_page=[-1]*size

	def __str__(self):
		des_str=''
		for i in range(self.size):
			des_str+='-----%6d-----%6d-----\n' % (self.TLB_v_page[i],self.TLB_p_page[i])
		str='-----V_page-----P_page-----'
		return '-----TLB表的大小为：%d-----所有的表项对应内容如下-----\n' % self.size+str+'\n'+des_str

	def add_item(self,v_page,p_page):
		self.TLB_v_page.pop()
		self.TLB_p_page.pop()
		self.TLB_v_page.insert(0,v_page)
		self.TLB_p_page.insert(0,p_page)

	def find_page(self,v_page):
		for i in range(self.size):
			if self.TLB_v_page[i]==v_page:
				p_page=self.TLB_p_page[i]
				index=self.TLB_p_page.index(p_page)
				self.TLB_p_page.pop(index)
				self.TLB_p_page.insert(0,p_page)
				self.TLB_v_page.pop(index)
				self.TLB_v_page.insert(0,v_page)
				return p_page
		return -1