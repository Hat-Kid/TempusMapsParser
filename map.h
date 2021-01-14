#pragma once

#include <iostream>

struct Map
{
    Map(std::string name);
    Map(std::string name, std::string intendedClass);
    Map(std::string name, std::string intendedClass, std::string tier);

    std::string m_szName;
    std::string m_szIntendedClass; // "soldier", "demo", "soldier, demo"
    std::string m_iTier;
    bool m_bHasGimmicks;
    bool m_bHasCourses;
};

Map::Map(std::string name) : m_szName(name)
{
    m_szIntendedClass = "soldier";
    m_iTier = "";
    m_bHasGimmicks = false;
    m_bHasCourses = false;
}

Map::Map(std::string name, std::string intendedClass) : m_szName(name), m_szIntendedClass(intendedClass)
{
    m_iTier = "";
    m_bHasGimmicks = false;
    m_bHasCourses = false;
}

Map::Map(std::string name, std::string intendedClass, std::string tier) : m_szName(name), m_szIntendedClass(intendedClass), m_iTier(tier)
{
    m_bHasGimmicks = false;
    m_bHasCourses = false;
}