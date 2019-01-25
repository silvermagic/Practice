#include <iostream>
#include <sstream>
#include <string>
#include "pugixml.hpp"

std::string example =
        "<school>\n"
        "	<people>\n"
        "		<teacher name='Yuntianhe' level='expert'>\n"
        "		  <age>27</age>\n"
        "		  <sex>male</sex>\n"
        "		</teacher>\n"
        "		<teacher name='Hanlingsha'>\n"
        "		  <age>27</age>\n"
        "		  <sex>female</sex>\n"
        "		  <speciality>piano</speciality>\n"
        "		</teacher>\n"
        "		<teacher name='Liumengli' level='amateur'>\n"
        "		  <age>27</age>\n"
        "		  <sex>female</sex>\n"
        "		  <speciality>pipa</speciality>\n"
        "		</teacher>\n"
        "		<student name='Hu' job='monitor'>\n"
        "		  <age>15</age>\n"
        "		  <sex>male</sex>\n"
        "		</student>\n"
        "		<student name='zhang' job='representative'>\n"
        "		  <age>15</age>\n"
        "		  <sex>male</sex>\n"
        "		</student>\n"
        "	</people>\n"
        "</school>";

// 格式化输出XML字符串
inline std::string node_to_string(pugi::xml_node &&n) {
    std::stringstream ss;
    pugi::xml_writer_stream w(ss);
    n.print(w);
    return ss.str();
}

int main(int argc, char *argv[]) {
    pugi::xml_document doc;
    doc.load_string(example.c_str());
    // root节点对应的xml路径就是/，shcool节点对应的xml路径就是/school/
    pugi::xml_node root = doc.root();

    // 查找存在level属性的teacher节点
    pugi::xpath_node_set teacher_nodes = root.select_nodes("./school/people/teacher[@level]");
    for (auto iter = teacher_nodes.begin(); iter != teacher_nodes.end(); iter++) {
        std::cout << node_to_string(iter->node()) << std::endl;
    }

    // 查找存在level属性并且值为expert的teacher节点
    teacher_nodes = root.select_nodes("./school/people/teacher[@level='expert']");
    for (auto iter = teacher_nodes.begin(); iter != teacher_nodes.end(); iter++) {
        std::cout << node_to_string(iter->node()) << std::endl;
    }

    // 查找存在speciality元素的teacher节点
    teacher_nodes = root.select_nodes("./school/people/teacher/speciality/..");
    for (auto iter = teacher_nodes.begin(); iter != teacher_nodes.end(); iter++) {
        std::cout << node_to_string(iter->node()) << std::endl;
    }

    // 查找存在level属性并且存在speciality元素的teacher节点
    teacher_nodes = root.select_nodes("./school/people/teacher[@level]/speciality/..");
    for (auto iter = teacher_nodes.begin(); iter != teacher_nodes.end(); iter++) {
        std::cout << node_to_string(iter->node()) << std::endl;
    }

    // 查找所有男性节点
    pugi::xpath_node_set male_nodes = root.select_nodes("./school/people/child::*[sex='male']");
    for (auto iter = male_nodes.begin(); iter != male_nodes.end(); iter++) {
        std::cout << node_to_string(iter->node()) << std::endl;
    }

    // 随机为一个student新增属性look
    pugi::xml_node modify_node = root.select_single_node("./school/people/student").node();
    if (!modify_node.append_attribute("look").set_value("nice")) {
        std::cout << "append attribute failed!" << std::endl;
        return -1;
    }
    std::cout << node_to_string(std::move(root)) << std::endl;

    // 删除student添加的look属性
    modify_node = root.select_single_node("./school/people/student[@look]").node();
    if (!modify_node.remove_attribute("look")) {
        std::cout << "remove attribute failed!" << std::endl;
        return -1;
    }
    std::cout << node_to_string(std::move(root)) << std::endl;

    // 为teacher新增元素marry（加在末尾）
    modify_node = root.select_single_node("./school/people/teacher").node();
    if (!modify_node.append_child("marry").append_child(pugi::node_pcdata).set_value("yes")) {
        std::cout << "append child failed!" << std::endl;
        return -1;
    }
    std::cout << node_to_string(std::move(root)) << std::endl;

    // 删除teacher添加的元素
    pugi::xml_node delete_node = root.select_single_node("./school/people/teacher/marry/..").node();
    if (!delete_node.remove_child("marry")) {
        std::cout << "append child failed!" << std::endl;
        return -1;
    }
    std::cout << node_to_string(std::move(root)) << std::endl;

    //为teacher新增元素marry（加在首部）
    modify_node = root.select_single_node("./school/people/teacher").node();
    if (!modify_node.prepend_child("marry").prepend_child(pugi::node_pcdata).set_value("yes"))
    {
      std::cout << "append child failed!" << std::endl;
      return -1;
    }
    std::cout << node_to_string(std::move(root)) << std::endl;
    return 0;
}