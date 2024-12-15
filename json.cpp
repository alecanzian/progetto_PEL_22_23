#include "json.hpp"
#include <limits>
#include <assert.h>
#include <fstream>

struct json::impl{
    
    struct cell_list{
        json info;
        cell_list* next;
    };
    typedef cell_list* List;

    struct cell_dict{
        std::pair<std::string,json> info;
        cell_dict* next;
    };
    typedef cell_dict* Dict;

    List list_head;
    List list_tail;
    Dict dict_head;
    Dict dict_tail;
    double* num;
    bool* boolean;
    std::string* str;

    int used;//per veddere se è null(-1), una lista settata(1),un dizionario settato(2), altrimenti vale 0 per str,bool,num

    void destroy(json&);
    void destroy_list(List&);
    void destroy_dictionary(Dict&);

    void copy(json const&, json&);  
    void copy_list(json const& obj, json& res);
    void copy_dictionary(json const& obj, json& res);  

    void print(json const&, int num, bool cond); 

};

struct json::list_iterator{
		using iterator_category = std::forward_iterator_tag;
        using value_type =  json;
		using pointer = json*;
		using reference = json&;
		
		list_iterator(impl::List p){
			m_ptr = p;
		}

		reference operator*() const{
			return m_ptr->info;
		}
		pointer operator->() const{
			return &(m_ptr->info);
		}
		list_iterator& operator++(){
			m_ptr = m_ptr->next;
			return *this;
		}
		list_iterator operator++(int){
			list_iterator tmp = *this;
			m_ptr = m_ptr->next;
			return tmp;
		}
		bool operator==(list_iterator const& p) const{
			return m_ptr == p.m_ptr;
		}
		bool operator!=(list_iterator const& p) const{
			//return!(*this==p);
            return m_ptr != p.m_ptr;
		}
	
	private:
		impl::List m_ptr;
};

struct json::const_list_iterator{
		using iterator_category = std::forward_iterator_tag;
		using value_type = const json;
		using pointer = json const*;
		using reference = json const&;
		
		const_list_iterator(impl::List p){
			m_ptr = p;
		}
		
		reference operator*() const{
			return m_ptr->info;
		}
		pointer operator->() const{
			return &(m_ptr->info);
		}
		const_list_iterator& operator++(){
			m_ptr = m_ptr->next;
			return *this;
		}
		const_list_iterator operator++(int){
			const_list_iterator tmp = *this;
			m_ptr = m_ptr->next;
			return tmp;
		}
		bool operator==(const_list_iterator const& p) const{
			return m_ptr == p.m_ptr;
		}
		bool operator!=(const_list_iterator const& p) const{
			//return !(*this == p);
            return m_ptr != p.m_ptr;
		}
	
	private:
		impl::List m_ptr;
};

struct json::dictionary_iterator{
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<std::string,json>;
		using pointer = std::pair<std::string,json>*;
		using reference = std::pair<std::string,json>&;
		
		dictionary_iterator(impl::Dict p){
			m_ptr = p;
		}
		
		reference operator*() const{
			return m_ptr->info;
		}
		pointer operator->() const{
			return &(m_ptr->info);
		}
		dictionary_iterator& operator++(){
			m_ptr = m_ptr->next;
			return *this;
		}
		dictionary_iterator operator++(int){
			dictionary_iterator tmp = *this;
			m_ptr = m_ptr->next;
			return tmp;
		}
		bool operator==(dictionary_iterator const& p) const{
			return m_ptr == p.m_ptr;
		}
		bool operator!=(dictionary_iterator const& p) const{
			//return !(*this == p);
            return m_ptr != p.m_ptr;
		}

	private:
		impl::Dict m_ptr;
};

struct json::const_dictionary_iterator{
		using iterator_category = std::forward_iterator_tag;
		using value_type = const std::pair<std::string,json>;
		using pointer = std::pair<std::string,json> const*;
		using reference = std::pair<std::string,json> const&;
		
		const_dictionary_iterator(impl::Dict p){
			m_ptr = p;
		}
		
		reference operator*() const{
			return m_ptr->info;
		}
		pointer operator->() const{
			return &(m_ptr->info);
		}
		const_dictionary_iterator& operator++(){
			m_ptr = m_ptr->next;
			return *this;
		}
		const_dictionary_iterator operator++(int){
			const_dictionary_iterator tmp = *this;
			m_ptr = m_ptr->next;
			return tmp;
		}
		bool operator==(const_dictionary_iterator const& p) const{
			return m_ptr == p.m_ptr;
		}
		bool operator!=(const_dictionary_iterator const& p) const{
			//return!(*this == p);
            return m_ptr != p.m_ptr;
		}

	private:
		impl::Dict m_ptr;
};


//Default constructor
json::json(){
    pimpl = new impl;

    pimpl->boolean = nullptr;
    pimpl->num = nullptr;
    pimpl->str = nullptr;

    pimpl->list_head= nullptr;
    pimpl->list_tail = nullptr;

    pimpl->dict_head= nullptr;
    pimpl->dict_tail = nullptr;

    pimpl->used = -1;
}

//Copy constructor
json::json(json const& obj):json(){
    assert(this->is_null());
    if(!obj.is_null()) pimpl->copy(obj, *this);
}

//Move constructor
json::json(json&& obj):json(){
    if(!obj.is_null()){
        if(obj.is_bool()){
            pimpl->boolean = obj.pimpl->boolean;
            obj.pimpl->boolean = nullptr;
            pimpl->used = 0;
        }
        else if(obj.is_number()){
            pimpl->num = obj.pimpl->num;
            obj.pimpl->num = nullptr;
            pimpl->used = 0;
        }
        else if(obj.is_string()){
            pimpl->str = obj.pimpl->str;
            obj.pimpl->str = nullptr;
            pimpl->used = 0;
        }
        else if(obj.is_dictionary()){
            pimpl->dict_head = obj.pimpl->dict_head;
            pimpl->dict_tail = obj.pimpl->dict_tail;
            obj.pimpl->dict_head  = nullptr;
            obj.pimpl->dict_tail = nullptr;
            pimpl->used = 2;
        }
        else if(obj.is_list()){
            pimpl->list_head = obj.pimpl->list_head;
            pimpl->list_tail = obj.pimpl->list_tail;
            obj.pimpl->list_head  = nullptr;
            obj.pimpl->list_tail = nullptr;
            pimpl->used = 1;
        }
    }

}

//Destructor
json::~json(){
    if(!is_null()) pimpl->destroy(*this);
    delete pimpl;
}

//Funione ricorsiva che elimina tutti i nodi delle liste o dei dizionari
void json::impl::destroy(json& obj){
    if(obj.is_list()){
        destroy_list(obj.pimpl->list_head);
        obj.pimpl->list_head = obj.pimpl->list_tail = nullptr;
    }
    else if(obj.is_dictionary()){
        destroy_dictionary(obj.pimpl->dict_head);
        obj.pimpl->dict_head = obj.pimpl->dict_tail = nullptr;
    }
    else if(obj.is_string()){
        delete obj.pimpl->str;
        obj.pimpl->str = nullptr;
    }
    else if(obj.is_bool()){
        delete obj.pimpl->boolean;
        obj.pimpl->boolean = nullptr;
    }
    else if(obj.is_number()){
        delete obj.pimpl->num;
        obj.pimpl->num = nullptr;
    }
    obj.pimpl->used = -1;
    assert(obj.is_null());

}
void json::impl::destroy_list(impl::List& ptr){
    
    auto head = ptr;
    while(head != nullptr){
        head = head->next;
        delete ptr;
        ptr = head;
    }
}
void json::impl::destroy_dictionary(impl::Dict& ptr){
    auto head = ptr;
    while(head != nullptr){
        head = head->next;
        delete ptr;
        ptr = head;
    }

}

//Funzione ricorsiva che copia il contenuto dell'oggetto
void json::impl::copy(json const& obj, json& res){
    if(obj.is_dictionary()){
        res.set_dictionary();
        for(auto it = obj.begin_dictionary(); it != obj.end_dictionary(); it++){
            res.insert(std::pair<std::string, json>{(*it).first, (*it).second});
        }
    }
    if(obj.is_list()){
        res.set_list();
        for(auto it = obj.begin_list(); it != obj.end_list(); it++){
            res.push_back(*it);
        }
    }
    else if((obj).is_bool())
            (res).set_bool((obj).get_bool());
    else if((obj).is_number())
            (res).set_number((obj).get_number());
    else if((obj).is_string())
            (res).set_string((obj).get_string());    
}

//Operatori di assegnamento
json& json::operator=(json const& obj){
    if(this != &obj){
        pimpl->destroy(*this);
        assert(is_null());
        pimpl->copy(obj, *this);
    }
    return *this;
}
json& json::operator=(json&& obj){
    if(this != &obj){
        pimpl->destroy(*this);
        if(!obj.is_null()){
            if(obj.is_bool()){
                pimpl->boolean = obj.pimpl->boolean;
                obj.pimpl->boolean = nullptr;
                pimpl->used = obj.pimpl->used;
            }
            else if(obj.is_number()){
                pimpl->num = obj.pimpl->num;
                obj.pimpl->num = nullptr;
                pimpl->used = obj.pimpl->used;
            }
            else if(obj.is_string()){
                pimpl->str = obj.pimpl->str;
                obj.pimpl->str = nullptr;
                pimpl->used = obj.pimpl->used;
            }
            else if(obj.is_dictionary()){
                pimpl->dict_head = obj.pimpl->dict_head;
                pimpl->dict_tail = obj.pimpl->dict_tail;
                obj.pimpl->dict_head  = nullptr;
                obj.pimpl->dict_tail = nullptr;
                pimpl->used = obj.pimpl->used;
            }
            else if(obj.is_list()){
                pimpl->list_head = obj.pimpl->list_head;
                pimpl->list_tail = obj.pimpl->list_tail;
                obj.pimpl->list_head  = nullptr;
                obj.pimpl->list_tail = nullptr;
                pimpl->used = obj.pimpl->used;
            }
        }
    }
    return *this;
}


bool json::is_list() const{
    return (pimpl->list_head != nullptr or pimpl->used == 1);
}
bool json::is_dictionary() const{
    return (pimpl->dict_head!= nullptr or pimpl->used == 2);
}
bool json::is_string() const{
    return pimpl->str != nullptr and pimpl->used == 0;
}
bool json::is_number() const{
    return pimpl->num != nullptr and pimpl->used == 0;
}
bool json::is_bool() const{
    return pimpl->boolean != nullptr and pimpl->used == 0;
}
bool json::is_null() const{
    //return (pimpl->used == -1);
    return !(is_list() or is_dictionary() or is_string() or is_number() or is_bool());
}


json const& json::operator[](std::string const& word) const{
    if(is_dictionary()){
        if(pimpl->dict_head == nullptr)
            throw json_exception{"Dictionary is empty"};
        for(auto it = begin_dictionary(); it != end_dictionary(); it++){
            if(it->first == word)
                return it->second;
        }
        throw json_exception{"Word not found in the dictionary"};
    }
    throw json_exception{"Object is not a dictionary(operator[])"};
    
}
json& json::operator[](std::string const& word){
    if(is_dictionary()){
            auto it = begin_dictionary();
            auto prev = it;
            for(; it != end_dictionary(); it++){
                if(it->first == word)
                    return it->second;
                prev = it;
            }
        this->insert(std::pair<std::string,json> (word, json()));
        if(prev != nullptr)
            return (++prev)->second;
        
        prev = begin_dictionary();
        return prev->second;
    }
    throw json_exception{"Object is not a dictionary(operator[])"};
}


double& json::get_number(){
    if(is_number())
        return *(pimpl->num);
    else throw json_exception{"Object is not a number(get_number)"};
}
double const& json::get_number() const{
    if(is_number())
        return *(pimpl->num);
    else throw json_exception{"Object is not a number(get_number)"};
}
bool& json::get_bool(){
    if(is_bool())
        return*(pimpl->boolean);
    else throw json_exception{"Object is not a bool(get_bool)"};    
}
bool const& json::get_bool() const{
    if(is_bool())
        return*(pimpl->boolean);
    else throw json_exception{"Object is not a bool(get_bool)"};
}
std::string& json::get_string(){
    if(is_string())
        return *(pimpl->str);
    else throw json_exception{"Object is not a string(get_string)"};
}
std::string const& json::get_string() const{
    if(is_string())
        return *(pimpl->str);
    else throw json_exception{"Object is not a string(get_string)"};
}

void json::set_string(std::string const& obj){
    if(!(is_string())){
        if(!is_null())
            pimpl->destroy(*this);    
        pimpl->str = new std::string;
        *(pimpl->str) = obj;
        pimpl->used = 0;
    }
    else
        *(pimpl->str) = obj;
}
void json::set_bool(bool obj){
    if(!is_bool()){
        if(!is_null())
            pimpl->destroy(*this);
        pimpl->boolean = new bool;
        *(pimpl->boolean) = obj;
        pimpl->used = 0;
    }
    else
        *(pimpl->boolean) = obj;
}
void json::set_number(double obj){
    if(!(is_number())){
        if(!is_null())
            pimpl->destroy(*this);
        pimpl->num = new double;
        *(pimpl->num) = obj;
        pimpl->used = 0;
    }
    else
        *(pimpl->num) = obj;
}
void json::set_null(){
    if(!is_null()){
        pimpl->destroy(*this);
    }
}
void json::set_list(){
    if(!is_null()) 
        pimpl->destroy(*this);
    pimpl->used = 1;
}
void json::set_dictionary(){
    if(!is_null()) 
        pimpl->destroy(*this);
    pimpl->used = 2;
}


void json::push_front(json const& obj){
    if(this->is_list()){
        impl::List pc = new impl::cell_list{obj, pimpl->list_head};
        if(pimpl->list_tail == nullptr){
            pimpl->list_tail = pc;
        }
        pimpl->list_head = pc;
    }
    else 
        throw json_exception{"object is not a list(push_front)"};
}
void json::push_back(json const& obj){
    if(this->is_list()){
        auto pc = new impl::cell_list{obj,nullptr};
        if(pimpl->list_head == nullptr){
            pimpl->list_head = pc;
        }
        else pimpl->list_tail->next = pc;
        pimpl->list_tail = pc;
    }
    else 
        throw json_exception{"object is not a list(push_back)"};
}
void json::insert(std::pair<std::string,json> const& obj){
    if(this->is_dictionary()){
        auto pc = new impl::cell_dict{obj,nullptr};
        if(pimpl->dict_head == nullptr)
            pimpl->dict_head = pc;
        else
            pimpl->dict_tail->next = pc;
        pimpl->dict_tail = pc;
    }
    else 
        throw json_exception{"object is not a dictionary(insert)"};
}


void print(json const& obj, int container, bool h) {
    if(obj.is_list()){
        char c = --container;
        if(h == true)
            while(c > 0){
                std::cout<<" ";
                c--;
            }
        
        container++;
        std::cout<<"["<<std::endl;

        for(auto it = obj.begin_list(); it != obj.end_list(); ){
            if(it->is_list() or it->is_dictionary()){
                c = container;
                while(c > 0){
                    std::cout<<" ";
                    c--;
                }
                
                int a = container + 1;
                print(*it,a, false);
            }  
            else{
                c = container;
                while(c > 0){
                    std::cout<<" ";
                    c--;
                }
               
                if(it->is_bool()){
                    if(it->get_bool() == true) std::cout<<"true";
                    else std::cout<<"false";
                }
                else if(it->is_number()) std::cout<<it->get_number();
                else if(it->is_string()) std::cout<<"\""<<it->get_string()<<"\"";
                else if(it->is_null()) std::cout<<"null";
            }
            if((++it) != obj.end_list()) std::cout<<",";
            std::cout<<std::endl;
        }
        c = --container;
        while(c > 0){
            std::cout<<" ";
            c--;
        }
        
        std::cout<<"]";
    }
    else if(obj.is_dictionary()){
        char c = --container;
        if(h == true){
        while(c > 0){
            std::cout<<" ";
            c--;
        }
        
        }
        container++;
        std::cout<<"{"<<std::endl;
        for(auto it = obj.begin_dictionary(); it != obj.end_dictionary(); ){
    
            if(it->second.is_list() or it->second.is_dictionary()){
                c = container;
                while(c > 0){
                    std::cout<<" ";
                    c--;
                }
                
                std::cout<<"\""<<it->first<<"\" : ";
                int a = container + /*it->first.length() + 6*/1;
                print((*it).second,a,false);
            }
            else{
                c = container;
                while(c > 0){
                    std::cout<<" ";
                    c--;
                }
               
                std::cout<<"\""<<it->first<<"\" : ";
                if(it->second.is_bool()){
                    if(it->second.get_bool() == true) std::cout<<"true";
                    else std::cout<<"false";
                }
                else if(it->second.is_number()) std::cout<<it->second.get_number();
                else if(it->second.is_string()) std::cout<<"\""<<it->second.get_string()<<"\"";
                else if(it->second.is_null()) std::cout<<"null";
            }
            if((++it) != obj.end_dictionary())
                std::cout<<",";
             std::cout<<std::endl;
        }
        c = --container;
        while(c > 0){
            std::cout<<" ";
            c--;
        }
        
        std::cout<<"}";
    }
    else{
                
        if(obj.is_bool()){
            if(obj.get_bool() == true) std::cout<<"true";
            else std::cout<<"false";
        }
        else if(obj.is_number()) std::cout<<obj.get_number();
        else if(obj.is_string()) std::cout<<"\""<<obj.get_string()<<"\"";
        else if(obj.is_null()) std::cout<<"null";
    }
}

json::list_iterator json::begin_list(){
    if(is_list())
        return {pimpl->list_head};
    else
        throw json_exception{"object is not a list(begin_list)"};
}
json::const_list_iterator json::begin_list() const{
    if(is_list())
        return {pimpl->list_head};
    else
        throw json_exception{"object is not a list(begin_list)"};
}
json::list_iterator json::end_list(){
    if(is_list())
        return {nullptr};
    else 
        throw json_exception{"object is not a list(end_list)"};
}
json::const_list_iterator json::end_list() const{
    if(is_list())
        return {nullptr};
    else 
        throw json_exception{"object is not a list(end_list)"};
}

json::dictionary_iterator json::begin_dictionary(){
    if(is_dictionary())
        return {pimpl->dict_head};
    else 
        throw json_exception{"object is not a dictionary(begin_dictionary)"};
}
json::const_dictionary_iterator json::begin_dictionary() const{
    if(is_dictionary())
        return {pimpl->dict_head};
    else 
        throw json_exception{"object is not a dictionary(begin_dictionary)"};
}
json::dictionary_iterator json::end_dictionary(){
    if(is_dictionary()) 
        return {nullptr};
    else 
        throw json_exception{"object is not a dictionary(end_dictionary)"};
}
json::const_dictionary_iterator json::end_dictionary() const{
    if(is_dictionary()) 
        return {nullptr};
    else 
        throw json_exception{"object is not a dictionary(end_dictionary)"};
}


void parse_list(std::istream&, json&);
void parse_dictionary(std::istream&, json&);
void read_json(std::istream&, json&);
void delete_whitespace(std::istream&);
void parse_string(std::istream& is, json& obj){
    char c;
    std::string res;
    while (is.get(c)) {
        if (c == '\"' && res.back() != '\\') 
            break;
        else 
            res += c;
    }
    obj.set_string(res);
}
void parse_number(std::istream& is, json& obj){
    float number;
    is>>number;
    obj.set_number(number);
}
void parse_null_or_bool(std::istream& is, json& obj){
    std::string line;

    char t = is.peek();
    if(t == 'n') line = "null";
    else if(t == 't'){
        line = "true";
        obj.set_bool(true);
    }
    else if(t == 'f'){
        line = "false";
        obj.set_bool(false);
    }
    while(t>='a' and t<='z'){
        is.get(t);//mangia il carattere
        if(t != line[0]) throw json_exception{"Parse error: lessical error"};
        line.erase(0,1);
        t = is.peek();
    }
    if(line != "") throw json_exception{"Parse error: missing a character"};
}


void delete_whitespace(std::istream& is){
    while(is.peek() == 32 or is.peek() == 10 or is.peek() == 9 or is.peek() == 13)//\n = 10, tab = 9, whitespace = 32, carriage return = 13
        is.ignore(1);
}
void parse_list(std::istream& is, json& obj) {
    obj.set_list();
    char t;

    if(is.peek() != '[') throw json_exception{"Parse error: t != '['"};
    is.ignore(1);   //ignora [
    
    
    do{
        delete_whitespace(is);

        if(is.peek() == ']') {
            is.ignore(1);
            return;   
        }
        else if(is.peek() == '"'){//stringa
            is.ignore(1);
            json out;
            parse_string(is, out);
            obj.push_back(out);
        }
        else if(((char)is.peek() >= '0' and (char)is.peek() <= '9') or (char)is.peek() == '-' or (char)is.peek() == '+' or (char)is.peek() == '.' ){//numero
            json out;
            parse_number(is, out);
            obj.push_back(out);
        }
        else if((char)is.peek() >= 'a' and (char)is.peek() <= 'z'){//booleano o null
            json out;
            parse_null_or_bool(is, out);
            obj.push_back(out);
        }
        else if((char)is.peek() == '['){
            json out;
            parse_list(is, out);
            obj.push_back(out);
        }
        if((char)is.peek() == '{'){
            json out;
            parse_dictionary(is, out);
            obj.push_back(out);
        }

        delete_whitespace(is);
        is.get(t);  //mangia , o ]
       
        if( t == ','){
            delete_whitespace(is);
            if(is.peek() == ']') throw json_exception{"Parse error: missing an element in list"};
        }
        else if(t != ']')
            throw json_exception{"Parse error: t != ']' and t != ','"};
        
        
    }while(t != ']');
}

void parse_dictionary(std::istream& is, json& obj){
    obj.set_dictionary();
    char t;

    if(is.peek() != '{') throw json_exception{"Parse error: t != '{'"};
    is.ignore(1);   //ignora {    

    do{
        delete_whitespace(is);

        if(is.peek() == '}'){
            is.ignore();
            return;
        }
        if(is.peek() != '"') throw json_exception{"Parse error: t != '\"'"};
        is.ignore(1);   //ignora "
        
        json str;
        parse_string(is, str);

        delete_whitespace(is);

        if(is.peek() != ':') throw json_exception{"Parse error: t != ':'"};
        is.ignore(1);   //ignora :
        
        delete_whitespace(is);

        if(is.peek() == '"'){//stringa
            is.ignore(1);
            json out;
            parse_string(is, out);
            obj.insert(std::pair<std::string,json>{str.get_string(), out});
        }
        if(((char)is.peek() >= '0' and (char)is.peek() <= '9') or (char)is.peek() == '-' or (char)is.peek() == '+' or (char)is.peek() == '.'){//numero
            json out;
            parse_number(is, out);
            obj.insert(std::pair<std::string,json>{str.get_string(), out});
        }
        if((char)is.peek() >= 'a' and (char)is.peek() <= 'z'){//booleano o null
            json out;
            parse_null_or_bool(is, out);
            obj.insert(std::pair<std::string,json>{str.get_string(), out});
        }
        if((char)is.peek() == '['){
            json out;
            parse_list(is, out);
            obj.insert(std::pair<std::string,json>{str.get_string(), out});
        }
        if((char)is.peek()== '{'){
            json out;
            parse_dictionary(is, out);
            obj.insert(std::pair<std::string,json>{str.get_string(), out});
        }

        delete_whitespace(is);

        is.get(t);     //mangia , o }
        if( t == ','){
            delete_whitespace(is);
            if( is.peek() == '}') throw json_exception{"Parse error: missing an element in dictionary"};
        }
        else if(t != '}') 
            throw json_exception{"Parse error: t != '}' and t != ','"};
        
        

    }while(t != '}');

}
void read_json(std::istream & is, json& obj){
    delete_whitespace(is);
    if(is.peek() == '[')
        parse_list(is, obj);
    else if(is.peek() == '{')
        parse_dictionary(is, obj);
    else throw json_exception{"Parse error"};
}

std::ostream& operator<<(std::ostream& lhs, json const& rhs){
    print(rhs, 1, true);
    std::cout<<std::endl;
    return lhs;
}
std::istream& operator>>(std::istream& lhs, json& rhs){
    read_json(lhs, rhs);
    return lhs;
}
