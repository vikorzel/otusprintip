#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <tuple>
#include <type_traits>


/*!
 \brief Проверка на тип char
        Шаблон проверяет является ли переданный в него тип (unsigned) char или нет
*/
template<typename> struct is_char{
    static const bool value = false;
};

template<> struct is_char<char>{
    static const bool value = true;
};

template<> struct is_char<unsigned char>{
    static const bool value = true;
};

template<typename ... Types>
struct is_same_types;

template<typename T, typename U, typename ... Types>
struct is_same_types<T,U,Types...>{
    static const bool value =   (is_same_types<T,U>::value)&&
                                (is_same_types<T,Types...>::value)&&
                                (is_same_types<U, Types...>::value);
};

template<typename T, typename U>
struct is_same_types<T,U>{
    static const bool value = std::is_same<T,U>::value;
};

template<typename T>
struct is_container{
    template<typename,typename> class checker;

    template<typename C>
    static std::true_type test(checker<C, decltype(&C::get_allocator)> *);

    template<typename C>
    static std::false_type test(...);

    static const bool value = std::is_same<std::true_type, decltype(test<T>(nullptr))>::value;
    
};


template<typename T>
typename std::enable_if<is_char<typename std::remove_cv<T>::type>::value,std::string>::type 
addr_as_str(T* str){
    return std::string(str);
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value,std::string>::type 
addr_as_str(T addr){
    std::stringstream ss;
    size_t typesize = sizeof(T);
    for(int i = typesize-1; i >= 0; --i){
        if(i!=((int)typesize-1)) ss<<".";
        ss<<(((addr)>>(8*i))&0xFF);
    }
    return ss.str();
}

template<typename T>
std::string addr_as_str(std::basic_string<T> str){
  return str;
}

template<typename T>
typename std::enable_if<is_container<T>::value,std::string>::type
addr_as_str(T addr){
    std::stringstream ss;
    bool is_first = true;
    for( auto e: addr ){
        if(!is_first){
            ss<<".";
        }else{
            is_first=false;
        }

        ss<<e;
    }
    return ss.str();
}
/*


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
}*/

template<std::size_t I = 0, typename... Tp>
typename std::enable_if<I == sizeof...(Tp), void>::type
each_in_tuple(std::stringstream&, std::tuple<Tp...>&)
{ }

template<std::size_t I = 0, typename... Tp>
typename std::enable_if<I < sizeof...(Tp), void>::type
each_in_tuple(std::stringstream& ss, std::tuple<Tp...>& t){
    if(I != 0 ) ss<<".";
    ss << std::get<I>(t);
    each_in_tuple<I + 1, Tp...>(ss,t);
}

template<typename ... Types>
std::string addr_as_str(std::tuple<Types...> tuple){     
    static_assert(is_same_types<Types...>::value,"Found different types in tuple");           
    std::stringstream ss;
    each_in_tuple(ss,tuple);
    return ss.str();
}

int main(){  
    std::cout<<addr_as_str(char(-1))<<std::endl;
    std::cout<<addr_as_str(short(0))<<std::endl;
    std::cout<<addr_as_str(int{2130706433})<<std::endl;
    std::cout<<addr_as_str(long{8875824491850138409})<<std::endl;
    std::cout<<addr_as_str(std::string{"8.800.55.35.35"})<<std::endl;
    std::cout<<addr_as_str(std::list<short>{12,85,0,0});
    std::cout<<addr_as_str(std::vector<int>{6,7,8,9})<<std::endl;
    std::cout<<addr_as_str(std::make_tuple('8','8','8','8'))<<std::endl;
    return 0;
}