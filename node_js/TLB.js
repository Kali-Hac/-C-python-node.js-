'use strict';
var i;
class TLB{
    constructor(size=16){
        this.size=16;
        this.TLB_v_page=[];
        this.TLB_p_page=[];
        for(i=0;i<size;i++){
            this.TLB_v_page.push(-1);
            this.TLB_p_page.push(-1);
        }
    }
	des_str(){
        var des_str='';
        for(i=0;i<this.size;i++){
            des_str+='-----'+this.TLB_v_page[i]+'-----'+this.TLB_p_page[i]+'-----\n';
        }
		str='-----V_page-----P_page-----';
		return '-----TLB表的大小为：'+this.size+'-----所有的表项对应内容如下-----\n' +str+'\n'+des_str;
    }
    add_item(v_page,p_page){
        this.TLB_v_page.pop();
		this.TLB_p_page.pop();
		this.TLB_v_page.splice(0,0,v_page);
		this.TLB_p_page.splice(0,0,p_page);
    }

	find_page(v_page){
        for(i=0;i<this.size;i++){
                if (this.TLB_v_page[i]===v_page){
                    var p_page=this.TLB_p_page[i];
                    var index=this.TLB_p_page.indexOf(p_page);
                    this.TLB_p_page.pop(index);
                    this.TLB_p_page.splice(0,0,p_page);
                    this.TLB_v_page.pop(index);
                    this.TLB_v_page.splice(0,0,v_page);
                    return p_page;
                }
            }
        return -1;
        }
    }

    module.exports = TLB;