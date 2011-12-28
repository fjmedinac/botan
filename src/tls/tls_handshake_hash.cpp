/*
* TLS Handshake Hash
* (C) 2004-2006,2011 Jack Lloyd
*
* Released under the terms of the Botan license
*/

#include <botan/internal/tls_handshake_hash.h>
#include <botan/md5.h>
#include <botan/sha160.h>
#include <memory>

namespace Botan {

void TLS_Handshake_Hash::update(Handshake_Type handshake_type,
                                const MemoryRegion<byte>& handshake_msg)
   {
   update(static_cast<byte>(handshake_type));

   const size_t record_length = handshake_msg.size();
   for(size_t i = 0; i != 3; i++)
      update(get_byte<u32bit>(i+1, record_length));

   update(handshake_msg);
   }

/**
* Return a TLS Handshake Hash
*/
SecureVector<byte> TLS_Handshake_Hash::final()
   {
   MD5 md5;
   SHA_160 sha1;

   md5.update(data);
   sha1.update(data);

   SecureVector<byte> output;
   output += md5.final();
   output += sha1.final();
   return output;
   }

/**
* Return a SSLv3 Handshake Hash
*/
SecureVector<byte> TLS_Handshake_Hash::final_ssl3(const MemoryRegion<byte>& secret)
   {
   const byte PAD_INNER = 0x36, PAD_OUTER = 0x5C;

   MD5 md5;
   SHA_160 sha1;

   md5.update(data);
   sha1.update(data);

   md5.update(secret);
   sha1.update(secret);

   for(size_t i = 0; i != 48; ++i)
      md5.update(PAD_INNER);
   for(size_t i = 0; i != 40; ++i)
      sha1.update(PAD_INNER);

   SecureVector<byte> inner_md5 = md5.final(), inner_sha1 = sha1.final();

   md5.update(secret);
   sha1.update(secret);
   for(size_t i = 0; i != 48; ++i)
      md5.update(PAD_OUTER);
   for(size_t i = 0; i != 40; ++i)
      sha1.update(PAD_OUTER);
   md5.update(inner_md5);
   sha1.update(inner_sha1);

   SecureVector<byte> output;
   output += md5.final();
   output += sha1.final();
   return output;
   }

}
