import requests
import os
from xml.etree import ElementTree

# BASE = "http://127.0.0.1:2017/"
BASE = "http://a265a3bbc240.ngrok.io/"


def read_xml_file():
    """
    read test xml file(CAP) and return as a string
    :return: xml as a string
    """
    file_name = 'alert.xml'
    full_file = os.path.abspath(file_name)
    tree = ElementTree.parse(full_file)
    root = tree.getroot()

    return ElementTree.tostring(root, encoding='utf8', method='xml')


def send_post(xml):
    """
    send POST req with xml_str as a data param
    :param xml: xml as a string
    :return: None
    """
    response = requests.post(BASE + "data",
                             headers={"Content-Type": "application/xml"},
                             data=xml
                             )
    if response.status_code == 200:
        print("[200 OK] POST Successful")
    assert response.status_code == 200
    print(response.json())


xml_str = read_xml_file()
send_post(xml_str)
