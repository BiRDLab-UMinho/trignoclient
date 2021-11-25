#include "interface.hpp"   // trigno::network::Interface
#include "basic_configurator.hpp"

namespace trigno::network {

BasicConfigurator::BasicConfigurator(Interface* network) :
    _network(network) {
        /* .. */
        // reset();  // @note should never call virtual methods on base constructor!!!
}



BasicConfigurator::~BasicConfigurator() {
    /* .. */
}



void BasicConfigurator::attachTo(Interface* network) {
    /* .. */
    _network = network;
}

}  // namespace trigno::netowk