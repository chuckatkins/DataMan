#include <iostream>
#include <unistd.h>
#include <sstream>
#include "StreamMan.h"
#include "zmq.h"

using namespace std;

StreamMan::StreamMan(){
}

StreamMan::StreamMan(string local_address, string remote_address)
{
    zmq_context = zmq_ctx_new ();
    zmq_tcp_req = zmq_socket (zmq_context, ZMQ_REQ);
    zmq_tcp_rep = zmq_socket (zmq_context, ZMQ_REP);
    zmq_connect (zmq_tcp_req, remote_address.c_str());
    zmq_bind (zmq_tcp_rep, local_address.c_str());
    zmq_tcp_rep_thread_active = true;
    zmq_tcp_rep_thread = new thread(&StreamMan::zmq_tcp_rep_thread_func, this);
}

StreamMan::~StreamMan(){
    cout << "~StreamMan" << endl;
    if(zmq_tcp_req) zmq_close(zmq_tcp_req);
    if(zmq_tcp_rep) zmq_close(zmq_tcp_rep);
    if(zmq_context) zmq_ctx_destroy(zmq_context);
    zmq_tcp_rep_thread_active = false;
    if(zmq_tcp_rep_thread->joinable()) zmq_tcp_rep_thread->join();
    if(zmq_tcp_rep_thread) delete zmq_tcp_rep_thread;
}

void StreamMan::flush(){
    json msg;
    msg["operation"] = "flush";
    char ret[10];
    zmq_send(zmq_tcp_req, msg.dump().c_str(), msg.dump().length(), 0);
    zmq_recv(zmq_tcp_req, ret, 10, 0);
}

void StreamMan::zmq_tcp_rep_thread_func(){
    while (zmq_tcp_rep_thread_active){
        char msg[1024]="";
        int err = zmq_recv (zmq_tcp_rep, msg, 1024, ZMQ_NOBLOCK);
        zmq_send (zmq_tcp_rep, "OK", 10, 0);
        if (err>=0){
            cout << "StreamMan::zmq_tcp_rep_thread_func: " << msg << endl;
            json j = json::parse(msg);
            if(getmode == "callback"){
                on_recv(j);
            }
        }
        usleep(10000);
    }
}


