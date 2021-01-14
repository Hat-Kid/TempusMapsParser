#include <iostream>

#include <curl/curl.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <Poco/JSON/Parser.h>

#include "include/json.hpp"
#include "include/inja.hpp"
#include "include/csv.h"

#include "map.h"

std::string FetchMapClassData()
{
    std::string result;

    try
    {
        curlpp::Cleanup curlCleanup;
        std::ostringstream ostream;
        ostream << curlpp::options::Url("https://raw.githubusercontent.com/Hona/TempusHub/master/src/TempusHubBlazor/wwwroot/MapClasses.csv");
        result = ostream.str();
    }
    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (curlpp::LogicError &e)
    {
        std::cout << e.what() << std::endl;
    }

    return result;
}

void FetchMapTierData(std::string mapName, int &soldierTier, int &demoTier) // tier_info: 3 = soldier, 4 = demoman
{
    try
    {
        using namespace Poco::JSON;

        curlpp::Cleanup myCleanup;
        std::ostringstream jsonStream;
        Parser parser;
        Poco::Dynamic::Var parseResult;
        std::string resultStr;
        std::string apiUrl = "https://tempus.xyz/api/maps/detailedList";

        jsonStream << curlpp::options::Url(apiUrl);
        resultStr = jsonStream.str();
        parseResult = parser.parse(resultStr);
        Array::Ptr arrObj = parseResult.extract<Array::Ptr>();

        for (int i = 0; i < arrObj->size(); i++)
        {
            Object::Ptr mapObj = arrObj->getObject(i);

            if (mapObj->getValue<std::string>("name") == mapName)
            {
                soldierTier = mapObj->getObject("tier_info")->getValue<int>("3");
                demoTier = mapObj->getObject("tier_info")->getValue<int>("4");
            }
        }
    }
    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError &e)
    {
        std::cout << e.what() << std::endl;
    }
}

std::vector<Map> CreateMapList()
{
    std::string fileIn = FetchMapClassData();

    std::ofstream out("mapclasses.csv");
    out << fileIn;
    out.close();

    io::CSVReader<2> in("mapclasses.csv");
    std::string className, mapName;
    int soldierTier, demoTier;

    std::vector<Map> mapList = {};

    while (in.read_row(className, mapName))
    {
        Map map(mapName);
        FetchMapTierData(mapName, soldierTier, demoTier);

        if (className == "S")
        {
            map.m_szIntendedClass = "soldier";
            map.m_iTier = "t" + std::to_string(soldierTier);
        }
        else if (className == "D")
        {
            map.m_szIntendedClass = "demo";
            map.m_iTier = "t" + std::to_string(demoTier);
        }
        else if (className == "B")
        {
            map.m_szIntendedClass = "soldier, demo";
            map.m_iTier = "t" + std::to_string(soldierTier) + ", t" + std::to_string(demoTier);
        }
        else
        {
            continue;
        }

        mapList.push_back(map);
    }

    return mapList;
}

void ParseResultsIntoJson()
{
    using namespace inja;
    using json = nlohmann::json;

    json data;
    Environment env;
    Template temp = env.parse_template("jsontemplate.txt");
    std::string result;

    std::vector<Map> mapList = CreateMapList();

    for (int i = 0; i < mapList.size(); i++)
    {
        data["mapName"] = mapList.at(i).m_szName;
        data["className"] = mapList.at(i).m_szIntendedClass;
        data["mapTier"] = mapList.at(i).m_iTier;
        data["mapScreenshot"] = mapList.at(i).m_szName + "_248.jpeg";
        data["mapHasCourses"] = false /*mapList.at(i).m_bHasCourses*/;
        data["mapHasGimmicks"] = false /*mapList-.at(i).m_bHasGimmicks*/; // TODO: allow filtering for courses and gimmick maps

        if (i == mapList.size() - 1)
        {
            result += env.render(temp, data);
        }
        else
        {
            result += env.render(temp, data) + "," + "\n";
        }
    }

    std::ofstream out("result.json");
    out << result;
    out.close();
}

int main(int, char **argv)
{
    ParseResultsIntoJson();
}