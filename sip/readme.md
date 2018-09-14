# Zhu Jinguo 20180913
1.编译两个库 osip 和exosip 
 ./configure
  make
  sudo make install

2包含库 
第一种做法 
 #export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
添加环境变量

第二种做法
将目录   /usr/local/lib    添加到文件中
$ sudo vim /etc/ld.so.conf
$ sudo ldconfig -v
$ export  PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig 
一般编译opencv的时候就添加了，所以 只需要第二个步骤

3.库编译结束

