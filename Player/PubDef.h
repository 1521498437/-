/**
* @file PubDef.h
* @brief ����������
* @authors yandaoyang
* @date 2020/12/27
* @note Ŀǰ���ڿ������Ӳ����
*/

#pragma once

#define SAFE_DEL(ptr) if(ptr){delete ptr;ptr=nullptr;}


//#define HARDWARE_DECODE


/* 
* ĿǰFont Awesome�ֿ�����Ӧ�õ�ͼ���Ӧ���ַ���ɲο�������վ��
* http://www.fontawesome.com.cn/cheatsheet/
* 
* Ŀǰ��δʵ�֣����뵽�ļ��������ﲹ�䣩��
* 1�����ϡ����ϡ����¡����ϵ���ק�ı䴰�ڴ�С
* 2����ͣ��ʱ��seek����ʾ��seek���Ŀ�껭�棬���ı���ͣ״̬
* 3����Ƶֹͣ�����١���һ������һ����˫��ȫ��/�ָ���������ͣ/�ָ�
* 4��������������ͼ���ա����ý��桢��Ƶ�б�
* 5�����������⡢�������Զ����ء�����������BaseWindow�ṩ������ק�Ľӿ�
* 6��init()Ӧ��Ϊbool���ͣ���ΪCommonWindow���麯������APP_MGRͳһinit
* 7��public��onBtnMaxClicked����Ӧ��Ϊreq�������źţ���MainPlayer��bindSlots��
* 8������޲������������ʧ��������ڰ�ť��Ϊ��
* 9��avformat_open_input��interrupt
* 6��...
*/

