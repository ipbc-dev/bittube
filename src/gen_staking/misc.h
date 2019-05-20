#ifndef __GEN_MULTISIG_MISC_H__
#define __GEN_MULTISIG_MISC_H__

#include "crypto/crypto.h"
#include "wallet/wallet2.h"

namespace genms {
  const char* tr(const char* str);
}

std::string input_line(const std::string& prompt, bool yesno = false);
epee::wipeable_string input_secure_line(const char *prompt);
std::string sc_to_hex(const crypto::secret_key &k);

struct key_pair {
  std::string sec;
  std::string pub;
};
struct wallet_keys {
  key_pair view;
  key_pair spend;
};

class file_cleaner {
  const std::string file;
public:
  file_cleaner(const std::string &file) : file(file) {}
  ~file_cleaner() { unlink(file.c_str()); }
};

std::string get_wallet_seed(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd);
std::string get_wallet_multisig_info(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd);
std::string get_wallet_address(boost::shared_ptr<tools::wallet2> &wallet);
wallet_keys get_wallet_keys(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd);

void print_wallet_info(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd);

void restore_wallet_from_keys(boost::shared_ptr<tools::wallet2> &wallet, const std::string &file, const epee::wipeable_string &pwd, const std::string &address, const std::string &view_key, const std::string &spend_key);
void make_22_multisig(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd, const std::string &msig);

std::string file_to_b64(const std::string &file);

std::string wallet_info_json(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd);
std::string multisig_info_json(boost::shared_ptr<tools::wallet2> &wallet, const epee::wipeable_string &pwd);
std::string random_string(size_t length);

#endif
