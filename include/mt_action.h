/*
 * Copyright (C) zhoulv2000@163.com
 */

#ifndef _MT_ACTION_H_INCLUDED_
#define _MT_ACTION_H_INCLUDED_

#include "mt_utils.h"
#include "mt_buffer.h"
#include "mt_event_proxyer.h"
#include "mt_connection.h"
#include "mt_c.h"
#include "mt_frame.h"

MTHREAD_NAMESPACE_USING

class IMessage
{
public:
    // 虚函数，子类继承
    virtual int HandleProcess() 
    { 
        return -1; 
    }
    IMessage() 
    { }
    virtual ~IMessage() 
    { }
    inline void SetDataPtr(void *data)
    {
        m_data_ = data;
    }
    inline void* GetDataPtr()
    {
        return m_data_;
    }
    
private:
    void *m_data_;
};

// IMtAction继承ISession
class IMtAction : public ISession, public IMtActionBase
{
public:
    IMtAction();
    virtual ~IMtAction();

    inline void SetMsgBufferSize(int buff_size)
    {
        m_buff_size_ = buff_size;
    }
    inline int GetMsgBufferSize()
    {
        return (m_buff_size_ > 0) ? m_buff_size_ : 65535;
    }
    inline void SetConnType(eConnType type)
    {
        m_conn_type_ = type;
    }
    inline eConnType GetConnType()
    {
        return m_conn_type_;
    }
    inline void SetErrno(eActionError err)
    {
        m_errno_ = err;
    }
    inline eActionError GetErrno()
    {
        return m_errno_;
    }
    inline void SetCost(int cost)
    {
        m_time_cost_ = cost;
    }
    inline int GetCost()
    {
        return m_time_cost_;
    }
    inline void SetMsgFlag(eActionState flag)
    {
        m_flag_ = flag;
    }
    inline eActionState GetMsgFlag()
    {
        return m_flag_;
    }
    inline void SetIMessagePtr(IMessage* msg)
    {
        m_msg_ = msg;
    }
    inline IMessage* GetIMessagePtr()
    {
        return m_msg_;
    }
    inline void SetIConnection(IMtConnection* conn)
    {
        m_conn_ = conn;
    }
    inline IMtConnection* GetIConnection()
    {
        return m_conn_;
    }
    inline ThreadBase* GetOwnerThread()
    {
        return m_thread_;
    }

    Eventer* GetEventer();

    int InitConnection();
    void Reset(bool reset_all = false);

public:
    virtual int HandleEncode(void* buf, int& len, IMessage* msg)
    {
        return 0;
    }
    virtual int HandleInput(void* buf, int len, IMessage* msg)
    {
        return 0;
    }
    virtual int HandleProcess(void* buf, int len, IMessage* msg)
    {
        return 0;
    }
    virtual int HandleError(int err, IMessage* msg)
    {
        return 0;
    }

    // 四个处理阶段
    virtual int DoEncode();
    virtual int DoInput();
    virtual int DoProcess();
    virtual int DoError();

protected:
    eActionState        m_flag_;
    eConnType           m_conn_type_;
    eActionError        m_errno_;

    int                 m_time_cost_;
    int                 m_buff_size_;

    IMessage            *m_msg_;
    IMtConnection       *m_conn_;
    ThreadBase          *m_thread_;
};

class IMtActionClient
{
    typedef std::vector<IMtAction*> IMtActionList;

public:
    inline void Add(IMtAction* action)
    {
        m_action_list_.push_back(action);
        LOG_TRACE("m_action_list_ size : %d", m_action_list_.size());
    }

    int SendRecv(int timeout);

    int Poll(IMtActionList list, int mask, int timeout);

    int NewSock();

    int Open(int timeout);

    int Sendto(int timeout);

    int Recvfrom(int timeout);

    int RunSendRecv(int timeout);

private:
    IMtActionList m_action_list_;
};

class IMtActionServer
{
public:
    IMtActionServer() : m_conn_(NULL), 
        m_action_(NULL), m_accept_timeout_(50)
    { }

    inline void SetIMtAction(IMtAction* action)
    {
        m_action_ = action;
    }

    inline IMtActionBase* GetIMtActon()
    {
        return m_action_;
    }

    inline IMtConnection* GetConnection()
    {
        return m_conn_;
    }

    inline int GetAcceptTimeout()
    {
        return m_accept_timeout_;
    }

    inline void SetLocalAddr(struct sockaddr_in addr)
    {
        memcpy(&m_localaddr_, &addr, sizeof(addr));
    }
    
    int NewSock();

    int Accept(int timeout);

public:
    static void ListenFd(void *args);

private:
    IMtAction *m_action_;
    struct sockaddr_in m_localaddr_;
    IMtConnection *m_conn_;
    int m_accept_timeout_;
};

#endif
