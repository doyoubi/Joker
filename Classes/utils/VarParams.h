#ifndef JOKER_VAR_PARAMS
#define JOKER_VAR_PARAMS

#include <unordered_map>

#include "utils/debug.h"

namespace joker
{

    class ParameterBase
    {
    public:
        // if there is not virutal function, we can't use dynamic_cast
        // since there is no type info stored in vtable
        virtual ~ParameterBase() {} 
        template<class T> const T& get() const;
        template<class T> void set(const T & val);
    };

    template<class T>
    class Parameter : public ParameterBase
    {
    public:
        Parameter(const T & value) : _value(value) {}
        const T & get() const { return _value; }
        void set(const T & value) { _value = value; }
    private:
        T _value;
    };

    template<class T>
    const T & ParameterBase::get() const
    {
        Parameter<T> * p = dynamic_cast<Parameter<T> *>(this);
        DEBUGCHECK(p, "incorrect type for ParameterBase::get");
        return p->get();
    }

    template<class T>
    void ParameterBase::set(const T & value)
    {
        Parameter<T> * p = dynamic_cast<Parameter<T> *>(this);
        DEBUGCHECK(p, "incorrect type for ParameterBase::set");
        p->set(value);
    }

    class VarParams
    {
    public:
        typedef std::unique_ptr<ParameterBase> Ptr;

        template<class T>
        T & get(const char * paramName) const
        {
            DEBUGCHECK(_params.count(paramName) == 1,
                "VarParmas::get : the parameter " + string(paramName) + " not exist");
            return _params.at(paramName)->get<T>();
        }
        
        template<class T>
        void add(const char * paramName, T & value)
        {
            DEBUGCHECK(_params.count(paramName) == 0,
                "VarParmas::get : the parameter " + string(paramName) + " already exist");
            auto p = Ptr(new Parameter<T>(value));
            _params.emplace(paramName, std::move(p));
        }

    private:
        std::unordered_map<string, Ptr> _params;
    };

}

#endif
