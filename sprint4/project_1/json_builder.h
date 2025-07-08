#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <memory>

#include "json.h"

namespace json {
/*
Key(std::string). При определении словаря задаёт строковое значение ключа для очередной пары ключ-значение. Следующий вызов метода обязательно должен задавать соответствующее этому ключу значение с помощью метода Value или начинать его определение с помощью StartDict или StartArray.
Value(Node::Value). Задаёт значение, соответствующее ключу при определении словаря, очередной элемент массива или, если вызвать сразу после конструктора json::Builder, всё содержимое конструируемого JSON-объекта. Может принимать как простой объект — число или строку — так и целый массив или словарь.
Здесь Node::Value — это синоним для базового класса Node, шаблона variant с набором возможных типов-значений. Смотрите заготовку кода.
StartDict(). Начинает определение сложного значения-словаря. Вызывается в тех же контекстах, что и Value. Следующим вызовом обязательно должен быть Key или EndDict.
StartArray(). Начинает определение сложного значения-массива. Вызывается в тех же контекстах, что и Value. Следующим вызовом обязательно должен быть EndArray или любой, задающий новое значение: Value, StartDict или StartArray.
EndDict(). Завершает определение сложного значения-словаря. Последним незавершённым вызовом Start* должен быть StartDict.
EndArray(). Завершает определение сложного значения-массива. Последним незавершённым вызовом Start* должен быть StartArray.
Build(). Возвращает объект json::Node, содержащий JSON, описанный предыдущими вызовами методов. К этому моменту для каждого Start* должен быть вызван соответствующий End*. При этом сам объект должен быть определён, то есть вызов json::Builder{}.Build() недопустим.
*/

class Builder {
    mutable Node root_;
    std::vector<std::unique_ptr<json::Node>> stack_;
public:
    Builder& Value(Node::Value);

    Builder& Key(std::string);
    Builder& StartDict();
    Builder& EndDict();

    Builder& StartArray();
    Builder& EndArray();

    json::Node Build();

};

}  // namespace json