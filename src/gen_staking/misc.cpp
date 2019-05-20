#include "misc.h"

#include <iostream>
#include "include_base_utils.h"
#include "common/i18n.h"
#include "common/command_line.h"
#include "common/util.h"
#include "common/scoped_message_writer.h"
#include "string_coding.h"

using namespace std;
using namespace epee;
using namespace cryptonote;

namespace genms
{
  const char* tr(const char* str)
  {
    return i18n_translate(str, "tools::gen_multisig");
  }
}

std::string input_line(const std::string& prompt, bool yesno)
{
#ifdef HAVE_READLINE
  rdln::suspend_readline pause_readline;
#endif
  std::cout << prompt;
  if (yesno)
    std::cout << "  (Y/Yes/N/No)";
  std::cout << ": " << std::flush;

  std::string buf;
#ifdef _WIN32
  buf = tools::input_line_win();
#else
  std::getline(std::cin, buf);
#endif

  return epee::string_tools::trim(buf);
}

epee::wipeable_string input_secure_line(const char *prompt)
{
#ifdef HAVE_READLINE
  rdln::suspend_readline pause_readline;
#endif
  auto pwd_container = tools::password_container::prompt(false, prompt, false);
  if (!pwd_container)
  {
    MERROR("Failed to read secure line");
    return "";
  }

  epee::wipeable_string buf = pwd_container->password();

  buf.trim();
  return buf;
}

std::string sc_to_hex(const crypto::secret_key &k)
{
  static constexpr const char hex[] = u8"0123456789abcdef";
  const uint8_t *ptr = (const uint8_t*)k.data;
  std::string str(sizeof(k) * 2, '0');
  for (size_t i = 0, sz = sizeof(k); i < sz; ++i)
  {
    str[i * 2 + 0] = hex[*ptr >> 4];
    str[i * 2 + 1] = hex[*ptr & 15];
    ++ptr;
  }
  return str;
}

std::string get_wallet_seed(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd)
{
  epee::wipeable_string seed, seed_pass("");

  bool ready = false, success = false, multisig = wallet->multisig(&ready);
  if (multisig && !ready) throw std::runtime_error("Unable to get seed, wallet is MultiSig but not ready");

  { // scope for unlocker
    tools::wallet_keys_unlocker unlocker(*wallet, true, pwd);
    if (multisig) success = wallet->get_multisig_seed(seed, seed_pass);
    else if (wallet->is_deterministic()) success = wallet->get_seed(seed, seed_pass);
    else throw std::runtime_error("Unable to get seed, wallet is not multisig nor deterministic?");
  }

  if (!success) throw std::runtime_error("Call to get_wallet_seed/get_multisig_seed returned false");

  size_t len = seed.size();
  std::vector<char> buf(len);
  memcpy(buf.data(), seed.data(), len);
  return std::string(buf.begin(), buf.end());
}

std::string get_wallet_multisig_info(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd) {
  crypto::secret_key sk;
  crypto::public_key pk;
  tools::wallet_keys_unlocker unlocker(*wallet, true, pwd);
  std::string msig = wallet->get_multisig_info();
  if (!tools::wallet2::verify_multisig_info(msig, sk, pk)) throw std::runtime_error("Failed to verify multisig info");
  return msig;
}

void restore_wallet_from_keys(boost::shared_ptr<tools::wallet2> &wallet, const std::string &file, const epee::wipeable_string &pwd, const std::string &address, const std::string &view_key, const std::string &spend_key) {
  cryptonote::address_parse_info info;
  if (!cryptonote::get_account_address_from_str(info, MAINNET, address)) throw std::runtime_error("Failed to parse address");
  if (info.is_subaddress) throw std::runtime_error("Trying to restore subaddress?");

  crypto::secret_key sk;
  if (!string_tools::hex_to_pod(spend_key, sk)) throw std::runtime_error("Failed to parse spend key");
  crypto::secret_key vk;
  if (!string_tools::hex_to_pod(view_key, vk)) throw std::runtime_error("Failed to parse view key");

  crypto::public_key pkey;
  if (!crypto::secret_key_to_public_key(sk, pkey)) throw std::runtime_error("Failed to verify spend key");
  if (info.address.m_spend_public_key != pkey) throw std::runtime_error("Spend key doesn't match address");
  if (!crypto::secret_key_to_public_key(vk, pkey)) throw std::runtime_error("Failed to verify view key");
  if (info.address.m_view_public_key != pkey) throw std::runtime_error("View key doesn't match address");

  wallet->generate(file, pwd, info.address, sk, vk, false);
}

std::string get_wallet_address(boost::shared_ptr<tools::wallet2> &wallet) {
  return wallet->get_account().get_public_address_str(MAINNET);
}

wallet_keys get_wallet_keys(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd) {
  tools::wallet_keys_unlocker unlocker(*wallet, true, pwd);
  const cryptonote::account_keys &keys = wallet->get_account().get_keys();
  key_pair view {sc_to_hex(keys.m_view_secret_key), string_tools::pod_to_hex(keys.m_account_address.m_view_public_key)};
  key_pair spend {sc_to_hex(keys.m_spend_secret_key), string_tools::pod_to_hex(keys.m_account_address.m_spend_public_key)};
  return wallet_keys {view, spend};
}

// TODO: change to return wallet info struct
void print_wallet_info(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd) {
  const std::string &seed = get_wallet_seed(wallet, pwd);
  tools::msg_writer() << "seed: " << seed;

  tools::msg_writer() << "address: " << get_wallet_address(wallet);

  const wallet_keys &keys = get_wallet_keys(wallet, pwd);
  tools::msg_writer() << "view keys";
  tools::msg_writer() << "secret: " << keys.view.sec;
  tools::msg_writer() << "public: " << keys.view.pub;

  tools::msg_writer() << "spend keys";
  tools::msg_writer() << "secret: " << keys.spend.sec;
  tools::msg_writer() << "public: " << keys.spend.pub;
}

void make_22_multisig(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd, const std::string &msig) {
  uint32_t threshold = 2, total = 2;
  std::string multisig_extra_info = wallet->make_multisig(pwd, {msig}, threshold);
  if (!multisig_extra_info.empty()) throw std::runtime_error("Failed to make multisig in one step");

  if (!wallet->multisig(NULL, &threshold, &total)) throw std::runtime_error("Wallet is not multisig");
  if (threshold != 2 || total != 2) throw std::runtime_error("Wallet is not 2/2 multisig");
}

std::string file_to_b64(const std::string &file) {
  std::ifstream in(file, std::ios_base::binary);
  if (!in.good()) throw std::runtime_error("Unable to open file for reading");
  in.seekg(0, std::ios_base::end);
  const std::streampos len = in.tellg();
  in.seekg(0, std::ios_base::beg);
  std::vector<char> buf(len);
  in.read(buf.data(), len);
  if (in.gcount() != len) throw std::runtime_error("Unable to read the whole file");
  return epee::string_encoding::base64_encode(std::string(buf.begin(), buf.end()));
}

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
  std::srand(std::time(0));
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
