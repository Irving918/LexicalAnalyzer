#include <iostream>
#include <string>
#include "val.h"

using namespace std;

Value Value::operator+(const Value& op) const{

    if(GetType() == op.GetType()){
        if(IsInt()){
            return Value(GetInt() + op.GetInt());
        }
        else if(IsReal()){
            return Value(GetReal() + op.GetReal());
        }
    }
    else{
        if(IsInt() && op.IsReal()){
            return Value((float)GetInt() + op.GetReal());
        }
        else if(IsReal() + op.IsInt()){
            return Value(GetReal() + (float)op.GetInt());
        }
    }



    return Value();
}

Value Value::operator-(const Value& op) const{

    if(GetType() == op.GetType()){

        if(IsInt()){
            return Value(GetInt() - op.GetInt());
        }
        else if(IsReal()){
            return Value(GetReal() - op.GetReal());
        }
    }
    else{
        if(IsInt() && op.IsReal()){
            return Value((float)GetInt() - op.GetReal());
        }
        else if(IsReal() && op.IsInt()){
            return Value(GetReal() - (float)op.GetInt());
        }
    }

    return Value();
}


Value Value::operator*(const Value& op) const{
    
    if(GetType() == op.GetType()){

        if(IsInt()){
            return Value(GetInt() * op.GetInt());
        }
        else if(IsReal()){
            return Value(GetReal() * op.GetReal());
        }
    }
    else{
        if(IsInt() && op.IsReal()){
            return Value((float)GetInt() * op.GetReal());
        }
        else if(IsReal() && op.IsInt()){
            return Value(GetReal() * (float)op.GetInt());
        }
    }

    return Value();
}

Value Value::operator/(const Value& op) const{

    if(GetType() == op.GetType()){

        if(IsInt()){
            return Value(GetInt() / op.GetInt());
        }
        else if(IsReal()){
            return Value(GetReal() / op.GetReal());
        }
    }
    else{
        if(IsInt() && op.IsReal()){
            return Value((float)GetInt() / op.GetReal());
        }
        else if(IsReal() && op.IsInt()){
            return Value(GetReal() / (float)op.GetInt());
        }
    }

    return Value();
}

Value Value::Catenate(const Value& op) const{

    if(GetType() == op.GetType()){
        if(IsString()){
            return Value(GetString() + op.GetString());
        }
    }

    return Value();
}

Value Value::Power(const Value& op) const{


    if(GetType() == op.GetType()){

        if(IsReal()){
            return Value(pow(GetReal(),op.GetReal()));
        }
        if(IsInt()){
            return Value(pow((float)GetInt(),(float)op.GetInt()));
        }
    }
    else if(IsInt() && op.IsReal()){
        return Value(pow((float)GetInt(), op.GetReal()));   
    }
    else if(IsReal() && op.IsInt()){
        return Value(pow(GetReal() , (float)op.GetInt()));
    }

    return Value();
}

Value Value::operator==(const Value& op) const{

    if(GetType() == op.GetType()){
        if(IsInt()){
            return Value(GetInt() == op.GetInt());
        }
        else if(IsReal()){
            return Value(GetReal() == op.GetReal());
        }
    }
    else if(IsInt() && op.IsReal()){
        return Value((float)GetInt() == op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value(GetReal() == (float)op.GetInt());
    }

    return Value();

}

Value Value::operator>(const Value& op) const{

    if(GetType() == op.GetType()){
        if(IsInt()){
            return Value(GetInt() > op.GetInt());
        }
        else if(IsReal()){
            return Value(GetReal() > op.GetReal());
        }
    }
    else if(IsInt() && op.IsReal()){
        return Value((float)GetInt() > op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value(GetReal() > (float)op.GetInt());
    }

    return Value();



}

Value Value::operator<(const Value& op) const{

    
    if(GetType() == op.GetType()){
        if(IsInt()){
            return Value(GetInt() < op.GetInt());
        }
        else if(IsReal()){
            return Value(GetReal() < op.GetReal());
        }
    }
    else if(IsInt() && op.IsReal()){
        return Value((float)GetInt() < op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value(GetReal() < (float)op.GetInt());
    }

    return Value();
}







