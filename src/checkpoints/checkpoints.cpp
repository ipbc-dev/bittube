// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The BitTube Project
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

#include "include_base_utils.h"

using namespace epee;

#include "checkpoints.h"

#include "common/dns_utils.h"
#include "include_base_utils.h"
#include "string_tools.h"
#include "storages/portable_storage_template_helper.h" // epee json include
#include "serialization/keyvalue_serialization.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "checkpoints"

namespace cryptonote
{
  /**
   * @brief struct for loading a checkpoint from json
   */
  struct t_hashline
  {
    uint64_t height; //!< the height of the checkpoint
    std::string hash; //!< the hash for the checkpoint
        BEGIN_KV_SERIALIZE_MAP()
          KV_SERIALIZE(height)
          KV_SERIALIZE(hash)
        END_KV_SERIALIZE_MAP()
  };

  /**
   * @brief struct for loading many checkpoints from json
   */
  struct t_hash_json {
    std::vector<t_hashline> hashlines; //!< the checkpoint lines from the file
        BEGIN_KV_SERIALIZE_MAP()
          KV_SERIALIZE(hashlines)
        END_KV_SERIALIZE_MAP()
  };

  //---------------------------------------------------------------------------
  checkpoints::checkpoints()
  {
  }
  //---------------------------------------------------------------------------
  bool checkpoints::add_checkpoint(uint64_t height, const std::string& hash_str)
  {
    crypto::hash h = crypto::null_hash;
    bool r = epee::string_tools::parse_tpod_from_hex_string(hash_str, h);
    CHECK_AND_ASSERT_MES(r, false, "Failed to parse checkpoint hash string into binary representation!");

    // return false if adding at a height we already have AND the hash is different
    if (m_points.count(height))
    {
      CHECK_AND_ASSERT_MES(h == m_points[height], false, "Checkpoint at given height already exists, and hash for new checkpoint was different!");
    }
    m_points[height] = h;
    return true;
  }
  //---------------------------------------------------------------------------
  bool checkpoints::is_in_checkpoint_zone(uint64_t height) const
  {
    return !m_points.empty() && (height <= (--m_points.end())->first);
  }
  //---------------------------------------------------------------------------
  bool checkpoints::check_block(uint64_t height, const crypto::hash& h, bool& is_a_checkpoint) const
  {
    auto it = m_points.find(height);
    is_a_checkpoint = it != m_points.end();
    if(!is_a_checkpoint)
      return true;

    if(it->second == h)
    {
      MINFO("CHECKPOINT PASSED FOR HEIGHT " << height << " " << h);
      return true;
    }else
    {
      MWARNING("CHECKPOINT FAILED FOR HEIGHT " << height << ". EXPECTED HASH: " << it->second << ", FETCHED HASH: " << h);
      return false;
    }
  }
  //---------------------------------------------------------------------------
  bool checkpoints::check_block(uint64_t height, const crypto::hash& h) const
  {
    bool ignored;
    return check_block(height, h, ignored);
  }
  //---------------------------------------------------------------------------
  //FIXME: is this the desired behavior?
  bool checkpoints::is_alternative_block_allowed(uint64_t blockchain_height, uint64_t block_height) const
  {
    if (0 == block_height)
      return false;

    auto it = m_points.upper_bound(blockchain_height);
    // Is blockchain_height before the first checkpoint?
    if (it == m_points.begin())
      return true;

    --it;
    uint64_t checkpoint_height = it->first;
    return checkpoint_height < block_height;
  }
  //---------------------------------------------------------------------------
  uint64_t checkpoints::get_max_height() const
  {
    std::map< uint64_t, crypto::hash >::const_iterator highest = 
        std::max_element( m_points.begin(), m_points.end(),
                         ( boost::bind(&std::map< uint64_t, crypto::hash >::value_type::first, _1) < 
                           boost::bind(&std::map< uint64_t, crypto::hash >::value_type::first, _2 ) ) );
    return highest->first;
  }
  //---------------------------------------------------------------------------
  const std::map<uint64_t, crypto::hash>& checkpoints::get_points() const
  {
    return m_points;
  }

  bool checkpoints::check_for_conflicts(const checkpoints& other) const
  {
    for (auto& pt : other.get_points())
    {
      if (m_points.count(pt.first))
      {
        CHECK_AND_ASSERT_MES(pt.second == m_points.at(pt.first), false, "Checkpoint at given height already exists, and hash for new checkpoint was different!");
      }
    }
    return true;
  }

  bool checkpoints::init_default_checkpoints(network_type nettype)
  {
    if (nettype == TESTNET)
    {
      return true;
    }
    if (nettype == STAGENET)
    {
      return true;
    }
  ADD_CHECKPOINT(50,   "dbfc2fce97b6c95bb130cb88c3e06a3020b144719f6660fb5d8a2b2bc847318f");
  ADD_CHECKPOINT(200,  "3b0d7cecca432c41ef84b6ab930e859274c117c0319e34c36fbc858f7fc6255c");
  ADD_CHECKPOINT(1000, "ab459e59b90d38f2cf370835dd5558bfd79f26f5034a58128e6aded3c611c594");
  ADD_CHECKPOINT(1500, "3c57b653c48f1ad113b7053aea628897b636ad4f366f7ee1d3f7aae018c07549");
  ADD_CHECKPOINT(1600, "3b5bc6181690ae31f0fae0426c02d6541af4a7d2dcc12e5c0ac53f81616e8dc1");
  ADD_CHECKPOINT(2000, "cc9537cae496f1dfed600af2238d617cf0c7c65801ad5672ec04580c4b2438ce");
  ADD_CHECKPOINT(5500, "53c1af87ee0d0607770e5911c0d755b882bc210bfaaf8d50091dfa32d17ac380");
  ADD_CHECKPOINT(5515, "dcdcd66f456c9a33510d473f6a1bc293225e597e0c0ec815a1c54f72f8aa8aef");
  ADD_CHECKPOINT(6166, "eedf1491ea2237082749e92b4c6dd3c11cfb7ef6cf52fea8c8620fc01a72e591");
  ADD_CHECKPOINT(7000, "62dafd54e4ef7e572b3b89617007a80194a224e12396e4291447ccecb5ca598c");
  ADD_CHECKPOINT(8000, "bd1b9b3a2e98e375fcfbb5358033adc448d347b5657a11c1c29eb7375a07f0b2");
  ADD_CHECKPOINT(9000, "b25d9800a515eabab17e5ab9fcbec38797cf61c7406be34795f607beccc14c26");
  ADD_CHECKPOINT(10000, "554e4fe4d7c60ac17ba4880713137973dc65a04199bdd91b363ba0a36d4b92c3");
  ADD_CHECKPOINT(11000, "e5e4ccd719fe7f29f6e85e3a965fe33bd165fe950a3c9a9c48830c25578d85b8");
  ADD_CHECKPOINT(12000, "0ecc1f453b184d7db6d3e3ac5ef5d41763d7e49d807420a768593a0ed0e466f5");
  ADD_CHECKPOINT(13000, "82c47518a8ced16e408958636bfde75c22703c5b624e36692466e2712818bd41"); 
  ADD_CHECKPOINT(14000, "c4ec39612d4a35b7e528b42ccc5575f021203874656c148fc36c62a46d65067d");
  ADD_CHECKPOINT(15000, "726fc5cfc1e48740b2c77575393fdc7db299a47cc9d90d40d088c22fe3b2d129");
  ADD_CHECKPOINT(16000, "af062ec6f9312e78f1f4824ac7059c6f58553684657c5e9cf5405dba820beaab");
  ADD_CHECKPOINT(17000, "95cac6667df0f6e4068c9f6551198de51c7ce6c886170334a47ebb4b440633c5");
  ADD_CHECKPOINT(18000, "0ce592e6d0059f7661193d1439d90210db4809ca6a371c2f19f694fab30d171b");
  ADD_CHECKPOINT(19000, "1b7be4a99458c1b1d75e5adbb05dc399b234f88dd0e0ba1ba99d6f52342a26bd");
  ADD_CHECKPOINT(20000, "1c46b80d216747c496f0df31c92c8fc25f29eae3e18c89fdb5d9de4c01de745b");
  ADD_CHECKPOINT(22998, "346d722eac20b8d12071260fa93e1948db73a3e2cfb2fa93aef4b56b513f2e63");
  ADD_CHECKPOINT(22999, "51f35ff5ff792234ab53ce9a04f2fcac0bb86fa826b103e4d7718e14cffbc85d");
  ADD_CHECKPOINT(23000, "7aef3f43d2acd3710b496b9248771a0623af64df75cd1be432efdd4c709dfa0e");
  ADD_CHECKPOINT(23001, "8cff91aa6096759c57da488c84a8e623897baaf85dc6be0014f0055cdebb9149");
  ADD_CHECKPOINT(30000, "e9fec8a991ecef5691223fc6474e7f23a95bf49d0327de6d946b342e2a7884b3");
  ADD_CHECKPOINT(40000, "ee18a481e192d69a7edaeec7c25c30208309d88b5d54e68487af105aa309fa08");
  ADD_CHECKPOINT(50000, "66d5478805fd31dc558c9d320d4a9946f46329a1fa439fcd520677572b55acf0");
  ADD_CHECKPOINT(60000, "b213931d63096025b8b814594e13c3e9c754ac4568a3f3674c58b35058141780");
  ADD_CHECKPOINT(70000, "28d73851e71962489d384c441fc781d90c4d8a10720123061450d3ff24d846fc");
  ADD_CHECKPOINT(80000, "20acacf0b33dbe41c09d63dece4959374c2785fb71380546c8b3f46c5163d6e4");
  ADD_CHECKPOINT(81000, "b09dfd90c6dca5165a89505333c457f13dba1ca30ec4ee24ed83041830b44c45");
  ADD_CHECKPOINT(90000, "1d85cfd5fd39e85730796d559c024f4798932e1a4e7db7f022dac4a9c868107f");
  ADD_CHECKPOINT(95765, "48f800d10098635d33db725855295af9c241d50e34ff54425f496b4d412da3f5");
  ADD_CHECKPOINT(100000, "745376ecd62a00ef7a299b9b9b82d7eb69b13ac7780075e720fd883ddac3a161");
  ADD_CHECKPOINT(105000, "2a5c6281281715bec10b3e432f76969655b65bcd4c9f3a4450aaa7552bdc0270");
  ADD_CHECKPOINT(110000, "7b1294ae06c8ed07d9a5bc456a92df97b8aec4bb62af8f9937355c6ea11ea3a1");
  ADD_CHECKPOINT(115000, "6fdb77269a4ebc4c0d2eacef335c02caf376c8cb44b6a834954c7386b3273ba2");
  ADD_CHECKPOINT(120000, "0c956f9e23ad7184fe143300df04cb8f8843109c323d4e41370b61f5103b2a5a");
  ADD_CHECKPOINT(125000, "e21726bd3b531b39047fc77b3783addcf8042a4cc03d6b97a19fc67a165096e9");
  ADD_CHECKPOINT(130000, "5f486a2cbace959c9a35351d54562f517339544a8643b450eec3b0ca2b9302b2");
  ADD_CHECKPOINT(135000, "a8035e5824afa97373e3b469dc630df0fc20630480365914e7a68ccaebdb5d22");
  ADD_CHECKPOINT(140000, "9605733d2b2f1cfeb69ebc25fa823e8e5c3b25b0d920b720cabe0a271a37ae15");
  ADD_CHECKPOINT(145000, "78ee5be612f55e47720ccbfc377336e3899eba2fc9b829e5d85c59f42f5f6dd6");
  ADD_CHECKPOINT(150000, "aa34aaac216c88ff424b89012f3acacf80c15c9f53573eb5b6aab4eb82ad54c8");
  ADD_CHECKPOINT(155000, "808425d59a8fdaa95cc00e430ddf3d2fa1ff96b965e2727fbcd6d4b7e0985b87");
  ADD_CHECKPOINT(160000, "ebd7efe332c96941b70e68038c8996a3852e6edfcdcf659c0bff7383bb2af1df");
  ADD_CHECKPOINT(165000, "dc632559aa6daec0aa7fa07153c2dffcab8e78630239234a87c3e3cb5b444f5f");
  ADD_CHECKPOINT(168325, "cf297c55da5461595bb7ae26c00d6275c25035326cd8f8f9a01fe7e8dc10a236");
    return true;
  }

  bool checkpoints::load_checkpoints_from_json(const std::string &json_hashfile_fullpath)
  {
    boost::system::error_code errcode;
    if (! (boost::filesystem::exists(json_hashfile_fullpath, errcode)))
    {
      LOG_PRINT_L1("Blockchain checkpoints file not found");
      return true;
    }

    LOG_PRINT_L1("Adding checkpoints from blockchain hashfile");

    uint64_t prev_max_height = get_max_height();
    LOG_PRINT_L1("Hard-coded max checkpoint height is " << prev_max_height);
    t_hash_json hashes;
    if (!epee::serialization::load_t_from_json_file(hashes, json_hashfile_fullpath))
    {
      MERROR("Error loading checkpoints from " << json_hashfile_fullpath);
      return false;
    }
    for (std::vector<t_hashline>::const_iterator it = hashes.hashlines.begin(); it != hashes.hashlines.end(); )
    {
      uint64_t height;
      height = it->height;
      if (height <= prev_max_height) {
	LOG_PRINT_L1("ignoring checkpoint height " << height);
      } else {
	std::string blockhash = it->hash;
	LOG_PRINT_L1("Adding checkpoint height " << height << ", hash=" << blockhash);
	ADD_CHECKPOINT(height, blockhash);
      }
      ++it;
    }

    return true;
  }

  bool checkpoints::load_checkpoints_from_dns(network_type nettype)
  {
    std::vector<std::string> records;

    // All four BitTubePulse domains have DNSSEC on and valid
    static const std::vector<std::string> dns_urls = { 
	
    };

    static const std::vector<std::string> testnet_dns_urls = { 
	
    };

    static const std::vector<std::string> stagenet_dns_urls = { 
	
    };

    if (!tools::dns_utils::load_txt_records_from_dns(records, nettype == TESTNET ? testnet_dns_urls : nettype == STAGENET ? stagenet_dns_urls : dns_urls))
      return true; // why true ?

    for (const auto& record : records)
    {
      auto pos = record.find(":");
      if (pos != std::string::npos)
      {
        uint64_t height;
        crypto::hash hash;

        // parse the first part as uint64_t,
        // if this fails move on to the next record
        std::stringstream ss(record.substr(0, pos));
        if (!(ss >> height))
        {
    continue;
        }

        // parse the second part as crypto::hash,
        // if this fails move on to the next record
        std::string hashStr = record.substr(pos + 1);
        if (!epee::string_tools::parse_tpod_from_hex_string(hashStr, hash))
        {
    continue;
        }

        ADD_CHECKPOINT(height, hashStr);
      }
    }
    return true;
  }

  bool checkpoints::load_new_checkpoints(const std::string &json_hashfile_fullpath, network_type nettype, bool dns)
  {
    bool result;

    result = load_checkpoints_from_json(json_hashfile_fullpath);
    if (dns)
    {
      result &= load_checkpoints_from_dns(nettype);
    }

    return result;
  }
}
