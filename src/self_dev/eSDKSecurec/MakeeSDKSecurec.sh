#!/bin/sh

########################################################################################################################################
# ע�����
#
#1������ű�MakeeSDKSecurec.sh�ϴ���linux���Ի�����Ҫ�޸Ľű��ļ�����Ϊ��ִ�У���ִ��dos2unix����ת��������ű��޷�����
#
#2: ����ű�MakeeSDKSecurec.sh֧�֣�
#	1��bit����ѡ�������Ŀǰ֧��64��32λ���ֲ���ֵ��ʹ��ʱ��Ҫ���Ϲؼ���bit��ָʾ��Ĭ�ϲ���ֵΪ64λ��
#���磺	./MakeeSDKSecurec.sh bit 32		bitΪ32
#		./MakeeSDKSecurec.sh			bitΪ64
########################################################################################################################################

eSDK_WORK_DIR=`pwd`
Huawei_Secure_DIR=$eSDK_WORK_DIR/src/Huawei_Secure_C_V100R001C01B019
eSDK_Secure_DIR=$eSDK_WORK_DIR/src

bit=64
osver=suse11

#��ȡϵͳ��Ϣ
<<EOF
SYSTEMINFO=`uname -m |grep x86_64`
echo "$SYSTEMINFO"
if [ -n "$SYSTEMINFO" ]
then
	bit=64
else
	bit=32
fi
EOF


if [ -f /etc/SuSE-release ]
then
	SYSTEMINFO=`cat /etc/SuSE-release |grep "VERSION" | awk '{print $3}'`
else
	SYSTEMINFO=""
fi

if [ "X$SYSTEMINFO" == "X11" ]
then
	osver=suse11
elif [ "X$SYSTEMINFO" == "X10" ]
then
	osver=suse10
else
	osver=suse11
fi

while [ $# != 0 ]
do
	if [ $1 == bit ] 
	then
		bit=$2
	elif [ $1 == os ] 
	then
		osver=$2
	fi
	shift
done

if [ $osver == v100nptl ]
then
	bit=32
fi
echo $bit
echo $osver
if [ $bit != 32 ] && [ $bit != 64 ] 
then
	echo "parameter error! please input 'bit''64' or '32'"
	exit
fi

cd ${Huawei_Secure_DIR}/src
if [ $bit == 64 ] 
then
	make clean -f Makefile64 os=$osver
	make -f Makefile64 os=$osver
elif [ $bit == 32 ] 
then
	make clean -f Makefile #os=$osver
	make -f Makefile #os=$osver
fi

cd $eSDK_Secure_DIR
make clean -f Makefile bit=$bit os=$osver
make -f Makefile bit=$bit os=$osver
make copy -f Makefile bit=$bit os=$osver






