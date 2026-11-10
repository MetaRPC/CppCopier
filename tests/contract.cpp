#include <copier/copier.hpp>
#include <cassert>
#include <iostream>
int main() {
  if(copier::StartReply{}.ok || copier::ListReply{}.ok || copier::SimpleReply{}.ok) return 1;
  for(const auto& key : {"", "TRIAL", "trial", "a b", "x\r\nInjected: yes"}) if(copier::valid_api_key(key)) return 2;
  if(copier::auth_headers("owner-test").find(L"apikey: owner-test\r\n")==std::wstring::npos) return 3;
  for(uint64_t value : {uint64_t(0),uint64_t(127),uint64_t(128),uint64_t(9007199254740993),UINT64_MAX}) {
    std::vector<uint8_t> bytes;copier::write_varint(bytes,value);
    const uint8_t* p=bytes.data();uint64_t decoded=0;
    if(!copier::read_varint(p,p+bytes.size(),decoded)||decoded!=value) return 4;
  }
  uint8_t invalid[10]={255,255,255,255,255,255,255,255,255,127};const uint8_t* p=invalid;uint64_t value;
  if(copier::read_varint(p,p+10,value))return 5;
  std::cout<<"PASS: false replies, authentication, key injection refusal and uint64 boundaries\n";
}
