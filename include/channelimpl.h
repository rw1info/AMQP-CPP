/**
 *  ChannelImpl.h
 *
 *  Extended channel object that is used internally by the library, but
 *  that has a private constructor so that it can not be used from outside
 *  the AMQP library
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace AMQP {

/**
 *  Class definition
 */
class ChannelImpl
{
private:
    /**
     *  The actual channel object
     *  @var    Channel
     */
    Channel *_parent;

    /**
     *  Pointer to the connection
     *  @var    Connection
     */
    Connection *_connection;

    /**
     *  The handler that is notified about events
     *  @var    MyChannelHandler
     */
    ChannelHandler *_handler;
    
    /**
     *  The channel number
     *  @var uint16_t
     */
    uint16_t _id;

    /**
     *  State of the channel object
     *  @var enum
     */
    enum {
        state_connected,
        state_closed
    } _state = state_connected;
    
    /**
     *  Is a transaction now active?
     *  @var bool
     */
    bool _transaction = false;

    /**
     *  Construct a channel object
     *  
     *  Note that the constructor is private, and that the Channel class is
     *  a friend. By doing this we ensure that nobody can instantiate this
     *  object, and that it can thus only be used inside the library.
     * 
     *  @param  parent          the public channel object
     *  @param  connection      pointer to the connection
     *  @param  handler         handler that is notified on events
     */
    ChannelImpl(Channel *parent, Connection *connection, ChannelHandler *handler = nullptr);

public:
    /**
     *  Destructor
     */
    virtual ~ChannelImpl();

    /**
     *  Pause deliveries on a channel
     * 
     *  This will stop all incoming messages
     *  
     *  This method returns true if the request to pause has been sent to the
     *  broker. This does not necessarily mean that the channel is already
     *  paused. 
     * 
     *  @return bool
     */
    bool pause();
    
    /**
     *  Resume a paused channel
     * 
     *  @return bool
     */
    bool resume();

    /**
     *  Is the channel connected?
     *  @return bool
     */
    bool connected()
    {
        return _state == state_connected;
    }
    
    /**
     *  Start a transaction
     *  @return bool
     */
    bool startTransaction();
    
    /**
     *  Commit the current transaction
     *  @return bool
     */
    bool commitTransaction();
    
    /**
     *  Rollback the current transaction
     *  @return bool
     */
    bool rollbackTransaction();
    
    /**
     *  declare an exchange
     *  @param  name        name of the exchange to declare
     *  @param  type        type of exchange
     *  @param  flags       additional settings for the exchange
     *  @param  arguments   additional arguments
     *  @return bool
     */
    bool declareExchange(const std::string &name, ExchangeType type, int flags, const Table &arguments);
    
    /**
     *  bind two exchanges
     *  @param  source      exchange which binds to target
     *  @param  target      exchange to bind to
     *  @param  routingKey  routing key
     *  @param  glags       additional flags
     *  @param  arguments   additional arguments for binding
     *  @return bool
     */
    bool bindExchange(const std::string &source, const std::string &target, const std::string &routingkey, int flags, const Table &arguments);
    
    /**
     *  unbind two exchanges
     *  @param  source      the source exchange
     *  @param  target      the target exchange
     *  @param  routingkey  the routing key
     *  @param  flags       optional flags
     *  @param  arguments   additional unbind arguments
     *  @return bool
     */
    bool unbindExchange(const std::string &source, const std::string &target, const std::string &routingkey, int flags, const Table &arguments);
    
    /**
     *  remove an exchange
     *  @param  name        name of the exchange to remove
     *  @param  flags       additional settings for deleting the exchange
     *  @return bool
     */
    bool removeExchange(const std::string &name, int flags);
    
    /**
     *  declare a queue
     *  @param  name        queue name
     *  @param  flags       additional settings for the queue
     *  @param  arguments   additional arguments
     *  @return bool
     */
    bool declareQueue(const std::string &name, int flags, const Table &arguments);
    
    /**
     *  Bind a queue to an exchange
     *  @param  exchangeName    name of the exchange to bind to
     *  @param  queueName       name of the queue
     *  @param  routingkey      routingkey
     *  @param  flags           additional flags
     *  @param  arguments       additional arguments
     *  @return bool
     */
    bool bindQueue(const std::string &exchangeName, const std::string &queueName, const std::string &routingkey, int flags, const Table &arguments);

    /**
     *  Unbind a queue from an exchange
     *  @param  exchange    the source exchange
     *  @param  queue       the target queue
     *  @param  routingkey  the routing key
     *  @param  arguments   additional bind arguments
     *  @return bool
     */
    bool unbindQueue(const std::string &exchangeName, const std::string &queueName, const std::string &routingkey, const Table &arguments);
    
    /**
     *  Purge a queue
     *  @param  queue       queue to purge
     *  @param  flags       additional flags
     *  @return bool
     */
    bool purgeQueue(const std::string &name, int flags);
    
    /**
     *  Remove a queue
     *  @param  queue       queue to remove
     *  @param  flags       additional flags
     *  @return bool
     */
    bool removeQueue(const std::string &name, int flags);
    
    /**
     *  Close the current channel
     *  @return bool
     */
    bool close();

    /**
     *  Get the channel we're working on
     *  @return uint16_t
     */
    const uint16_t id() const
    {
        return _id;
    }
    
    /**
     *  Send a frame over the channel
     *  @param  frame       frame to send
     *  @return size_t      number of bytes sent
     */
    size_t send(const Frame &frame);
    
    /**
     *  Report to the handler that the channel is closed
     */
    void reportClosed()
    {
        // change state
        _state = state_closed;
        
        // inform handler
        if (_handler) _handler->onClosed(_parent);
    }
    
    /**
     *  Report to the handler that the channel is paused
     */
    void reportPaused()
    {
        // inform handler
        if (_handler) _handler->onPaused(_parent);
    }
    
    /**
     *  Report to the handler that the channel is resumed
     */
    void reportResumed()
    {
        // inform handler
        if (_handler) _handler->onResumed(_parent);
    }
    
    /**
     *  Report to the handler that the channel is opened
     */
    void reportReady()
    {
        // inform handler
        if (_handler) _handler->onReady(_parent);
    }
    
    /**
     *  Report an error message on a channel
     *  @param  message
     */
    void reportChannelError(const std::string &message)
    {
        // change state
        _state = state_closed;
        
        // inform handler
        if (_handler) _handler->onError(_parent, message);
    }

    /**
     *  Report that the exchange is succesfully declared
     */
    void reportExchangeDeclared()
    {
        if(_handler) _handler->onExchangeDeclared(_parent);
    }
    
    /**
     *  Report that the exchange is succesfully deleted
     */
    void reportExchangeDeleted()
    {
        if(_handler) _handler->onExchangeDeleted(_parent);
    }
    
    /**
     *  Report that the exchange is bound
     */
    void reportExchangeBound()
    {
        if(_handler) _handler->onExchangeBound(_parent);
    }
    
    /**
     *  Report that the exchange is unbound
     */
    void reportExchangeUnbound()
    {
        if(_handler) _handler->onExchangeUnbound(_parent);
    }
    
    /**
     *  Report that the queue was succesfully declared
     *  @param  queueName       name of the queue which was declared
     *  @param  messagecount    number of messages currently in the queue
     *  @param  consumerCount   number of active consumers in the queue
     */
    void reportQueueDeclared(const std::string &queueName, uint32_t messageCount, uint32_t consumerCount)
    {
        if(_handler) _handler->onQueueDeclared(_parent, queueName, messageCount, consumerCount);
    }
    
    /**
     *  Report that a queue was succesfully bound
     */
    void reportQueueBound()
    {
        if(_handler) _handler->onQueueBound(_parent);
    }
    
    /**
     *  Report that a queue was succesfully unbound
     */
    void reportQueueUnbound()
    {
        if(_handler) _handler->onQueueUnbound(_parent);
    }
    
    /**
     *  Report that a queue was succesfully deleted
     *  @param  messageCount    number of messages left in queue, now deleted
     */
    void reportQueueDeleted(uint32_t messageCount)
    {
        if(_handler) _handler->onQueueDeleted(_parent, messageCount);
    }
    
    /**
     *  Report that a queue was succesfully purged
     *  @param  messageCount    number of messages purged
     */
    void reportQueuePurged(uint32_t messageCount)
    {
        if(_handler) _handler->onQueuePurged(_parent, messageCount);
    }
    
    /**
     *  The channel class is its friend, thus can it instantiate this object
     */
    friend class Channel;
    
};

/**
 *  End of namespace
 */
}

