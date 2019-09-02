#include "config/errorcode.h"
#include "wasm/wasmnativecontract.hpp"
#include "wasm/exceptions.hpp"
#include "wasm/datastream.hpp"
#include "persistence/cachewrapper.h"

using namespace std;
using namespace wasm;

namespace wasm {

    void WasmNativeSetcode( CWasmContext &context ) {

        CAccount sender;
        WASM_ASSERT(context.cache.accountCache.GetAccount(context.control_trx.txUid, sender), UPDATE_ACCOUNT_FAIL,
                    "bad-read-accountdb",
                    "wasmnativecontract.Setcode, sender account does not exist, sender Id = %s",
                    context.control_trx.txUid.ToString().data())


        WASM_ASSERT(sender.OperateBalance(SYMB::WICC, BalanceOpType::SUB_FREE, context.control_trx.llFees),
                    UPDATE_ACCOUNT_FAIL, "operate-account-failed",
                    "wasmnativecontract.Setcode, operate account failed ,regId=%s",
                    context.control_trx.txUid.ToString().data())


        WASM_ASSERT(context.cache.accountCache.SetAccount(CUserID(sender.keyid), sender), UPDATE_ACCOUNT_FAIL,
                    "bad-save-accountdb",
                    "wasmnativecontract.Setcode, save account info error")

         using SetCode = std::tuple<uint64_t, string, string, string>;
         SetCode setcode = wasm::unpack<SetCode>(context.trx.data);

         auto nick_name = std::get<0>(setcode);
         auto code  = std::get<1>(setcode);
         auto abi   = std::get<2>(setcode);
         auto memo   = std::get<3>(setcode);

        CAccount account;
        CRegID contractRegId = wasm::Name2RegID(nick_name);
        // if (!context.cache.accountCache.GetAccount(contractRegId, account)){
        //   return;
        // }

        CUniversalContract contract;
        context.cache.contractCache.GetContract(contractRegId, contract);

        if (contract.vm_type != VMType::WASM_VM) contract.vm_type = VMType::WASM_VM;

        if (code.size() > 0) contract.code = code;
        if (abi.size() > 0) contract.abi = abi;
        if (memo.size() > 0) contract.memo = memo;

        WASM_ASSERT(context.cache.contractCache.SaveContract(contractRegId, contract), UPDATE_ACCOUNT_FAIL,
                    "bad-save-scriptdb",
                    "wasmnativecontract.Setcode, save account info error")

        // CUniversalContract contractTest;
        // context.cache.contractCache.GetContract(contractRegId, contractTest);

        std::cout << "WasmNativeSetcode ----------------------------"
                  << "  contract:" << nick_name
                  << " contractRegId:" << contractRegId.ToString()
                  //<< " abi:"<< contractTest.abi
                  // << " data:"<< params[3].get_str()
                  << " \n";

        //context.control_trx.nRunStep = contract.GetContractSize();
        //if (!SaveTxAddresses(height, index, cw, state, {txUid})) return false;
    }

}