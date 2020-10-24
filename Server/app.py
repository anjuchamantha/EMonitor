from flask import Flask, request, render_template
from flask_sqlalchemy import SQLAlchemy
from flask_restful import Api, Resource
from xml.etree import ElementTree

app = Flask(__name__)

ENV = 'dev'

if ENV == 'dev':
    app.debug = True
    app.config['SQLALCHEMY_DATABASE_URI'] = 'postgresql://postgres:password@localhost/EMonitorDB'

else:
    debug = False
    app.config['SQLALCHEMY_DATABASE_URI'] = ''

app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)


class EMonitor(db.Model):
    __tablename__ = 'sensor_data'
    id = db.Column(db.Integer, primary_key=True)
    msg_id = db.Column(db.String(128))
    timestamp = db.Column(db.String(128))

    temperature = db.Column(db.Float)
    humidity = db.Column(db.Float)
    pressure = db.Column(db.Float)
    light = db.Column(db.Float)

    temperature_sd = db.Column(db.Float)
    humidity_sd = db.Column(db.Float)
    pressure_sd = db.Column(db.Float)
    light_sd = db.Column(db.Float)

    def __init__(self, msg_id, timestamp, temperature, humidity,
                 pressure, light, temperature_sd, humidity_sd, pressure_sd, light_sd):
        self.msg_id = msg_id
        self.timestamp = timestamp

        self.temperature = temperature
        self.humidity = humidity
        self.pressure = pressure
        self.light = light

        self.temperature_sd = temperature_sd
        self.humidity_sd = humidity_sd
        self.pressure_sd = pressure_sd
        self.light_sd = light_sd


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


def put_to_db(xml_data):
    msg_id = (xml_data['identifier'])
    timestamp = xml_data['datetime']

    temperature = xml_data['data']['temperature']
    humidity = xml_data['data']['humidity']
    pressure = xml_data['data']['pressure']
    light = xml_data['data']['lightIntensity']

    temperature_sd = xml_data['data']['temperature_sd']
    humidity_sd = xml_data['data']['humidity_sd']
    pressure_sd = xml_data['data']['pressure_sd']
    light_sd = xml_data['data']['lightIntensity_sd']

    try:
        DB_data = EMonitor(msg_id, timestamp, temperature, humidity, pressure, light,
                           temperature_sd, humidity_sd, pressure_sd, light_sd)
        db.session.add(DB_data)
        db.session.commit()
        return True

    except:
        return False


@app.route('/data', methods=['POST'])
def post():
    xml_str = request.data
    xml_data = extract_data_from_xml(xml_str)
    print("[POST] /data : ", xml_data)
    if put_to_db(xml_data):
        return "DATABASE updated"
    else:
        return "DATABASE modification failed !"


@app.route('/')
def index():
    table = EMonitor.query.all()
    return render_template("index.html", table=table)


if __name__ == "__main__":
    app.run(port=2017)
