#ifndef MULT_H
#define MULT_H

#include "define_UT.hh"
#include <systemc.h>
#include <tlm.h>

class MultiplierModule : public sc_core::sc_module,
                         public virtual tlm::tlm_fw_transport_if<> {
  public:
    tlm::tlm_target_socket<> target_socket;

    SC_HAS_PROCESS(MultiplierModule);
    MultiplierModule(const sc_module_name &module_name);

  private:
    iostruct ioDataStruct;
    tlm::tlm_generic_payload *pending_transaction;
    sc_lv<32> tmp_result;
    float tmp_op1, tmp_op2;

    virtual void b_transport(tlm::tlm_generic_payload &trans, sc_time &t);

    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans,
                                    tlm::tlm_dmi &dmi_data);

    virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload
    &trans,
                                               tlm::tlm_phase &phase,
                                               sc_time &t);

    virtual unsigned int transport_dbg(tlm::tlm_generic_payload &trans);

    void multiplier_function();
    void end_of_elaboration();
    void reset();
};


#endif