#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <tuple>

template<typename> struct is_char{
    static const bool value = false;
};

template<> struct is_char<char>{
    static const bool value = true;
};

template<> struct is_char<unsigned char>{
    static const bool value = true;
};

template<typename T>
typename std::enable_if<is_char<std::remove_cv<T>::type>::value,std::string>::type 
addr_as_str(T* str){
    return std::string(str);
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value,std::string>::type 
addr_as_str(T addr){
    std::stringstream ss;
    size_t typesize = sizeof(T);
    for(int i = 0; i != typesize; i++){
        if(i!=0) ss<<".";
        ss<<((typename std::make_unsigned<T>::type)addr>>(typesize-i)&0xFF);
    }
    return ss.str();
}

template<typename T>
std::string addr_as_str(std::basic_string<T> str){
  return str;
}

template<typename T>
std::string addr_as_str(std::list<T> ip_list){
    std::stringstream ss;
    for(auto& element: ip_list){
        if(ss.rdbuf()->in_avail() != 0) ss<<".";
        ss<<element;
    }
    return ss.str();
}

template<typename T>
std::string addr_as_str(std::vector<T> ip_list){    
    std::stringstream ss;
    for(auto& element: ip_list){
        if(ss.rdbuf()->in_avail() != 0) ss<<".";
        ss<<element;
    }
    return ss.str();
}

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
each_in_tuple(std::stringstream& ss, std::tuple<Tp...>& t)
  { }

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
each_in_tuple(std::stringstream& ss, std::tuple<Tp...>& t)
  {
    if(I != 0 ) ss<<".";
    ss << std::get<I>(t);
    each_in_tuple<I + 1, Tp...>(ss,t);
  }

template<typename ... Types>
std::string addr_as_str(std::tuple<Types...> tuple){                
    std::stringstream ss;
    each_in_tuple(ss,tuple);
    return ss.str();
}

int main(){    
    std::cout<<addr_as_str(char(-1))<<std::endl;
    std::cout<<addr_as_str(short(0))<<std::endl;
    std::cout<<addr_as_str(std::vector<int>{127,0,0,1})<<std::endl;
    std::cout<<addr_as_str(std::make_tuple(123,45,67,89,101,112,131,41))<<std::endl;
    return 0;
}