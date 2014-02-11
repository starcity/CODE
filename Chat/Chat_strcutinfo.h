#ifndef CHAT_STRCUTINFO_H
#define CHAT_STRCUTINFO_H

//msg type
#define OTHERUSERLOGIN       0x01
#define OTHERUSERLOGOUT      0x02
#define SHOWFRIENDMSG        0x03
#define SENDFRIENDMSG        0x03
#define GETUSERINFO          0x04
#define SENDFILEDATA         0x05





//filetype
#define TMPFILETYPE          0x01
#define STOREFILETYPE        0x02


//msg length define
#define MSGLENGTH            0x04
#define ID                   sizeof(int)
#define TYPE                 sizeof(char)


//other define
#define INPUTMAXNUMBER       3000

#endif // CHAT_STRCUTINFO_H
