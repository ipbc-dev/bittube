// Copyright (c) 2018-2019, The BitTube Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

/*!
 * \file geb_staking_client.cpp
 * 
 * \brief A simple generator for a 2/2 multisig wallet that doesn't require specific knowledge
 */
#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/i18n.h"
#include "common/command_line.h"
#include "common/util.h"
#include "common/scoped_message_writer.h"
#include "wallet/wallet_args.h"

#include "misc.h"

using namespace std;
using namespace epee;
using namespace cryptonote;
using boost::lexical_cast;
namespace po = boost::program_options;

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.gen_staking_client"

namespace
{
  /*
  const command_line::arg_descriptor<bool, false> arg_restore_from_keys = {"restore-from-keys", genms::tr("Restore wallet from keys"), false};
  const command_line::arg_descriptor<bool, false> arg_restore_from_seed = {"restore-from-seed", genms::tr("Restore wallet from seed"), false};
  */

  const command_line::arg_descriptor< std::vector<std::string> > arg_command = {"command", ""};
}

int main(int argc, char* argv[])
{
  TRY_ENTRY();

  po::options_description desc_params(wallet_args::tr("Wallet options"));
  /*
  command_line::add_arg(desc_params, arg_restore_from_keys);
  command_line::add_arg(desc_params, arg_restore_from_seed);
  */

  boost::optional<po::variables_map> vm;
  bool should_terminate = false;
  std::tie(vm, should_terminate) = wallet_args::main(
    argc, argv,
    "bittube-gen-multisig-simple",
    genms::tr("This program generates a new 2/2 multisig wallet if you already have the reply"),
    desc_params,
    boost::program_options::positional_options_description(),
    [](const std::string &s, bool emphasis){ tools::scoped_message_writer(emphasis ? epee::console_color_white : epee::console_color_default, true) << s; },
    "bittube-gen-multisig-simple.log"
  );
  if (!vm)
    return 1;
  if (should_terminate)
    return 0;

  /*
  bool restoring_from_keys = command_line::get_arg(*vm, arg_restore_from_keys);
  bool restoring_from_seed = command_line::get_arg(*vm, arg_restore_from_seed);
  */
  
  try {
    std::string file = input_line("Enter name for new multisig wallet");
    const auto pwd_container = tools::password_container::prompt(true, "Enter password for new multisig wallet");
    const epee::wipeable_string &pwd = pwd_container->password();

    boost::shared_ptr<tools::wallet2> wallet(new tools::wallet2(MAINNET, 1, false));
    wallet->init("");
    wallet->set_seed_language("English");

    // TODO: restore-from-[keys|seed]-and-msig for a ground up multisig restore?
    wallet->generate(file, pwd, rct::rct2sk(rct::skGen()), false, false, false);

    std::string msig = input_line("Enter multisig");
    std::string seed = get_wallet_seed(wallet, pwd);
    std::string reply = get_wallet_multisig_info(wallet, pwd);

    make_22_multisig(wallet, pwd, msig);

    tools::success_msg_writer() << "Created new 2/2 multisig wallet: " << get_wallet_address(wallet) << std::endl;

    tools::success_msg_writer() << "Send the following multisig message to the initiator:";
    tools::msg_writer() << "===========================================================";
    tools::msg_writer() << reply;
    tools::msg_writer() << "===========================================================" << std::endl;

    tools::success_msg_writer() << "NOTE: the following information can be used to recover access to your wallet. Write it down and store it somewhere safe and secure. Please do not store them in your email or on file storage services outside of your immediate control." << std::endl;

    tools::success_msg_writer() << "Here's the base wallet seed and multisig for restoring the base wallet and redoing multisig setup from ground up.";
    tools::msg_writer() << "===========================================================";
    tools::msg_writer() << seed << std::endl;
    tools::msg_writer() << msig;
    tools::msg_writer() << "===========================================================" << std::endl;

    tools::success_msg_writer() << "You can also restore the multisig wallet directly by using the following seed with --restore-multisig-wallet.";
    tools::msg_writer() << "===========================================================";
    tools::msg_writer() << get_wallet_seed(wallet, pwd);
    tools::msg_writer() << "===========================================================" << std::endl;

    input_line("Press enter to exit");

    return 0;
  } catch (std::exception &e) {
    tools::fail_msg_writer() << "Error: " << e.what() << std::endl;
  } catch (...) {
    tools::fail_msg_writer() << "Unexpected error" << std::endl;
  }

  return -1;
  CATCH_ENTRY_L0("main", 1);
}
