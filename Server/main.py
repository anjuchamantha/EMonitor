from flask import Flask, request
from flask_restful import Api, Resource
from xml.etree import ElementTree

app = Flask(__name__)
api = Api(app)


def extract_data_from_xml(xml_str):
    """
    Given a XML string(CAP) this extracts the 'parameter' values
    :param xml_str: XML as a string
    :return: data as dictionary {name:value}
    """
    root = ElementTree.fromstring(xml_str)
    param = root.findall('info/parameter')
    data = {}
    for p in param:
        name = p.find('valueName').text
        value = p.find('value').text
        data[name] = float(value)
    return data


class Data(Resource):
    def post(self):
        xml_str = request.data
        data = extract_data_from_xml(xml_str)
        print("[POST] /data : ", data)
        return data


class Home(Resource):
    def get(self):
        return {"data": "Welcome to my site"}

    def post(self):
        return {"data": "Welcome to my site"}


api.add_resource(Home, "/")
api.add_resource(Data, "/data")

if __name__ == "__main__":
    app.run(debug=True, port=2017)
