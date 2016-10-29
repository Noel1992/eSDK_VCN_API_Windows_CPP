#!/bin/csh

########################################################################################################################################
# 注意事项：
#
#1：编译脚本MakeMP.sh上传到linux测试机后，需要修改脚本文件属性为可执行，并执行dos2unix进行转换，否则脚本无法运行
#
#2: 编译脚本MakeMP.sh支持suse10、suse11和redhat54三种系统，使用时按提示输入参数
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
#编译MP
#####################################################
echo
echo "@@@@@@@  start make MP  @@@@@@@"
cd $IVS_MP_DIR
make clean ver=$version os=$osver
make ver=$version os=$osver
make copy ver=$version os=$osver
echo "@@@@@@@ make MP success @@@@@@@"
echo
