#!/bin/bash
 
# 用来编译这两个sb玩意儿 by jinguo 20180913 

gcc uac.c -o uac -losip2 -leXosip2 -lpthread  -losipparser2 
gcc uas.c -o uas -losip2 -leXosip2 -lpthread  -losipparser2
