#include "multiplier_LT.hh"
#include "testbench_LT.hh"

class TopModule : public sc_module {
  private:
    MultiplierModule m_target;
    TestbenchModule m_initiator;

  public:
    TopModule(const sc_module_name &name)
        : sc_module(name), m_target("target"), m_initiator("initiator") {
        m_initiator.initiator_socket(m_target.target_socket);
    }
};


int main(int argc, char *argv[]) {
    TopModule top("top");

    sc_start();

    return 0;
}