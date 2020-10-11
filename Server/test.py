import requests
import os
from xml.etree import ElementTree

BASE = "http://127.0.0.1:2017/"


def readTestXML():
    file_name = 'alert.xml'
    full_file = os.path.abspath(file_name)
    tree = ElementTree.parse(full_file)
    root = tree.getroot()

    xmlstr = ElementTree.tostring(root, encoding='utf8', method='xml')
    # print(xmlstr)
    return xmlstr

    # param = root.findall('info/parameter')
    # print(param)
    # print('\n')
    # for p in param:
    #     name = p.find('valueName').text
    #     value = p.find('value').text
    #     print(name, value)


def fixed_xml_body_as_string():
    return """
    <payee>
        <name>John Smith</name>
        <address>
            <street>My street</street>
            <city>My city</city>
            <state>My state</state>
            <zipCode>90210</zipCode>
        </address>
        <phoneNumber>0123456789</phoneNumber>
        <accountNumber>12345</accountNumber>
    </payee>
    """


def sendPOST(xmlstr):
    response = requests.post(BASE + "data",
                             headers={"Content-Type": "application/xml"},
                             data=xmlstr
                             )
    print(response.json())
    assert response.status_code == 200
    # assert response.headers["Content-Type"] == "application/xml"


# test_send_xml_body_from_string_check_status_code_and_content_type()

xmlstr = readTestXML()
sendPOST(xmlstr)
