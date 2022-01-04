#ifndef SERVICE_H
#define SERVICE_H
#include <type_traits>

namespace srv {

   /****************************
            ��¼
   ****************************/

   struct LoginReq
   {
      char user[20] = { 0 };
      char pass[20] = { 0 };
   };
   struct LoginRes
   {
      int success = 0;
   };

   /****************************
            ע��
   ****************************/

   struct RegistReq
   {
      char user[20] = { 0 };
      char pass[20] = { 0 };
   };
   struct RegistRes
   {
      int success = 0;
   };

   /****************************
            ����ֱ��
   ****************************/

   struct StartLiveReq
   {
      char user[20] = { 0 };
      char url[64] = { 0 };
   };

   struct StartLiveRes
   {
      int success = 0;
   };

   struct LivingUserRec
   {
      char user[20] = { 0 };
      char url[64] = { 0 }; //rtmp://192.168.222.222/live/room1
   };

   /****************************
            ֱ������
   ****************************/

   struct WatchLiveReq //���ڽ���
   {
      char streamer[20] = { 0 }; //������
      char audience[20] = { 0 }; //������
   };

   struct RoomInfoRec
   {
      char online_name[20] = { 0 };
   };

   struct MessageReq
   {
      char user[20] = { 0 };
      char msg[100] = { 0 };
   };

   /****************************
            ��ȡҵ��id��
   ****************************/

   template <typename T>
   constexpr int ServiceId()
   {
      if      (std::is_same<T, LoginReq>::value)      return 1;
      else if (std::is_same<T, LoginRes>::value)      return 2;
      else if (std::is_same<T, RegistReq>::value)     return 3;
      else if (std::is_same<T, RegistRes>::value)     return 4;
      else if (std::is_same<T, StartLiveReq>::value)  return 5;
      else if (std::is_same<T, StartLiveRes>::value)  return 6;
      else if (std::is_same<T, LivingUserRec>::value) return 7;
      else if (std::is_same<T, WatchLiveReq>::value)  return 8;
      else if (std::is_same<T, RoomInfoRec>::value)   return 9;
      else if (std::is_same<T, MessageReq>::value)    return 10;
      else return 0;
   }

   /****************************
            ���ݰ���ͷ
   ****************************/

   template <typename T>
   struct PackHead
   {
      PackHead(int count = 1) : body_count(count), ser_type(ServiceId<T>()), body_len(sizeof(T) * count) {}
      int body_count = 0;
      int ser_type = 0;
      int body_len = 0;
   };

}

#endif // SERVICE_H
