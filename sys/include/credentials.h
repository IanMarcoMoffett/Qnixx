#ifndef _CREDENTIALS_H_
#define _CREDENTIALS_H_

#define IS_ROOT(user) (user.euid == 0)

typedef struct
{
  uid_t uid;
  uid_t euid;
  gid_t gid;
} user_t;


#endif
