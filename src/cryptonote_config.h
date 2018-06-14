// Copyright (c) 2014-2018, The Monero Project
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

#pragma once

#include <string>
#include <boost/uuid/uuid.hpp>

#define CRYPTONOTE_DNS_TIMEOUT_MS                       20000

#define CRYPTONOTE_MAX_BLOCK_NUMBER                     500000000
#define CRYPTONOTE_MAX_BLOCK_SIZE                       500000000  // block header blob limit, never used!
#define CRYPTONOTE_GETBLOCKTEMPLATE_MAX_BLOCK_SIZE	196608 //size of block (bytes) that is the maximum that miners will produce
#define CRYPTONOTE_MAX_TX_SIZE                          1000000000
#define CRYPTONOTE_PUBLIC_ADDRESS_TEXTBLOB_VER          0
#define CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW            10
#define CURRENT_TRANSACTION_VERSION                     2 
#define CURRENT_BLOCK_MAJOR_VERSION                     1
#define CURRENT_BLOCK_MINOR_VERSION                     0
#define BLOCK_MAJOR_VERSION_1                           1
#define BLOCK_MAJOR_VERSION_2                           2
#define BLOCK_MAJOR_VERSION_3                           3
#define BLOCK_MAJOR_VERSION_4                           4
#define BLOCK_MAJOR_VERSION_FUTURE                      (uint8_t)(-1)
#define CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT              500
#define CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V2           60*10
#define CRYPTONOTE_DEFAULT_TX_SPENDABLE_AGE             10

#define BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW               60
#define BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW_V2            11

// MONEY_SUPPLY - total number coins to be generated
#define MONEY_SUPPLY                                    ((uint64_t)(100000000000000000))
#define EMISSION_SPEED_FACTOR_PER_MINUTE                (22)
#define FINAL_SUBSIDY_PER_MINUTE                        ((uint64_t)30000000) // 3 * pow(10, 7)

#define CRYPTONOTE_REWARD_BLOCKS_WINDOW                 100
#define CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2    1000000 //size of block (bytes) after which reward for block calculated using block size
#define CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1    100000 //size of block (bytes) after which reward for block calculated using block size - before first fork
#define CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V5    1000000 //size of block (bytes) after which reward for block calculated using block size - second change, from v5
#define CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE          600
#define CRYPTONOTE_DISPLAY_DECIMAL_POINT                8
// COIN - number of smallest units in one coin
#define COIN                                            ((uint64_t)100000000) // pow(10, 8)

#define LEGACY_MINIMUM_FEE								((uint64_t)100000)
#define FEE_PER_KB_OLD                                  ((uint64_t)100000) // pow(10, 5)
#define FEE_PER_KB                                      ((uint64_t)100000) // pow(10, 5)
#define DYNAMIC_FEE_PER_KB_BASE_FEE                     ((uint64_t)200000) // 2 * pow(10, 5)
#define DYNAMIC_FEE_PER_KB_BASE_BLOCK_REWARD            ((uint64_t)45000000000) // 450 * pow(10, 8)
#define DYNAMIC_FEE_PER_KB_BASE_FEE_V5                  ((uint64_t)200000 * (uint64_t)CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2 / CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V5)

#define ORPHANED_BLOCKS_MAX_COUNT                       100


#define DIFFICULTY_TARGET_V2                            120  // seconds
#define DIFFICULTY_TARGET_V1                            120  // seconds - before first fork
#define DIFFICULTY_WINDOW                               720 // blocks
#define DIFFICULTY_WINDOW_V2                            70
#define DIFFICULTY_LAG                                  15  // !!!
#define DIFFICULTY_CUT                                  60  // timestamps to cut after sorting
#define DIFFICULTY_BLOCKS_COUNT_V2                      (DIFFICULTY_WINDOW_V2 + 1) // added +1 to make N=N
#define DIFFICULTY_BLOCKS_COUNT                         DIFFICULTY_WINDOW + DIFFICULTY_LAG


#define CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS_V1   DIFFICULTY_TARGET_V1 * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS
#define CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS_V2   DIFFICULTY_TARGET_V2 * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS
#define CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS       1


#define DIFFICULTY_BLOCKS_ESTIMATE_TIMESPAN             DIFFICULTY_TARGET_V1 //just alias; used by tests


#define BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT          10000  //by default, blocks ids count in synchronizing
#define BLOCKS_SYNCHRONIZING_DEFAULT_COUNT_PRE_V4       100    //by default, blocks count in blocks downloading
#define BLOCKS_SYNCHRONIZING_DEFAULT_COUNT              20     //by default, blocks count in blocks downloading

#define CRYPTONOTE_MEMPOOL_TX_LIVETIME                    (86400*3) //seconds, three days
#define CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME     604800 //seconds, one week

#define COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT           1000

#define P2P_LOCAL_WHITE_PEERLIST_LIMIT                  1000
#define P2P_LOCAL_GRAY_PEERLIST_LIMIT                   5000

#define P2P_DEFAULT_CONNECTIONS_COUNT                   32
#define P2P_DEFAULT_HANDSHAKE_INTERVAL                  60           //secondes
#define P2P_DEFAULT_PACKET_MAX_SIZE                     50000000     //50000000 bytes maximum packet size
#define P2P_DEFAULT_PEERS_IN_HANDSHAKE                  250
#define P2P_DEFAULT_CONNECTION_TIMEOUT                  5000       //5 seconds
#define P2P_DEFAULT_PING_CONNECTION_TIMEOUT             2000       //2 seconds
#define P2P_DEFAULT_INVOKE_TIMEOUT                      60*2*1000  //2 minutes
#define P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT            5000       //5 seconds
#define P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT       70
#define P2P_DEFAULT_ANCHOR_CONNECTIONS_COUNT            2

#define P2P_FAILED_ADDR_FORGET_SECONDS                  (60*60)     //1 hour
#define P2P_IP_BLOCKTIME                                (60*60*24)  //24 hour
#define P2P_IP_FAILS_BEFORE_BLOCK                       10
#define P2P_IDLE_CONNECTION_KILL_INTERVAL               (5*60) //5 minutes

#define P2P_SUPPORT_FLAG_FLUFFY_BLOCKS                  0x01
#define P2P_SUPPORT_FLAGS                               P2P_SUPPORT_FLAG_FLUFFY_BLOCKS

#define ALLOW_DEBUG_COMMANDS

#define CRYPTONOTE_NAME                         "bittube"
#define CRYPTONOTE_POOLDATA_FILENAME            "poolstate.bin"
#define CRYPTONOTE_BLOCKCHAINDATA_FILENAME      "data.mdb"
#define CRYPTONOTE_BLOCKCHAINDATA_LOCK_FILENAME "lock.mdb"
#define P2P_NET_DATA_FILENAME                   "p2pstate.bin"
#define MINER_CONFIG_FILE_NAME                  "miner_conf.json"

#define THREAD_STACK_SIZE                       5 * 1024 * 1024

#define HF_VERSION_DYNAMIC_FEE                  4
#define HF_VERSION_DEV_REWARD                   4
#define HF_VERSION_AIRTIME_REWARD               5
#define HF_VERSION_MIN_MIXIN_2                  BLOCK_MAJOR_VERSION_FUTURE
#define HF_VERSION_MIN_MIXIN_4                  BLOCK_MAJOR_VERSION_FUTURE
#define HF_VERSION_MIN_MIXIN_6                  BLOCK_MAJOR_VERSION_FUTURE
#define HF_VERSION_ENFORCE_RCT                  4
#define HF_VERSION_BULLETPROOF                  BLOCK_MAJOR_VERSION_FUTURE

#define PER_KB_FEE_QUANTIZATION_DECIMALS        8

#define HASH_OF_HASHES_STEP                     256

#define DEFAULT_TXPOOL_MAX_SIZE                 648000000ull // 3 days at 300000, in bytes

// New constants are intended to go here
namespace config
{
  uint64_t const DEFAULT_FEE_ATOMIC_XMR_PER_KB = 500; // Just a placeholder!  Change me!
  uint8_t const FEE_CALCULATION_MAX_RETRIES = 10;
  uint64_t const DEFAULT_DUST_THRESHOLD = ((uint64_t)10000); // pow(10, 5)
  uint64_t const BASE_REWARD_CLAMP_THRESHOLD = ((uint64_t)200);
  std::string const P2P_REMOTE_DEBUG_TRUSTED_PUB_KEY = "0000000000000000000000000000000000000000000000000000000000000000";

  uint64_t const CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 0xd1;
  uint64_t const CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = 0x404f;
  uint64_t const CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = 0x3750;
  uint16_t const P2P_DEFAULT_PORT = 24181;
  uint16_t const RPC_DEFAULT_PORT = 24182;
  uint16_t const ZMQ_RPC_DEFAULT_PORT = 24183;
  boost::uuids::uuid const NETWORK_ID = { {
      0x78, 0x18, 0x3A, 0x89, 0x15, 0xA5, 0x92, 0xB4, 0x67, 0x12, 0x29, 0x49, 0x19, 0xA1, 0x41, 0x78
    } }; // Bender's nightmare
  std::string const GENESIS_TX = "010a01ff000180a094a58d1d029b2e4c0281c0b02e7c53291a94d1d0cbff8883f8024f5142ee494ffbbd08807121015bfdea2f96583074c2fbd4092d6cd7e5e618b8eee918f321099cf24d54c52afc";
  uint32_t const GENESIS_NONCE = 70;
  
  std::string const DEVELOPMENT_WALLET_ADDRESS = "bxdaNPkW77u6KYJuYNDSJpfocTXjVpZ7mMAsoNELySdnbAr8U6aMvnULosC456Kk7NRCAS2Xe7o14NF7bbPKyVta39KPYFia3";
  std::string const MARKETING_WALLET_ADDRESS = "bxdEc4zm1g2ZQZkSwjYThuMJouTSfzL2xJXTC2o4q7Dq17sEcsMdwgRfNuBgg59TPCLJwCj4jJH7rT8bdKjx5p6Z35LJDknWU";
  std::string const AIRTIME_WALLET_ADDRESS = "bxcfouydX4Q2jxw2Thx49bM7tmBiEhP7c8rUoXSVmYxxDpEqqSpBzdRe9WQDkg5LekBtX9W9XinTuG6ttKBtBnpT14KNegFD1"; // view key d6809b482464de15defbbb4127a94118c2ebf77997f45a496863a7266b1406a9

  namespace testnet
  {
    uint64_t const CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 0xd1;
    uint64_t const CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = 0x404f;
    uint64_t const CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = 0x3750;
    uint16_t const P2P_DEFAULT_PORT = 24181; // 23306;
    uint16_t const RPC_DEFAULT_PORT = 24182; // 23307;
    uint16_t const ZMQ_RPC_DEFAULT_PORT = 24183; // 23308;
    boost::uuids::uuid const NETWORK_ID = { {
        0x7c, 0x18, 0x3A, 0x89, 0x15, 0xA5, 0x92, 0xB4, 0x67, 0x12, 0x29, 0x49, 0x19, 0xA1, 0x41, 0x78
        // 0x2c ,0xa6, 0xf4, 0xa7 , 0x7d, 0x0c , 0x78, 0xdc, 0x44, 0x3f, 0xd5, 0x69, 0xcb, 0x6d, 0x4e, 0x1c
      } }; // Bender's daydream
    std::string const GENESIS_TX = "010a01ff000180a094a58d1d029b2e4c0281c0b02e7c53291a94d1d0cbff8883f8024f5142ee494ffbbd08807121015bfdea2f96583074c2fbd4092d6cd7e5e618b8eee918f321099cf24d54c52afc";
    uint32_t const GENESIS_NONCE = 74;
	
    std::string const DEVELOPMENT_WALLET_ADDRESS = "bxdaNPkW77u6KYJuYNDSJpfocTXjVpZ7mMAsoNELySdnbAr8U6aMvnULosC456Kk7NRCAS2Xe7o14NF7bbPKyVta39KPYFia3";
    std::string const MARKETING_WALLET_ADDRESS = "bxdEc4zm1g2ZQZkSwjYThuMJouTSfzL2xJXTC2o4q7Dq17sEcsMdwgRfNuBgg59TPCLJwCj4jJH7rT8bdKjx5p6Z35LJDknWU";
    std::string const AIRTIME_WALLET_ADDRESS = "bxcfouydX4Q2jxw2Thx49bM7tmBiEhP7c8rUoXSVmYxxDpEqqSpBzdRe9WQDkg5LekBtX9W9XinTuG6ttKBtBnpT14KNegFD1"; // view key d6809b482464de15defbbb4127a94118c2ebf77997f45a496863a7266b1406a9
  }

  namespace stagenet
  {
    uint64_t const CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 0xd1;
    uint64_t const CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = 0x404f;
    uint64_t const CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = 0x3750;
    uint16_t const P2P_DEFAULT_PORT = 33104;
    uint16_t const RPC_DEFAULT_PORT = 33105;
    uint16_t const ZMQ_RPC_DEFAULT_PORT = 33106;
    boost::uuids::uuid const NETWORK_ID = { {            
        0x67 ,0xe3, 0x11, 0x1b , 0x1e, 0x14 , 0x77, 0xf7, 0x34, 0x33, 0xa0, 0x33, 0xcc, 0xc5, 0x33, 0x77
      } }; // Bender's daydream
    std::string const GENESIS_TX = "010a01ff0001ec85afd1b101029b2e4c0281c0b02e7c53291a94d1d0cbff8883f8024f5142ee494ffbbd08807121011e06b561bed255e2cd1c03e4f7727284f27d05c31094f03e6997e6ed927c31a1";
    uint32_t const GENESIS_NONCE = 72;
	
    std::string const DEVELOPMENT_WALLET_ADDRESS = "bxdaNPkW77u6KYJuYNDSJpfocTXjVpZ7mMAsoNELySdnbAr8U6aMvnULosC456Kk7NRCAS2Xe7o14NF7bbPKyVta39KPYFia3";
    std::string const MARKETING_WALLET_ADDRESS = "bxdEc4zm1g2ZQZkSwjYThuMJouTSfzL2xJXTC2o4q7Dq17sEcsMdwgRfNuBgg59TPCLJwCj4jJH7rT8bdKjx5p6Z35LJDknWU";
    std::string const AIRTIME_WALLET_ADDRESS = "bxcfouydX4Q2jxw2Thx49bM7tmBiEhP7c8rUoXSVmYxxDpEqqSpBzdRe9WQDkg5LekBtX9W9XinTuG6ttKBtBnpT14KNegFD1"; // view key d6809b482464de15defbbb4127a94118c2ebf77997f45a496863a7266b1406a9
  }
}

namespace cryptonote
{
  enum network_type : uint8_t
  {
    MAINNET = 0,
    TESTNET,
    STAGENET,
    FAKECHAIN,
    UNDEFINED = 255
  };
}
