#!/bin/csh

########################################################################################################################################
# ע�����
#
#1������ű�MakeMP.sh�ϴ���linux���Ի�����Ҫ�޸Ľű��ļ�����Ϊ��ִ�У���ִ��dos2unix����ת��������ű��޷�����
#
#2: ����ű�MakeMP.sh֧��suse10��suse11��redhat54����ϵͳ��ʹ��ʱ����ʾ�������
#
########################################################################################################################################

set IVS_MP_DIR = `pwd`

set version = debug
set osver = suse11

echo "#######################################################"
echo "#	Choose Correct Compile Option[1 - 8]"	
echo "#######################################################"
echo "#	 1> os=suse10 		version=debug"
echo "#"
echo "#	 2> os=suse10 		version=release"
echo "#"
echo "#	 3> os=suse11 		version=debug"
echo "#"
echo "#	 4> os=suse11 		version=release"
echo "#"
echo "#	 5> os=redhat54 	version=debug"
echo "#"
echo "#	 6> os=redhat54 	version=release"
echo "#"
echo "#	 7> os=v100nptl 	version=debug"
echo "#"
echo "#	 8> os=v100nptl 	version=release"
echo "#######################################################"
set Num=($<)
switch ($Num)
	case 1:
		set version = debug
		set osver = suse10
		breaksw
	case 2:
		set version = release
		set osver = suse10
		breaksw
	case 3:
		set version = debug
		set osver = suse11
		breaksw
	case 4:
		set version = release
		set osver = suse11
		breaksw
	case 5:
		set version = debug
		set osver = redhat54
		breaksw
	case 6:
		set version = release
		set osver = redhat54
		breaksw
	case 7:
		set version = debug
		set osver = v100nptl
		breaksw
	case 8:
		set version = release
		set osver = v100nptl
		breaksw
	default:
		set version = debug
		set osver = suse11
		echo "You chose compile option: None"
		breaksw
endsw

#####################################################
#����MP
#####################################################
echo
echo "@@@@@@@  start make MP  @@@@@@@"
cd $IVS_MP_DIR
make clean ver=$version os=$osver
make ver=$version os=$osver
make copy ver=$version os=$osver
echo "@@@@@@@ make MP success @@@@@@@"
echo
