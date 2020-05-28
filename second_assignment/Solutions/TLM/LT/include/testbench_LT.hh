#ifndef TB_H
#define TB_H

#include "define_LT.hh"
#include <random>
#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>

class TestbenchModule : public sc_core::sc_module,
                        public virtual tlm::tlm_bw_transport_if<> {
  public:
    tlm::tlm_initiator_socket<> initiator_socket;

    SC_HAS_PROCESS(TestbenchModule);
    TestbenchModule(sc_module_name name);

  private:
    tlm_utils::tlm_quantumkeeper m_qk;

    virtual void invalidate_direct_mem_ptr(uint64 start_range,
                                           uint64 end_range);

    virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans,
                                               tlm::tlm_phase &phase,
                                               sc_time &t);
    void run();
};

#endif