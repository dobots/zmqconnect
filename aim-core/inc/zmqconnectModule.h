/**
 * This file is created at Almende B.V. It is open-source software and part of the Common 
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from 
 * thread pools and TCP/IP components to control architectures and learning algorithms. 
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software being used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * @author You
 * @copyright Your Company
 * @date 21 Jun. 2013
 * @license LGPLv3
 */

#ifndef ZMQCONNECTMODULE_H_
#define ZMQCONNECTMODULE_H_

#include <string>
#include <vector>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
// zeromq specific headers
#include <zmq.hpp>
#include <json_spirit_reader.h>
#include <pthread.h>


namespace rur {

struct Param {
  std::string module_id;
  int parameter;
};

typedef std::vector<int> long_seq;

/**
 * Port name service record. This is like a domain name service record, but instead of containing an IP address and an
 * URI, it comes with a "name" that can be resolved as a "host", "port", and "pid". The name is something like "/write",
 * the host something like "127.0.0.1" or "dev.almende.com" (that is resolvable by dns), "port" is a TCP/UDP port, and
 * "pid" is the process identifier.
 */
typedef struct pns_record_t {
  std::string name;
  std::string host;
  std::string port;
  std::string pid;
} pns_record;

// Following structure makes it easier to store state information per socket
typedef struct zmq_socket_ext_t {
  zmq::socket_t *sock;
  std::string name;
  bool ready;
} zmq_socket_ext;

class zmqconnectModule {
private:
  Param *cliParam;
  
  // the socket over which is communicated with the name server
  zmq::socket_t *ns_socket;
  // standard control socket over which commands arrive to connect to some port for example
  zmq::socket_t *cmd_socket;
  pthread_t cmdThread;
  pthread_mutex_t cmdMutex;
  // standard control socket over which commands arrive to connect to some port for example
  std::vector<zmq_socket_ext*> zmq_sockets;
  static void* readCommandsHelper(void* object) {
    ((zmqconnectModule*)object)->readCommands();
    return NULL;
  }
  
  void readCommands();
protected:
  static const int channel_count = 0;
  const char* channel[0];
  // the standard zeromq context object
  zmq::context_t *context;
  // some default debug parameter
  char debug;
  /**
   * The resolve function can be called by modules to get a new socket (and if you want host name and port). It can also
   * be used by the connector, to bind to these previously set up sockets.
   */
  void Resolve(pns_record & record);
  void SendAck(zmq::socket_t *s, bool state);
  bool ReceiveAck(zmq::socket_t *s, bool & state, bool blocking);
  char* GetReply(zmq::socket_t *s, bool & state, bool blocking, int & reply_size);
  void SendRequest(zmq::socket_t *s, bool & state, bool blocking, std::string str);
  void HandleCommand();
  void Connect(std::string source, std::string target);
  zmq::socket_t* GetSocket(std::string name);

public:
  // Default constructor
  zmqconnectModule();
  
  // Default destructor
  virtual ~zmqconnectModule();
  
  // Extend this with your own code, first call zmqconnectModule::Init(name);
  void Init(std::string& name);
  
  // Function to get Param struct (to subsequently set CLI parameters)
  inline Param *GetParam() { return cliParam; }
  
  // Overwrite this function with your own code
  virtual void Tick() = 0;
  
  // Overwrite this function with your own code
  bool Stop() { return false; }
  
};
} // End of namespace

#endif // ZMQCONNECTMODULE_H_
