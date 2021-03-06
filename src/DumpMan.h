#ifndef DUMPMAN_H_
#define DUMPMAN_H_


#include "DataMan.h"

class DumpMan : public DataMan{
    public:
        DumpMan();
        ~DumpMan();

        virtual int init(json p_jmsg);
        virtual int put(const void *p_data, json p_jmsg);
        virtual int get(void *p_data, json &p_jmsg);
        void flush();
        string name(){return "DumpMan";}
        virtual void transform(const void* p_in, void* p_out, json &p_jmsg){};

    private:
        bool m_dumping = true;
};

extern "C" shared_ptr<DataMan> getMan(){
    return shared_ptr<DumpMan>(new DumpMan);
}



#endif
