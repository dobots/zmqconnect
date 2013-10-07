#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <zmqconnectModuleExt.h>

using namespace v8;
using namespace rur;

void RegisterModule(Handle<Object> exports) {
  zmqconnectModuleExt::NodeRegister(exports);
}

NODE_MODULE(zmqconnectModule, RegisterModule)
