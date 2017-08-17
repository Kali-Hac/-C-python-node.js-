'use strict';
var process=require("./process");
var pp=[];
var i;
var fs=require('fs');
for(i=0;i<10;i++){
    pp.push(new process());
}
for(i=0;i<10;i++){
    pp[i].run(5000);
}

