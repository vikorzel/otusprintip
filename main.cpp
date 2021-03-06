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

/*!
 \brief Проверка на тип char
        
        Шаблон проверяет является ли переданный в него тип (unsigned) char или нет
*/
template<> struct is_char<char>{
    static const bool value = true;
};

/*!
 \brief Проверка на тип char
       
       Шаблон проверяет является ли переданный в него тип (unsigned) char или нет
*/
template<> struct is_char<unsigned char>{
    static const bool value = true;
};

template <class T>
constexpr bool is_char_v = is_char<T>::value;

template< class T >
constexpr bool is_integral_v = std::is_integral<T>::value;

/*!
 \brief Проверка списка типов на идентичность
        
        Шаблон проверяет все ли переданные в него типы одинаковы
*/
template<typename ... Types>
struct is_same_types;

template<typename ... Types>
constexpr bool is_same_types_v = is_same_types<Types...>::value;

template<typename ... Types>
constexpr bool is_same_v = std::is_same<Types...>::value;

/*!
 \brief Проверка списка типов на идентичность
        
        Шаблон проверяет все ли переданные в него типы одинаковы
*/
template<typename T, typename U, typename ... Types>
struct is_same_types<T,U,Types...>{
    static const bool value =   (is_same_types_v<T,U>)&&
                                (is_same_types_v<T,Types...>)&&
                                (is_same_types_v<U, Types...>);
};

/*!
 \brief Проверка списка типов на идентичность
        
        Шаблон проверяет все ли переданные в него типы одинаковы
*/
template<typename T, typename U>
struct is_same_types<T,U>{
    static const bool value = is_same_v<T,U>;
};

/*!
 \brief Проверка на наличие get_allocator
        
        Проверяет наличие у переданного типа метода get_allocator
*/
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
typename std::enable_if_t<is_char_v<typename std::remove_cv_t<T>>,std::string> 
addr_as_str(T* str){
    return std::string(str);
}

template<typename T>
typename std::enable_if_t<is_integral_v<T>,std::string> 
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
typename std::enable_if_t<is_container<T>::value,std::string>
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

template<std::size_t I = 0, typename... Tp>
typename std::enable_if_t<I == sizeof...(Tp), void>
each_in_tuple(std::stringstream&, std::tuple<Tp...>&)
{ }

template<std::size_t I = 0, typename... Tp>
typename std::enable_if_t<I < sizeof...(Tp), void>
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