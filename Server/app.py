from flask import Flask, request, render_template
from flask_restful import Api, Resource
from xml.etree import ElementTree

app = Flask(__name__)


def extract_data_from_xml(xml_str):
    """
    Given a XML string(CAP) this extracts the 'parameter' values
    :param xml_str: XML as a string
    :return: data as dictionary {name:value}
    """
    root = ElementTree.fromstring(xml_str)
    param = root.findall('info/parameter')
    datetime = root.find('sent').text.strip()
    identifier = root.find('identifier').text.strip()
    readings = dict()
    readings['datetime'] = datetime
    readings['identifier'] = identifier
    data = {}
    for p in param:
        name = p.find('valueName').text.strip()
        value = p.find('value').text.strip()
        data[name] = float(value)
    readings["data"] = data
    return readings


class Data(Resource):
    def post(self):
        xml_str = request.data
        # print(xml_str)
        data = extract_data_from_xml(xml_str)
        print("[POST] /data : ", data)
        return data


@app.route('/data', methods=['POST'])
def post():
    xml_str = request.data
    # print(xml_str)
    data = extract_data_from_xml(xml_str)
    print("[POST] /data : ", data)
    return data


@app.route('/')
def index():
    return render_template("index.html")


if __name__ == "__main__":
    app.run(debug=True, port=2017)
