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
 * \file gen_staking_server.cpp
 * 
 * \brief Generates a multisig wallet in two parts
 */
#include <iostream>
#include <sstream>
#include <cstdio>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "include_base_utils.h"
#include "crypto/crypto.h"  // for crypto::secret_key definition
#include "common/i18n.h"
#include "common/command_line.h"
#include "common/util.h"
#include "common/scoped_message_writer.h"
#include "wallet/wallet_args.h"
#include "wallet/wallet2.h"

#include "misc.h"

using namespace std;
using namespace epee;
using namespace cryptonote;
using boost::lexical_cast;
namespace po = boost::program_options;

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.gen_staking_server"

namespace
{
  const command_line::arg_descriptor<std::string> arg_address = {"address", genms::tr("Address for the wallet"), ""};
  const command_line::arg_descriptor<std::string> arg_view_key = {"view-key", genms::tr("View key for the wallet"), ""};
  const command_line::arg_descriptor<std::string> arg_spend_key = {"spend-key", genms::tr("Spend key for the wallet"), ""};

  const command_line::arg_descriptor<bool, false> arg_trusted = {"trusted", genms::tr("Also generate client side information"), false};

  const command_line::arg_descriptor<std::string> arg_msig = {"msig", genms::tr("Multisig argument"), ""};

  const command_line::arg_descriptor< std::vector<std::string> > arg_command = {"command", ""};
}

class file_cleaner {
  const std::string file;
public:
  file_cleaner(const std::string &file) : file(file) {}
  ~file_cleaner() { unlink(file.c_str()); }
};

std::string wallet_info_json(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd) {
  wallet_keys keys = get_wallet_keys(wallet, pwd);
  // TODO: actual json output with rapidjson etc instead of this hack
  std::stringstream json;
  json << "{" << std::endl;
  json << "  \"address\": \"" << get_wallet_address(wallet) << "\"," << std::endl;
  json << "  \"seed\": \"" << get_wallet_seed(wallet, pwd) << "\"," << std::endl;
  json << "  \"view\": {" << std::endl;
  json << "    \"pub\": \"" << keys.view.pub << "\"," << std::endl;
  json << "    \"sec\": \"" << keys.view.sec << "\"" << std::endl;
  json << "  }," << std::endl;
  json << "  \"spend\": {" << std::endl;
  json << "    \"pub\": \"" << keys.spend.pub << "\"," << std::endl;
  json << "    \"sec\": \"" << keys.spend.sec << "\"" << std::endl;
  json << "  }," << std::endl;
  json << "  \"msig\": \"" << get_wallet_multisig_info(wallet, pwd) << "\"," << std::endl;
  json << "  \"height\": " << wallet->estimate_blockchain_height() << std::endl;
  json << "}";
  return json.str();
}

std::string multisig_info_json(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd) {
  wallet_keys keys = get_wallet_keys(wallet, pwd);
  // TODO: actual json output with rapidjson etc instead of this hack
  std::stringstream json;
  json << "{" << std::endl;
  json << "  \"address\": \"" << get_wallet_address(wallet) << "\"," << std::endl;
  json << "  \"seed\": \"" << get_wallet_seed(wallet, pwd) << "\"," << std::endl;
  json << "  \"view\": {" << std::endl;
  json << "    \"pub\": \"" << keys.view.pub << "\"," << std::endl;
  json << "    \"sec\": \"" << keys.view.sec << "\"" << std::endl;
  json << "  }" << std::endl;
  json << "}";
  return json.str();
}

std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

int main(int argc, char* argv[])
{
  try {
    po::options_description desc_params(wallet_args::tr("Wallet options"));
    command_line::add_arg(desc_params, arg_address);
    command_line::add_arg(desc_params, arg_view_key);
    command_line::add_arg(desc_params, arg_spend_key);
    command_line::add_arg(desc_params, arg_trusted);
    command_line::add_arg(desc_params, arg_msig);

    const char *file = std::tmpnam(nullptr); // FIXME: tmpnam is unsafe
    if (!file) throw std::runtime_error("Unable to get unique temporary file name");
    std::string log_file = std::string(file) + ".log";
    file_cleaner log_cleaner(log_file);

    boost::optional<po::variables_map> vm;
    bool should_terminate = false;
    std::tie(vm, should_terminate) = wallet_args::main(
      argc, argv,
      "bittube-gen-multisig-server",
      nullptr,
      desc_params,
      boost::program_options::positional_options_description(),
      [](const std::string &s, bool emphasis){ tools::scoped_message_writer(emphasis ? epee::console_color_white : epee::console_color_default, true) << s; },
      log_file.c_str(),
      false, true
    );
    if (!vm) throw std::runtime_error("Couldn't parse arguments");
    if (should_terminate) return 0;

    std::string address = command_line::get_arg(*vm, arg_address);
    std::string view_key = command_line::get_arg(*vm, arg_view_key);
    std::string spend_key = command_line::get_arg(*vm, arg_spend_key);
    bool trusted = command_line::get_arg(*vm, arg_trusted);
    std::string msig = command_line::get_arg(*vm, arg_msig);
    // TODO: sanity check arguments

    epee::wipeable_string pwd("");

    file_cleaner cache_cleaner(file);
    file_cleaner keys_cleaner(std::string(file) + ".keys");

    boost::shared_ptr<tools::wallet2> wallet(new tools::wallet2(MAINNET, 1, false));
    wallet->init("");
    wallet->set_seed_language("English");
    if (address.empty() || view_key.empty() || spend_key.empty()) {
      wallet->generate(file, pwd, rct::rct2sk(rct::skGen()), false, false, false);
    } else {
      restore_wallet_from_keys(wallet, file, pwd, address, view_key, spend_key);
    }

    // TODO: actual json output with rapidjson etc instead of this hack
    std::stringstream json;
    json << "{\n\"setup\": " << wallet_info_json(wallet, pwd);

    if (trusted) {
      std::string gen_client = std::tmpnam(nullptr); // FIXME: tmpnam is unsafe
      std::string gen_client_keys = gen_client + ".keys";

      std::string client_pwd_clear = random_string(10);
      epee::wipeable_string client_pwd(client_pwd_clear);

      file_cleaner client_cache_cleaner(gen_client);
      file_cleaner client_keys_cleaner(gen_client_keys);

      boost::shared_ptr<tools::wallet2> client_wallet(new tools::wallet2(MAINNET, 1, false));
      client_wallet->init("");
      client_wallet->set_seed_language("English");
      client_wallet->generate(gen_client, client_pwd, rct::rct2sk(rct::skGen()), false, false, false);

      json << ",\n\"client_setup\": " << wallet_info_json(client_wallet, client_pwd);

      msig = get_wallet_multisig_info(client_wallet, client_pwd); // store client msig for own finalization
      make_22_multisig(client_wallet, client_pwd, get_wallet_multisig_info(wallet, pwd));
      json << ",\n\"client_multisig\": " << multisig_info_json(client_wallet, client_pwd);

      client_wallet.reset(); // FIXME: client_wallet->store(); doesn't seem to generate the key file?

      // TODO: actual json output with rapidjson etc instead of this hack
      json << ",\n\"client_download\": {" << std::endl;
      json << "  \"password\": \"" << client_pwd_clear << "\"," << std::endl;
      json << "  \"file\": \"" << file_to_b64(gen_client_keys) << "\"" << std::endl;
      json << "}";
    }

    if (!msig.empty()) {
      make_22_multisig(wallet, pwd, msig);
      json << ",\n\"multisig\": " << multisig_info_json(wallet, pwd);
    }

    // TODO: actual json output with rapidjson etc instead of this hack
    json << std::endl << "}";
    std::cout << "{\"success\": true, \"result\": " << json.str() << "}" << std::endl;
    return 0;
  } catch (std::exception &e) {
    // TODO: actual json output with rapidjson etc instead of this hack
    std::cout << "{\"success\": false, \"error\": " << std::quoted(e.what()) << "}";
  } catch (...) {
    // TODO: actual json output with rapidjson etc instead of this hack
    std::cout << "{\"success\": false, \"error\": \"Unexpected error\"}";
  }
  return -1;
}
