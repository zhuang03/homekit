#ifndef _HOMEKIT_APP_H_
#define _HOMEKIT_APP_H_


class Homekit
{
private:
    /* data */
public:

   void init(void);
   void wifi_init(void);
   void wifi_connect(uint32_t delay);
   void smartconfig_connect(uint32_t delay);

    Homekit(/* args */);
    ~Homekit();
};



#endif // !_HOMEKIT_APP_H_
