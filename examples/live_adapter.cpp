#include <copier/copier.hpp>
#include <iomanip>
#include <sstream>
static std::string q(const std::string& s){std::ostringstream o;o<<'"';for(unsigned char c:s){switch(c){case '"':o<<"\\\"";break;case '\\':o<<"\\\\";break;case '\n':o<<"\\n";break;case '\r':o<<"\\r";break;case '\t':o<<"\\t";break;default:if(c<32)o<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<int(c);else o<<c;}}o<<'"';return o.str();}
int main(int argc,char** argv){try{
 std::string v[13];for(auto& s:v){if(!std::getline(std::cin,s))throw std::runtime_error("Expected 13 configuration lines");if(!s.empty()&&s.back()=='\r')s.pop_back();}
 if(argc<2)throw std::runtime_error("Operation required");copier::CopierService c(v[0],v[1]);std::string op=argv[1];
 auto account=[&](int i){copier::Account a;a.type="MT5";a.user=std::stoull(v[i]);a.password=v[i+1];a.server=v[i+2];a.id=v[i+3];return a;};
 if(op=="start"){copier::StartRequest req;req.master=account(2);req.slave=account(6);req.risk_type=v[10];req.risk_value=v[11];req.fixed_master_balance=v[12];req.copy_sl=true;req.copy_tp=true;auto r=c.Start(req);std::cout<<"{\"ok\":"<<(r.ok?"true":"false")<<",\"copier_id\":"<<q(r.copier_id)<<",\"error\":"<<q(r.error)<<"}\n";}
 else if(op=="list"){auto r=c.List();std::cout<<"{\"ok\":"<<(r.ok?"true":"false")<<",\"error\":"<<q(r.error)<<",\"copiers\":[";bool first=true;for(auto& s:r.copiers){if(!first)std::cout<<',';first=false;std::cout<<"{\"id\":"<<q(s.id)<<",\"paused\":"<<(s.paused?"true":"false")<<"}";}std::cout<<"]}\n";}
 else {copier::SimpleReply r;if(op=="pause"&&argc==4)r=c.Pause(argv[2],std::string(argv[3])=="true");else if(op=="remove"&&argc==3)r=c.Remove(argv[2]);else throw std::runtime_error("Unknown operation");std::cout<<"{\"ok\":"<<(r.ok?"true":"false")<<",\"error\":"<<q(r.error)<<"}\n";}
 }catch(const std::exception& e){std::cout<<"{\"ok\":false,\"error\":"<<q(e.what())<<"}\n";return 1;}}
